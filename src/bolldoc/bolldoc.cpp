#include "bolldoc.h"

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
    if (verifikat.getUnid() != getNextVerifikatId()) {
        std::stringstream ss;
        ss << "Verifikat has unid " << verifikat.getUnid() << ", should be " << _verifikat.size();
        throw std::runtime_error(ss.str());
    }
    checkYear(verifikat.getTransdatum());
    _verifikat.push_back(std::move(verifikat));
    setMutated();
}

void BollDoc::updateVerifikat(Verifikat&& verifikat) {
    // TODO: Limit this operation to verifikat created today!
    if (verifikat.getUnid() >= getNextVerifikatId()) {
        std::stringstream ss;
        ss << "Verifikat with unid " << verifikat.getUnid() << " does not exist";
        throw std::runtime_error(ss.str());
    }
    checkYear(verifikat.getTransdatum());
    _verifikat[verifikat.getUnid()] = std::move(verifikat);
    setMutated();
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
    return static_cast<int>(_verifikat.size());
}

const BollDoc::Verifikat& BollDoc::getVerifikat(int unid) const {
    checkVerifikatId(unid);
    return _verifikat[unid];
}

BollDoc::Verifikat& BollDoc::getVerifikatMut(int unid) {
    checkVerifikatId(unid);
    return _verifikat[unid];
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

void BollDoc::checkVerifikatId(int unid) const {
    int nextId = getNextVerifikatId();
    if (unid >= nextId || unid < 0) {
        std::stringstream ss;
        ss << "Verifikat " << unid << " requested, document only has 0-" << nextId - 1;
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

BollDoc::Verifikat::Verifikat(int unid, std::string text, Date transdatum)
    : _unid(unid), _text(std::move(text)), _transdatum(std::move(transdatum)) {}

bool BollDoc::Verifikat::operator==(const Verifikat& other) const {
    return _unid == other._unid && _text == other._text && _transdatum == other._transdatum && _rader == other._rader;
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
