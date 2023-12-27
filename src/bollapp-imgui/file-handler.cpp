#include "file-handler.h"

#include <malloc.h>
#include <nfd.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "bolldoc.h"
#include "report.h"
#include "serialize.h"
#include "sieparse.h"
#include "siewrite.h"

namespace {
Date int_to_date(int64_t date) {
    int year = (int)(date / 10000);
    int month = (date / 100) % 100;
    int day = date % 100;
    return Date(year, month, day);
}

int64_t date_to_int(const Date& date) {
    return date.getYear() * 10000 + date.getMonth() * 100 + date.getDay();
}

bool export_sie(const BollDoc& doc, const std::filesystem::path& siefile) {
    std::ofstream ofs(siefile);
    if (!ofs.is_open()) {
        std::cout << "Error: Opening " << siefile << std::endl;
        return false;
    }

    SIEData siedata;
    siedata.foretags_namn = doc.getFirma();
    siedata.org_nummer = doc.getOrgnummer();
    siedata.rakenskapsar_start = (int)date_to_int(Date(doc.getBokforingsar(), 1, 1));
    siedata.rakenskapsar_slut = (int)date_to_int(Date(doc.getBokforingsar(), 12, 31));

    for (const auto& [id, val] : doc.getKontoPlan()) {
        siedata.kontoplan[id] = SIEKonto(id, val.getText(), val.getSRU());
    }

    auto& balans_resultat = siedata.balans_resultat[0];
    for (const auto& rad : doc.getVerifikat(0).getRader()) {
        balans_resultat.ib[rad.getKonto()] = rad.getPengar().get();
    }

    BalansRapport balans;
    createBalansReport(doc, fullYear(doc.getBokforingsar()), balans);
    for (const auto& rad : balans.m_balans) {
        balans_resultat.ub[rad.m_konto] = rad.m_end.get();
    }

    ResultatRapport resultat;
    createResultatReport(doc, fullYear(doc.getBokforingsar()), resultat);
    for (const auto& rad : resultat.m_resultat) {
        balans_resultat.resultat[rad.first] = rad.second.get();
    }

    for (const auto& ver : doc.getVerifikationer()) {
        if (ver.getUnid() == 0) continue;
        SIEVerifikat sv;
        sv.transaktionsdatum = date_to_int(ver.getTransdatum());
        sv.bokforingsdatum = date_to_int(ver.getRad(0).getBokdatum());
        sv.id = ver.getUnid();
        sv.text = ver.getText();
        for (const auto& rad : ver.getRader()) {
            if (rad.getStruken()) {
                sv.rader.push_back(SIETransaktion(rad.getKonto(), rad.getPengar().get(), SIETransaktion::BTRANS));            
            } else {
                sv.rader.push_back(SIETransaktion(rad.getKonto(), rad.getPengar().get(), SIETransaktion::NORMAL));
            }
        }
        siedata.verifikat.push_back(sv);
    }

    return siewrite(siedata, date_to_int(now()), ofs);
}

std::unique_ptr<BollDoc> import_sie(const std::string& siefile) {
    std::ifstream ifs(siefile);
    if (!ifs.is_open()) {
        return 0;
    }
    SIEData siedata;
    if (!parse(siedata, ifs)) {
        return 0;
    }
    auto doc = std::make_unique<BollDoc>(1, siedata.foretags_namn, siedata.org_nummer, siedata.rakenskapsar_slut / 10000, "kr", false);
    if (siedata.fields.count("#KPTYP")) {
        const auto& kptyp = siedata.fields["#KPTYP"];
        if (kptyp.size() > 0) {
            doc->setKontoPlanTyp(kptyp[0]);
        }
    }

    for (const auto& [id, val] : siedata.kontoplan) {
        int first_digit = id / 1000;
        if (first_digit < 1 || first_digit > 9) {
            std::cerr << "Weird kontonr: " << id << std::endl;
            return 0;
        }
        int typ = first_digit <= 2 ? 1 : 3;
        BollDoc::Konto k(id, val.text, typ);
        if (val.sru >= 0) {
            k.getSRU() = val.sru;
            k.getTagg() = std::to_string(val.sru);
        }
        doc->addOrUpdateKonto(std::move(k));
    }

    BollDoc::Verifikat ib_ver(0, u8"Ingående saldon", Date(0, 1, 1));
    if (siedata.balans_resultat.count(0) > 0) {
        for (const auto& ib : siedata.balans_resultat[0].ib) {
            ib_ver.addRad(BollDoc::Rad(Date(0, 1, 1), ib.first, ib.second));
        }
    }
    doc->addVerifikat(std::move(ib_ver));

    for (const auto& verifikat : siedata.verifikat) {
        BollDoc::Verifikat bollver((int)verifikat.id, verifikat.text, int_to_date(verifikat.transaktionsdatum));
        for (const auto& sierad : verifikat.rader) {
            bollver.addRad(BollDoc::Rad(int_to_date(verifikat.bokforingsdatum), (int)sierad.konto, sierad.saldo));
        }
        doc->addVerifikat(std::move(bollver));
    }

    doc->setMutated();
    return doc;
}

std::vector<std::string> kvittoStrings(int unid) {
    std::vector<std::string> strs;

    std::string unid_str = "V" + std::to_string(unid);
    strs.push_back(unid_str);
    for (int i = 1; i < 10; ++i) {
        unid_str = "V" + std::to_string(unid) + "_" + std::to_string(i);
        strs.push_back(unid_str);
    }
    return strs;
}

}  // namespace

