#include "bolldoc.h"

#include <algorithm>
#include <optional>
#include <sstream>

#include "utils.h"

BollDoc::BollDoc(int version, std::string firma, std::string orgnummer, int bokforingsar, std::string valuta, bool avslutat)
    : _version(version),
      _firma(std::move(firma)),
      _orgnummer(std::move(orgnummer)),
      _bokforingsar(bokforingsar),
      _valuta(std::move(valuta)),
      _avslutat(avslutat),
      _dirty(false),
      _revision(0) {}

bool BollDoc::operator==(const BollDoc& other) const {
    return _version == other._version && _firma == other._firma && _orgnummer == other._orgnummer && _bokforingsar == other._bokforingsar &&
           _valuta == other._valuta && _avslutat == other._avslutat && _kontoplan == other._kontoplan && _verifikat == other._verifikat;
}

int BollDoc::getVersion() const {
    return _version;
}

const std::string& BollDoc::getFirma() const {
    return _firma;
}

const std::string& BollDoc::getOrgnummer() const {
    return _orgnummer;
}

int BollDoc::getBokforingsar() const {
    return _bokforingsar;
}

const std::string& BollDoc::getValuta() const {
    return _valuta;
}

bool BollDoc::getAvslutat() const {
    return _avslutat;
}

void BollDoc::addOrUpdateKonto(BollDoc::Konto&& konto) {
    _kontoplan.insert_or_assign(konto.getUnid(), std::move(konto));
    setMutated();
}

const BollDoc::Konto& BollDoc::getKonto(int unid) const {
    auto it = _kontoplan.find(unid);
    if (it == _kontoplan.end()) {
        std::stringstream ss;
        ss << "Could not find konto " << unid;
        throw std::runtime_error(ss.str());
    }
    return it->second;
}

const std::map<int, BollDoc::Konto>& BollDoc::getKontoPlan() const {
    return _kontoplan;
}

const std::string& BollDoc::getKontoPlanTyp() const {
    return _kptyp;
}

void BollDoc::setKontoPlanTyp(const std::string& kptyp) {
    _kptyp = kptyp;
    setMutated();
}

void BollDoc::addKontoGrupp(const std::pair<std::string, std::string>& kontogrupp) {
    _kontogrupper.push_back(kontogrupp);
    setMutated();
}

const std::vector<std::pair<std::string, std::string>>& BollDoc::getKontoGrupper() const {
    return _kontogrupper;
}

void BollDoc::addVerifikat(Verifikat&& verifikat) {
    if (verifikat.isBokforingsorder() && verifikat._unid < BokforingsorderIdStart) {
        // Older documents used the ordinary series for preliminary entries.
        verifikat._unid = getNextBokforingsorderId();
    }
    if (verifikat._unid < 0 ||
        (!verifikat.isBokforingsorder() && verifikat._unid >= BokforingsorderIdStart) ||
        std::any_of(_verifikat.begin(), _verifikat.end(), [&](const auto& v) {
            return v.getUnid() == verifikat.getUnid();
        })) {
        throw std::runtime_error("Invalid or duplicate verifikat id");
    }
    for (const auto& v : _verifikat) {
        if (v.getKvittoId() == verifikat._kvittoId) {
            int next = verifikat._kvittoId + 1;
            for (const auto& existing : _verifikat)
                next = std::max(next, existing.getKvittoId() + 1);
            verifikat._kvittoId = next;
            break;
        }
    }
    checkYear(verifikat.getTransdatum());
    _verifikat.push_back(std::move(verifikat));
    setMutated();
}

int BollDoc::updateVerifikat(Verifikat&& verifikat) {
    auto& existing = getVerifikatMut(verifikat.getUnid());
    checkYear(verifikat.getTransdatum());
    if (existing.getUnid() == 0 && verifikat.isBokforingsorder())
        throw std::runtime_error("Opening balances cannot be converted to a bokforingsorder");
    if (existing.isBokforingsorder() != verifikat.isBokforingsorder()) {
        verifikat._unid = verifikat.isBokforingsorder()
            ? getNextBokforingsorderId() : getNextVerifikatId();
    }
    // Receipt filenames remain stable when the accounting number changes.
    verifikat._kvittoId = existing._kvittoId;
    existing = std::move(verifikat);
    setMutated();
    return existing.getUnid();
}

void BollDoc::updateVerifikat(int unid, const std::vector<Rad>& rader) {
    getVerifikatMut(unid).update(rader);
    setMutated();
}

void BollDoc::setVerifikatTransdatum(int unid, const Date& date) {
    checkYear(date);
    getVerifikatMut(unid).setTransdatum(date);
    setMutated();
}

