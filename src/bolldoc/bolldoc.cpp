#include "bolldoc.h"
#include "utils.h"

#include <optional>
#include <sstream>

BollDoc::BollDoc(int version,
                 std::string firma,
                 std::string orgnummer,
                 int bokforingsar,
                 std::string valuta,
                 bool avslutat)
: _version(version)
, _firma(std::move(firma))
, _orgnummer(std::move(orgnummer))
, _bokforingsar(bokforingsar)
, _valuta(std::move(valuta))
, _avslutat(avslutat) {
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

const std::map<int, BollDoc::Konto>& BollDoc::getKontoPlan() const {
    return _kontoplan;
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

const std::vector<BollDoc::Verifikat>& BollDoc::getVerifikationer() const {
    return _verifikat;
}

BollDoc::Konto::Konto(int unid, std::string text, int typ, std::optional<std::string> normalt, std::optional<std::string> tagg)
: _unid(unid)
, _text(std::move(text))
, _typ(typ)
, _normalt(std::move(normalt))
, _tagg(std::move(tagg)) {
}

int BollDoc::Konto::getUnid() const {
    return _unid;
}

const std::string& BollDoc::Konto::getText() const {
    return _text;
}

int BollDoc::Konto::getTyp() const {
    return _typ;
}

const std::optional<std::string>& BollDoc::Konto::getNormalt() const {
    return _normalt;
}

const std::optional<std::string>& BollDoc::Konto::getTagg() const {
    return _tagg;    
}

BollDoc::Rad::Rad(Date bokdatum, int konto, Pengar pengar, std::optional<Date> struken)
: _bokdatum(std::move(bokdatum))
, _konto(konto)
, _pengar(pengar)
, _struken(struken) {
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
