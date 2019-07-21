#include "serialize.h"

#include "utils.h"

#include <rapidxml.hpp>
#include <rapidxml_print.hpp>

#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

using namespace rapidxml;

namespace {
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

    template<class T>
    void appendAttribute(xml_document<>& doc, xml_node<>* node, const std::string& name, const T& value) {
        std::stringstream ss;
        ss << value;
        xml_attribute<> *attr = doc.allocate_attribute(
            doc.allocate_string(name.c_str()),
            doc.allocate_string(ss.str().c_str()));
        node->append_attribute(attr);
    }

    std::string intToHex(const uint32_t x) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(8) << std::uppercase << x;
        return ss.str();
    }
}

BollDoc Serialize::loadDocument(std::istream& input) {
    xml_document<> doc;
    uint32_t checksum;
    {
        std::string xmlText(std::istreambuf_iterator<char>(input), {});
        checksum = Utils::calcChecksum(xmlText);
        doc.parse<0>(&xmlText[0]);
    }

    auto bollbok = getNode(&doc, "bollbok");
    auto version = getAttrInt(bollbok, "version");
    auto kontrollsumma = getAttrString(bollbok, "kontrollsumma");
    if (kontrollsumma != intToHex(checksum)) {
        throw std::runtime_error("Checksum error while loading document");
    }
    auto info = getNode(bollbok, "info");
    auto firma = getAttrString(info, "firma");
    auto orgnummer = getAttrString(info, "orgnummer");
    auto bokforingsar = getAttrInt(info, "bokföringsår");
    auto valuta = getAttrString(info, "valuta");
    auto avslutat = getAttrStringOpt(info, "avslutat").value_or("") == "1";
    BollDoc bolldoc(version, firma, orgnummer, bokforingsar, valuta, avslutat);

    auto kontoplan = getNode(bollbok, "kontoplan");
    for (auto konto = getNode(kontoplan, "konto"); konto; konto = konto->next_sibling("konto")) {
        int unid = getAttrInt(konto, "unid");
        auto text = getAttrString(konto, "text");
        auto typ = getAttrInt(konto, "typ");
        auto normalt = getAttrStringOpt(konto, "normalt");
        auto tagg = getAttrStringOpt(konto, "k1");
        BollDoc::Konto k(unid, std::move(text), typ, std::move(normalt), std::move(tagg));
        bolldoc.addKonto(std::move(k));
    }

    auto verifikationer = getNode(bollbok, "verifikationer");
    for (auto verifikat = getNode(verifikationer, "verifikat"); verifikat; verifikat = verifikat->next_sibling("verifikat")) {
        auto unid = getAttrInt(verifikat, "unid");
        auto text = getAttrString(verifikat, "text");
        auto transdatum = getAttrString(verifikat, "transdatum");
        BollDoc::Verifikat v(unid, std::move(text), parseDate(transdatum));
        for (auto rad = getNode(verifikat, "rad"); rad; rad = rad->next_sibling("rad")) {
            auto bokdatum = getAttrString(rad, "bokdatum");
            auto konto = getAttrInt(rad, "konto");
            auto pengar = parsePengar(getAttrString(rad, "pengar"));
            auto struken = getAttrStringOpt(rad, "struken");
            std::optional<Date> strukenDate;
            if (struken) strukenDate = parseDate(*struken);
            BollDoc::Rad r{parseDate(bokdatum), konto, pengar, strukenDate};
            v.addRad(std::move(r));
        }
        bolldoc.addVerifikat(std::move(v));
    }

    return bolldoc;
}

void Serialize::saveDocument(const BollDoc& bolldoc, std::ostream& output) {
    xml_document<> doc;

    // xml declaration
    xml_node<>* decl = doc.allocate_node(node_declaration);
    decl->append_attribute(doc.allocate_attribute("version", "1.0"));
    decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
    decl->append_attribute(doc.allocate_attribute("standalone", "yes"));
    doc.append_node(decl);

    xml_node<>* bollbok = doc.allocate_node(node_element, "bollbok");
    doc.append_node(bollbok);
    appendAttribute(doc, bollbok, "version", bolldoc.getVersion());

    xml_node<>* info = doc.allocate_node(node_element, "info");
    bollbok->append_node(info);
    if (bolldoc.getAvslutat()) {
        appendAttribute(doc, info, "avslutat", 1);
    }
    appendAttribute(doc, info, "firma", bolldoc.getFirma());
    appendAttribute(doc, info, "orgnummer", bolldoc.getOrgnummer());
    appendAttribute(doc, info, "bokföringsår", bolldoc.getBokforingsar());
    appendAttribute(doc, info, "valuta", bolldoc.getValuta());

    xml_node<>* kontoplan = doc.allocate_node(node_element, "kontoplan");
    bollbok->append_node(kontoplan);
    for (auto&& k : bolldoc.getKontoPlan()) {
        const BollDoc::Konto& konto = k.second;
        xml_node<>* kontonode = doc.allocate_node(node_element, "konto");
        kontoplan->append_node(kontonode);
        appendAttribute(doc, kontonode, "unid", konto.getUnid());
        appendAttribute(doc, kontonode, "text", konto.getText());
        appendAttribute(doc, kontonode, "typ", konto.getTyp());
        if (konto.getTagg()) {
            appendAttribute(doc, kontonode, "k1", *konto.getTagg());
        }
        if (konto.getNormalt()) {
            appendAttribute(doc, kontonode, "normalt", *konto.getNormalt());
        }
    }

    xml_node<>* verifikationer = doc.allocate_node(node_element, "verifikationer");
    bollbok->append_node(verifikationer);
    for (auto&& v : bolldoc.getVerifikationer()) {
        xml_node<>* verifikat = doc.allocate_node(node_element, "verifikat");
        verifikationer->append_node(verifikat);
        appendAttribute(doc, verifikat, "unid", v.getUnid());
        appendAttribute(doc, verifikat, "text", v.getText());
        appendAttribute(doc, verifikat, "transdatum", v.getTransdatum());
        for (auto&& r : v.getRader()) {
            xml_node<>* rad = doc.allocate_node(node_element, "rad");
            verifikat->append_node(rad);
            appendAttribute(doc, rad, "bokdatum", r.getBokdatum());
            appendAttribute(doc, rad, "konto", r.getKonto());
            appendAttribute(doc, rad, "pengar", r.getPengar());
            if (r.getStruken()) {
                appendAttribute(doc, rad, "struken", *r.getStruken());
            }
        }
    }

    std::stringstream ss;
    ss << doc;
    uint32_t checksum = Utils::calcChecksum(ss.str());
    appendAttribute(doc, bollbok, "kontrollsumma", doc.allocate_string(intToHex(checksum).c_str()));
    output << doc;
}