void BollDoc::setVerifikatText(int unid, const std::string& text) {
    getVerifikatMut(unid).setText(text);
    setMutated();
}

int BollDoc::getNextVerifikatId() const {
    int next = 0;
    for (const auto& v : _verifikat)
        if (!v.isBokforingsorder()) next = std::max(next, v.getUnid() + 1);
    if (next >= BokforingsorderIdStart)
        throw std::runtime_error("Verifikat id series exhausted");
    return next;
}

int BollDoc::getNextBokforingsorderId() const {
    int next = BokforingsorderIdStart;
    for (const auto& v : _verifikat)
        if (v.isBokforingsorder()) next = std::max(next, v.getUnid() + 1);
    return next;
}

const BollDoc::Verifikat& BollDoc::getVerifikat(int unid) const {
    auto it = std::find_if(_verifikat.begin(), _verifikat.end(),
                         [unid](const auto& v) { return v.getUnid() == unid; });
    if (it == _verifikat.end())
        throw std::runtime_error("Could not find verifikat " + std::to_string(unid));
    return *it;
}

BollDoc::Verifikat& BollDoc::getVerifikatMut(int unid) {
    return const_cast<Verifikat&>(static_cast<const BollDoc&>(*this).getVerifikat(unid));
}

const std::vector<BollDoc::Verifikat>& BollDoc::getVerifikationer() const {
    return _verifikat;
}

std::vector<const BollDoc::Verifikat*> BollDoc::getVerifikatRange(const Date& start, const Date& end) const {
    std::vector<const Verifikat*> retval;
    for (auto& v : _verifikat) {
        if (v.getTransdatum() >= start && v.getTransdatum() < end) {
            retval.push_back(&v);
        }
    }
    return retval;
}

std::vector<const BollDoc::Verifikat*> BollDoc::getVerifikatRange(const int unidStart, const int unidEnd) const {
    std::vector<const Verifikat*> retval;
    for (auto& v : _verifikat) {
        if (v.getUnid() >= unidStart && v.getUnid() < unidEnd) {
            retval.push_back(&v);
        }
    }
    return retval;
}

void BollDoc::checkYear(const Date& date) const {
    if (date.getYear() != 0 && date.getYear() != _bokforingsar) {
        std::stringstream ss;
        ss << "Wrong year in verifikat, document has year " << _bokforingsar << " and verifikat has year " << date.getYear();
        throw std::runtime_error(ss.str());
    }
}

BollDoc::Konto::Konto(int unid, std::string text, int typ, std::string normalt, std::string tagg)
    : _unid(unid), _text(std::move(text)), _typ(typ), _normalt(std::move(normalt)), _tagg(std::move(tagg)), _sru(-1) {}

bool BollDoc::Konto::operator==(const Konto& other) const {
    return _unid == other._unid && _text == other._text && _typ == other._typ && _normalt == other._normalt && _tagg == other._tagg;
}

int BollDoc::Konto::getUnid() const {
    return _unid;
}

const std::string& BollDoc::Konto::getText() const {
    return _text;
}

std::string& BollDoc::Konto::getText() {
    return _text;
}

int BollDoc::Konto::getTyp() const {
    return _typ;
}

int& BollDoc::Konto::getTyp() {
    return _typ;
}

const std::string& BollDoc::Konto::getNormalt() const {
    return _normalt;
}

std::string& BollDoc::Konto::getNormalt() {
    return _normalt;
}

const std::string& BollDoc::Konto::getTagg() const {
    return _tagg;
}

std::string& BollDoc::Konto::getTagg() {
    return _tagg;
}

int BollDoc::Konto::getSRU() const {
    return _sru;
}

int& BollDoc::Konto::getSRU() {
    return _sru;
}


BollDoc::Rad::Rad(Date bokdatum, int konto, Pengar pengar, std::optional<Date> struken)
    : _bokdatum(std::move(bokdatum)), _konto(konto), _pengar(pengar), _struken(struken) {}

bool BollDoc::Rad::operator==(const Rad& other) const {
    return _bokdatum == other._bokdatum && _konto == other._konto && _pengar == other._pengar && _struken == other._struken;
}

const Date& BollDoc::Rad::getBokdatum() const {
    return _bokdatum;
}

int BollDoc::Rad::getKonto() const {
    return _konto;
}

Pengar BollDoc::Rad::getPengar() const {
    return _pengar;
}

const std::optional<Date>& BollDoc::Rad::getStruken() const {
    return _struken;
}