FileDialogResult fileOpenDialog(const std::string& filter, std::string& filename) {
    nfdchar_t* out_path = NULL;
    nfdresult_t result = NFD_OpenDialog(filter.c_str(), NULL, &out_path);
    if (result == NFD_OKAY) {
        filename = out_path;
        free(out_path);
    } else if (result == NFD_CANCEL) {
    } else {
        std::cout << "Error: " << NFD_GetError() << std::endl;
    }
    return (FileDialogResult)result;
}

FileDialogResult fileSaveDialog(const std::string& filter, std::string& filename) {
    nfdchar_t* out_path = NULL;
    nfdresult_t result = NFD_SaveDialog(filter.c_str(), NULL, &out_path);
    if (result == NFD_OKAY) {
        filename = out_path;
        free(out_path);
    } else if (result == NFD_CANCEL) {
    } else {
        std::cout << "Error: " << NFD_GetError() << std::endl;
    }
    return (FileDialogResult)result;
}

FileHandler::FileHandler() : _op(OP_NOP), _quit(false) {
    newFile();
}

FileHandler::~FileHandler() {}

void FileHandler::newFile() {
    _doc = std::make_unique<BollDoc>(1, "Rasmus", "123456-7890", 2013, "kr", false);
    _filename.clear();
}

FileHandler::OpenError FileHandler::open(std::string& chosen_file) {
    if (fileOpenDialog("bollbok", chosen_file) == FDR_OKAY) {
        return openFile(chosen_file, DO_CHECKSUM);
    }
    return OE_CANCEL;
}

FileHandler::OpenError FileHandler::openFile(const std::string& filename, ChecksumPolicy checkpol) {
    auto p = std::filesystem::u8path(filename);
    std::ifstream ifs(p);
    if (ifs.good()) {
        try {
            _doc = std::make_unique<BollDoc>(Serialize::loadDocument(ifs, checkpol == IGNORE_CHECKSUM));
        } catch (std::exception&) {
            return OE_CHECKSUM_ERROR;
        }
        _filename = p;
        return OE_SUCCESS;
    } else {
        std::cout << "Error: Opening " << filename << std::endl;
        return OE_FILE_ERROR;
    }
}

FileHandler::OpenError FileHandler::import_sie() {
    std::string siefile;
    if (fileOpenDialog("se", siefile) == FDR_OKAY) {
        auto doc = ::import_sie(siefile);
        if (doc) {
            _doc = std::move(doc);
            _filename.clear();
            return OE_SUCCESS;
        } else {
            return OE_FILE_ERROR;
        }
    }
    return OE_CANCEL;
}

bool FileHandler::save() {
    if (_filename.empty()) {
        return saveas();
    }
    std::ofstream ofs(_filename);
    Serialize::saveDocumentCustom(*_doc, ofs);
    _doc->clearDirty();
    return true;
}

bool FileHandler::saveas() {
    nfdchar_t* out_path = NULL;
    nfdresult_t result = NFD_SaveDialog("bollbok", NULL, &out_path);

    if (result == NFD_OKAY) {
        _filename = std::filesystem::u8path(out_path);
        free(out_path);
        return save();
    } else if (result == NFD_CANCEL) {
    } else {
        std::cout << "Error: " << NFD_GetError() << std::endl;
    }
    return false;
}

