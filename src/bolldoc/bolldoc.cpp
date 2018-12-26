#include "bolldoc.h"

#include <iostream>
#include <optional>
#include <sstream>

#include <rapidxml.hpp>

namespace {
    using namespace rapidxml;

    std::optional<std::string> getAttrStringOpt(xml_node<>* node, const std::string& name) {
        auto attr = node->first_attribute(name.c_str());
        return attr ? std::make_optional(attr->value()) : std::nullopt;
    }

    std::string getAttrString(xml_node<>* node, const std::string& name) {
        auto attr = getAttrStringOpt(node, name);
        if (!attr) {
            throw std::runtime_error("Attribute " + name + " not found.");
        }
        return *attr;
    }

    int getAttrInt(xml_node<>* node, const std::string& name) {
        return std::stoi(getAttrString(node, name));
    }

    xml_node<>* getNode(xml_node<>* node, const std::string& name) {
        auto n = node->first_node(name.c_str());
        if (!n) {
            throw std::runtime_error("Attribute " + name + " not found.");
        }
        return n;
    }
}

BollDoc loadDocument(std::istream& input) {
    using namespace rapidxml;
    std::string xmlText(std::istreambuf_iterator<char>(input), {});
    xml_document<> doc;
    doc.parse<0>(&xmlText[0]);

    auto bollbok = getNode(&doc, "bollbok");
    auto version = getAttrInt(bollbok, "version");
    auto info = getNode(bollbok, "info");
    auto firma = getAttrString(info, "firma");
    auto orgnummer = getAttrString(info, "orgnummer");
    auto bokforingsar = getAttrInt(info, "bokföringsår");
    auto valuta = getAttrString(info, "valuta");
    BollDoc bolldoc(version, firma, orgnummer, bokforingsar, valuta);

    auto kontoplan = getNode(bollbok, "kontoplan");
    for (auto konto = getNode(kontoplan, "konto"); konto; konto = konto->next_sibling("konto")) {
        int unid = getAttrInt(konto, "unid");
        auto text = getAttrString(konto, "text");
        auto typ = getAttrInt(konto, "typ");
        auto normalt = getAttrStringOpt(konto, "normalt");
        BollDoc::Konto k(unid, std::move(text), typ, std::move(normalt));
        bolldoc.addKonto(std::move(k));
    }

    return bolldoc;
}

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
