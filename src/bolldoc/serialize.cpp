#include "serialize.h"

#include "utils.h"

#include <rapidxml.hpp>

#include <iostream>
#include <optional>
#include <sstream>
#include <string>

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

    uint32_t hexToInt(const std::string& s) {
        uint32_t x;
        std::stringstream ss;
        ss << std::hex << s;
        ss >> x;
        return x;
    }
}

BollDoc Serialize::loadDocument(std::istream& input) {
    using namespace rapidxml;
    xml_document<> doc;
    uint32_t checksum;
    {
        std::string xmlText(std::istreambuf_iterator<char>(input), {});
        checksum = Utils::calcChecksum(xmlText);
        doc.parse<0>(&xmlText[0]);
    }

    auto bollbok = getNode(&doc, "bollbok");
    auto version = getAttrInt(bollbok, "version");
    auto kontrollsumma = hexToInt(getAttrString(bollbok, "kontrollsumma"));
    if (kontrollsumma != checksum) {
        throw std::runtime_error("Checksum error while loading document");
    }
    auto info = getNode(bollbok, "info");
    auto firma = getAttrString(info, "firma");
    auto orgnummer = getAttrString(info, "orgnummer");
    auto bokforingsar = getAttrInt(info, "bokföringsår");
    auto valuta = getAttrString(info, "valuta");
    BollDoc bolldoc(version, firma, orgnummer, bokforingsar, valuta);

    auto kontoplan = getNode(bollbok, "kontoplan");
    for (auto konto = getNode(kontoplan, "konto"); konto; konto = konto->next_sibling("konto")) {
        // TODO: attr k1
        int unid = getAttrInt(konto, "unid");
        auto text = getAttrString(konto, "text");
        auto typ = getAttrInt(konto, "typ");
        auto normalt = getAttrStringOpt(konto, "normalt");
        BollDoc::Konto k(unid, std::move(text), typ, std::move(normalt));
        bolldoc.addKonto(std::move(k));
    }

    auto verifikationer = getNode(bollbok, "verifikationer");
    for (auto verifikat = getNode(verifikationer, "verifikat"); verifikat; verifikat = verifikat->next_sibling("verifikat")) {
        auto unid = getAttrInt(verifikat, "unid");
        auto text = getAttrString(verifikat, "text");
        auto transdatum = getAttrString(verifikat, "transdatum");
        BollDoc::Verifikat v(unid, std::move(text), parseDate(transdatum));
        for (auto rad = getNode(verifikat, "rad"); rad; rad = rad->next_sibling("rad")) {
            // TODO: attr struken
            auto bokdatum = getAttrString(rad, "bokdatum");
            auto konto = getAttrInt(rad, "konto");
            auto pengar = Utils::parsePengar(getAttrString(rad, "pengar"));
            BollDoc::Rad r{parseDate(bokdatum), konto, pengar};
            v.addRad(std::move(r));
        }
        bolldoc.addVerifikat(std::move(v));
    }

    return bolldoc;
}