bool FileHandler::export_sie() {
    nfdchar_t* out_path = NULL;
    nfdresult_t result = NFD_SaveDialog("se", NULL, &out_path);

    if (result == NFD_OKAY) {
        auto siefile = std::filesystem::u8path(out_path);
        free(out_path);
        return ::export_sie(getDoc(), siefile);
    } else if (result == NFD_CANCEL) {
    } else {
        std::cout << "Error: " << NFD_GetError() << std::endl;
    }
    return false;
}


BollDoc& FileHandler::getDoc() {
    return *_doc;
}

std::vector<std::filesystem::path> FileHandler::getKvitton(int unid) const {
    std::vector<std::filesystem::path> kvitton;
    if (_filename.empty()) {
        return kvitton;
    }
    std::filesystem::path kvitto_dir = _filename;
    kvitto_dir.replace_extension("kvitton");
    if (std::filesystem::is_directory(kvitto_dir)) {
        for (const auto& kvitto_str : kvittoStrings(unid)) {
            std::filesystem::path p_pdf = kvitto_dir / (kvitto_str + ".pdf");
            std::filesystem::path p_png = kvitto_dir / (kvitto_str + ".png");
            if (std::filesystem::is_regular_file(p_pdf)) {
                kvitton.push_back(p_pdf);
            } else if (std::filesystem::is_regular_file(p_png)) {
                kvitton.push_back(p_png);
            }
        }
    }
    return kvitton;
}

bool FileHandler::canAttachKvitto() const {
    if (!_filename.empty()) {
        std::filesystem::path kvitto_dir = _filename;
        kvitto_dir.replace_extension("kvitton");
        if (std::filesystem::is_directory(kvitto_dir)) {
            return true;
        }
    }
    return false;
}

bool FileHandler::attachKvitto(int unid, const std::filesystem::path& kvitto) const {
    if (!_filename.empty()) {
        std::filesystem::path kvitto_dir = _filename;
        kvitto_dir.replace_extension("kvitton");
        if (std::filesystem::is_directory(kvitto_dir)) {
            for (const auto& kvitto_str : kvittoStrings(unid)) {
                std::filesystem::path p_pdf = kvitto_dir / (kvitto_str + ".pdf");
                std::filesystem::path p_png = kvitto_dir / (kvitto_str + ".png");
                if (std::filesystem::is_regular_file(p_pdf) || std::filesystem::is_regular_file(p_png)) {
                    continue;
                }
                std::filesystem::path p;
                if (kvitto.extension().string() == ".pdf") {
                    p = p_pdf;
                } else if (kvitto.extension().string() == ".png") {
                    p = p_png;
                } else {
                    continue;
                }
                std::error_code ec;
                std::filesystem::copy(kvitto, p, ec);
                if (ec) {
                    std::cout << "Could not copy from " << kvitto << " to " << p << std::endl;
                    return false;
                } else {
                    return true;
                }
            }
            std::cout << "Could not find suitable destination for kvitto file" << std::endl;
            return false;
        }
        std::cout << "Not a directory: " << kvitto_dir << std::endl;
        return false;
    }
    std::cout << "Could not attach kvitto before saving file" << std::endl;
    return false;
}


bool FileHandler::hasTitleChanged() {
    std::string title = getFilename();
    if (_doc->isDirty()) {
        title += " *";
    }
    if (title != _title) {
        _title = std::move(title);
        return true;
    }
    return false;
}

const std::string& FileHandler::getTitle() const {
    return _title;
}

const std::filesystem::path& FileHandler::getPath() const {
    return _filename;
}

std::string FileHandler::getFilename() const {
    if (_filename.empty()) {
        return "Untitled";
    } else {
        return _filename.filename().u8string();
    }
}

void FileHandler::setOp(Operation o, const std::string& arg) {
    _op = o;
    _op_arg = arg;
}

void FileHandler::cancelOp() {
    _op = OP_NOP;
}

bool FileHandler::shouldQuit() {
    return _quit;
}

FileHandler::OpenError FileHandler::performOp(std::string& chosen_file) {
    if (_op == OP_NEW) {
        newFile();
    } else if (_op == OP_OPEN) {
        return open(chosen_file);
    } else if (_op == OP_OPEN_FILE) {
        chosen_file = _op_arg;
        return openFile(_op_arg, DO_CHECKSUM);
    } else if (_op == OP_IMPORT) {
        return import_sie();
    } else if (_op == OP_QUIT) {
        _quit = true;
    }
    _op = OP_NOP;
    return OE_SUCCESS;
}