BollDoc::Verifikat::Verifikat(int unid, std::string text, Date transdatum, bool bokforingsorder, int kvittoId)
    : _unid(unid), _text(std::move(text)), _transdatum(std::move(transdatum)), _bokforingsorder(bokforingsorder), _kvittoId(kvittoId < 0 ? unid : kvittoId) {}

bool BollDoc::Verifikat::operator==(const Verifikat& other) const {
    return _unid == other._unid && _text == other._text && _transdatum == other._transdatum &&
           _bokforingsorder == other._bokforingsorder && _kvittoId == other._kvittoId && _rader == other._rader;
}

int BollDoc::Verifikat::getUnid() const {
    return _unid;
}

const std::string& BollDoc::Verifikat::getText() const {
    return _text;
}

std::string& BollDoc::Verifikat::getText() {
    return _text;
}

void BollDoc::Verifikat::setText(const std::string& text) {
    _text = text;
}

const Date& BollDoc::Verifikat::getTransdatum() const {
    return _transdatum;
}

void BollDoc::Verifikat::setTransdatum(const Date& date) {
    _transdatum = date;
}

bool BollDoc::Verifikat::isBokforingsorder() const {
    return _bokforingsorder;
}

int BollDoc::Verifikat::getKvittoId() const {
    return _kvittoId;
}

void BollDoc::Verifikat::promoteToVerifikat() {
    _bokforingsorder = false;
}

bool BollDoc::Verifikat::canConvertToBokforingsorder(const Date& date) const {
    return !_bokforingsorder &&
           std::all_of(_rader.begin(), _rader.end(), [&date](const Rad& rad) {
               return rad.getBokdatum() == date;
           });
}

void BollDoc::Verifikat::convertToBokforingsorder(const Date& date) {
    if (!canConvertToBokforingsorder(date)) {
        throw std::runtime_error(
            "Only a verifikat whose rows were entered on the conversion date can be converted to a bokforingsorder");
    }
    _bokforingsorder = true;
}

void BollDoc::Verifikat::addRad(Rad&& rad) {
    _rader.push_back(std::move(rad));
}

void BollDoc::Verifikat::update(const std::vector<Rad>& rader) {
    _rader = rader;
}

const BollDoc::Rad& BollDoc::Verifikat::getRad(int i) const {
    if (i >= (int)_rader.size() || i < 0) {
        std::stringstream ss;
        ss << "Rad " << i << " requested, verifikat only has 0-" << _rader.size() - 1;
        throw std::runtime_error(ss.str());
    }
    return _rader[i];
}

const std::vector<BollDoc::Rad>& BollDoc::Verifikat::getRader() const {
    return _rader;
}

bool BollDoc::Verifikat::getOmslutning(Pengar& omslutning) const {
    Pengar sum;
    omslutning = 0;
    for (auto& r : _rader) {
        if (!r.getStruken()) {
            sum += r.getPengar();
            if (r.getPengar().get() > 0) {
                omslutning += r.getPengar();
            }
        }
    }
    if (sum.get() != 0) {
        omslutning = sum;
        return false;
    }
    return true;
}

BollDoc BollDoc::newYear() const {
    BollDoc doc(*this);
    doc._bokforingsar++;
    doc._verifikat.clear();
    Verifikat v(0, "Ingående saldon", Date(0, 1, 1));
    std::map<int, Pengar> balans;
    for (auto& ver : getVerifikationer()) {
        for (auto& rad : ver.getRader()) {
            if (!rad.getStruken() && getKontoPlan().at(rad.getKonto()).getTyp() == 1) {
                balans[rad.getKonto()] += rad.getPengar();
            }
        }
    }
    for (auto& konto : balans) {
        v.addRad(Rad(now(), konto.first, konto.second));
    }
    doc.addVerifikat(std::move(v));
    doc.setMutated();
    return doc;
}

std::ostream& operator<<(std::ostream& stream, const BollDoc::Rad& rad) {
    stream << rad.getBokdatum() << ": " << rad.getKonto() << " " << rad.getPengar();
    auto struken = rad.getStruken();
    if (struken) {
        stream << " (struken " << *struken << ")";
    }
    return stream;
}

void BollDoc::setMutated() {
    _dirty = true;
    _revision++;
}

bool BollDoc::isDirty() const {
    return _dirty;
}

void BollDoc::clearDirty() {
    _dirty = false;
}

int64_t BollDoc::getRevision() const {
    return _revision;
}

void BollDoc::useVerifikatIdsForKvitton() {
    for (auto& v : _verifikat) v._kvittoId = v._unid;
    setMutated();
}
