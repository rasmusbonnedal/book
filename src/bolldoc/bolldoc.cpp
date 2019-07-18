#include "bolldoc.h"
#include "utils.h"

#include <optional>
#include <sstream>

BollDoc::BollDoc(int version,
                 std::string firma,
                 std::string orgnummer,
                 int bokforingsar,
                 std::string valuta)
: _version(version)
, _firma(std::move(firma))
, _orgnummer(std::move(orgnummer))
, _bokforingsar(bokforingsar)
, _valuta(std::move(valuta)) {
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

void BollDoc::addKonto(BollDoc::Konto&& konto) {
    _kontoplan.insert(std::make_pair(konto.getUnid(), std::move(konto)));
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

void BollDoc::addVerifikat(Verifikat&& verifikat) {
    if (verifikat.getUnid() != _verifikat.size()) {
        std::stringstream ss;
        ss << "Verifikat has unid " << verifikat.getUnid() << ", should be " << _verifikat.size();
        throw std::runtime_error(ss.str());
    }
    _verifikat.push_back(std::move(verifikat));
}

const BollDoc::Verifikat& BollDoc::getVerifikat(int unid) const {
    if (unid >= _verifikat.size() || unid < 0) {
        std::stringstream ss;
        ss << "Verifikat " << unid << " requested, document only has 0-" << _verifikat.size() - 1;
        throw std::runtime_error(ss.str());
    }
    return _verifikat[unid];
}

const std::vector<BollDoc::Verifikat>& BollDoc::getVerifikat() const {
    return _verifikat;
}

BollDoc::Konto::Konto(int unid, std::string text, int typ, std::optional<std::string> normalt)
: _unid(unid)
, _text(std::move(text))
, _typ(typ)
, _normalt(std::move(normalt)) {
}

int BollDoc::Konto::getUnid() const {
    return _unid;
}

const std::string BollDoc::Konto::getText() const {
    return _text;
}

int BollDoc::Konto::getTyp() const {
    return _typ;
}

std::optional<std::string> BollDoc::Konto::getNormalt() const {
    return _normalt;
}

BollDoc::Rad::Rad(Date bokdatum, int konto, int64_t pengar)
: _bokdatum(std::move(bokdatum))
, _konto(konto)
, _pengar(pengar) {
}

const Date& BollDoc::Rad::getBokdatum() const {
    return _bokdatum;
}

int BollDoc::Rad::getKonto() const {
    return _konto;
}

int64_t BollDoc::Rad::getPengar() const {
    return _pengar;
}

BollDoc::Verifikat::Verifikat(int unid, std::string text, Date transdatum)
: _unid(unid)
, _text(std::move(text))
, _transdatum(std::move(transdatum)) {
}

int BollDoc::Verifikat::getUnid() const {
    return _unid;
}

const std::string& BollDoc::Verifikat::getText() const {
    return _text;
}

const Date& BollDoc::Verifikat::getTransdatum() const {
    return _transdatum;
}

void BollDoc::Verifikat::addRad(Rad&& rad) {
    _rader.push_back(std::move(rad));
}

const BollDoc::Rad& BollDoc::Verifikat::getRad(int i) const {
    if (i >= _rader.size() || i < 0) {
        std::stringstream ss;
        ss << "Rad " << i << " requested, verifikat only has 0-" << _rader.size() - 1;
        throw std::runtime_error(ss.str());
    }
    return _rader[i];
}

const std::vector<BollDoc::Rad>& BollDoc::Verifikat::getRader() const {
    return _rader;
}

