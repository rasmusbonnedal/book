#include "serialize.h"

#include "utils.h"

#include <rapidxml.hpp>
#include <rapidxml_print.hpp>

#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

using namespace rapidxml;

namespace {
using StringPair = std::pair<std::string, std::string>;
using AttrVec = std::vector<StringPair>;

std::optional<std::string> getAttrStringOpt(xml_node<>* node,
                                            const std::string& name) {
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

xml_node<>* getNodeNothrow(xml_node<>* node, const std::string& name) {
    return node->first_node(name.c_str());
}

xml_node<>* getNode(xml_node<>* node, const std::string& name) {
    xml_node<>* n = node->first_node(name.c_str());
    if (!n) {
        throw std::runtime_error("Node " + name + " not found.");
    }
    return n;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

std::string toXmlText(const std::string& text) {
    std::string s = text;
    replaceAll(s, "&", "&amp;");
    replaceAll(s, "'", "&apos;");
    replaceAll(s, "\"", "&quot;");
    replaceAll(s, "<", "&lt;");
    replaceAll(s, ">", "&gt;");
    
    return s;
}

void writeXml(std::ostream& os, const std::string& indent, const std::string& node, const AttrVec& attrs, bool close = false, bool space = false, bool endl = true) {
    os << indent << "<" << node;
    for (const auto& attr : attrs) {
        os << " " << attr.first << "=\"" << attr.second << "\"";
    }
    if (space) {
        os << " ";
    }
    if (close) {
        os << "/";
    }
    os << ">";
    if (endl) {
        os << std::endl;
    }
}

template <class T>
void appendAttribute(std::unique_ptr<xml_document<>>& doc, xml_node<>* node,
                     const std::string& name, const T& value) {
    std::stringstream ss;
    ss << value;
    xml_attribute<>* attr =
        doc->allocate_attribute(doc->allocate_string(name.c_str()),
                                doc->allocate_string(ss.str().c_str()));
    node->append_attribute(attr);
}

std::string intToHex(const uint32_t x) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(8) << std::uppercase << x;
    return ss.str();
}
} // namespace

BollDoc Serialize::loadDocument(std::istream& input, bool ignoreChecksum) {
    auto doc = std::make_unique<xml_document<>>();
    uint32_t checksum;

    std::string xmlText(std::istreambuf_iterator<char>(input), {});
    checksum = Utils::calcChecksum(xmlText);
    doc->parse<0>(&xmlText[0]);

    auto bollbok = getNode(doc.get(), "bollbok");
    auto version = getAttrInt(bollbok, "version");
    auto kontrollsumma = getAttrString(bollbok, "kontrollsumma");
    if (!ignoreChecksum && kontrollsumma != intToHex(checksum)) {
        std::stringstream error;
        error << "Checksum error while loading document (" << intToHex(checksum)
              << " != " << kontrollsumma << ")";
        throw std::runtime_error(error.str());
    }
    auto info = getNode(bollbok, "info");
    auto firma = getAttrString(info, "firma");
    auto orgnummer = getAttrString(info, "orgnummer");
    auto bokforingsar = getAttrInt(info, "bokföringsår");
    auto valuta = getAttrString(info, "valuta");
    auto avslutat = getAttrStringOpt(info, "avslutat").value_or("") == "1";
    BollDoc bolldoc(version, firma, orgnummer, bokforingsar, valuta, avslutat);

    auto kontoplan = getNode(bollbok, "kontoplan");
    auto kptyp = getAttrStringOpt(kontoplan, "kptyp").value_or("");
    bolldoc.setKontoPlanTyp(kptyp);
    for (auto konto = getNodeNothrow(kontoplan, "konto"); konto;
         konto = konto->next_sibling("konto")) {
        int unid = getAttrInt(konto, "unid");
        auto text = getAttrString(konto, "text");
        auto typ = getAttrInt(konto, "typ");
        auto normalt = getAttrStringOpt(konto, "normalt");
        auto tagg = getAttrStringOpt(konto, "k1");
        BollDoc::Konto k(unid, std::move(text), typ, std::move(normalt),
                         std::move(tagg));
        bolldoc.addKonto(std::move(k));
    }

    auto kontogrupper = getNodeNothrow(bollbok, "kontogrupper");
    if (kontogrupper) {
        for (auto kontogrupp = getNodeNothrow(kontogrupper, "kontogrupp"); kontogrupp;
            kontogrupp = kontogrupp->next_sibling("kontogrupp")) {
            auto namn = getAttrString(kontogrupp, "namn");
            auto konton = getAttrString(kontogrupp, "konton");
            bolldoc.addKontoGrupp(std::make_pair(namn, konton));
        }
    }

    auto verifikationer = getNodeNothrow(bollbok, "verifikationer");
    for (auto verifikat = getNodeNothrow(verifikationer, "verifikat"); verifikat;
         verifikat = verifikat->next_sibling("verifikat")) {
        auto unid = getAttrInt(verifikat, "unid");
        auto text = getAttrString(verifikat, "text");
        auto transdatum = getAttrString(verifikat, "transdatum");
        BollDoc::Verifikat v(unid, std::move(text), parseDate(transdatum));
        for (auto rad = getNodeNothrow(verifikat, "rad"); rad;
             rad = rad->next_sibling("rad")) {
            auto bokdatum = getAttrString(rad, "bokdatum");
            auto konto = getAttrInt(rad, "konto");
            auto pengar = parsePengar(getAttrString(rad, "pengar"));
            auto struken = getAttrStringOpt(rad, "struken");
            std::optional<Date> strukenDate;
            if (struken)
                strukenDate = parseDate(*struken);
            BollDoc::Rad r{parseDate(bokdatum), konto, pengar, strukenDate};
            v.addRad(std::move(r));
        }
        bolldoc.addVerifikat(std::move(v));
    }

    return bolldoc;
}

void Serialize::saveDocumentCustom(const BollDoc& doc, std::ostream& output) {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>"
       << std::endl;
    ss << "<bollbok version=\"" << doc.getVersion() << "\" kontrollsumma=\"";
    size_t checksumpos = ss.tellp();
    ss << "00000000\">" << std::endl;

    std::string indent = "\t";
    writeXml(ss, indent, "info",
             {{"avslutat", doc.getAvslutat() ? "1" : "0"},
              {"firma", doc.getFirma()},
              {"orgnummer", doc.getOrgnummer()},
              {"bokföringsår", std::to_string(doc.getBokforingsar())},
              {"valuta", doc.getValuta()}},
             true, true);
    ss << std::endl;
    writeXml(ss, indent, "kontoplan", {{"kptyp", doc.getKontoPlanTyp()}});
    indent = "\t\t";
    for (const auto& konto : doc.getKontoPlan()) {
        AttrVec attrs = {{"unid", std::to_string(konto.second.getUnid())},
                         {"text", konto.second.getText()},
                         {"typ", std::to_string(konto.second.getTyp())}};
        if (konto.second.getTagg()) {
            attrs.push_back({"k1", *konto.second.getTagg()});
        }
        if (konto.second.getNormalt()) {
            attrs.push_back({"normalt", *konto.second.getNormalt()});
        }
        writeXml(ss, indent, "konto", attrs, true, true);
    }
    indent = "\t";
    ss << indent << "</kontoplan>" << std::endl;

    writeXml(ss, indent, "kontogrupper", {});
    indent = "\t\t";
    for (const auto& kontogrupp : doc.getKontoGrupper()) {
        writeXml(ss, indent, "kontogrupp",
                 {{"namn", kontogrupp.first}, {"konton", kontogrupp.second}},
                 true, false, false);
    }
    indent = "\t";
    ss << std::endl << indent << "</kontogrupper>" << std::endl;

    writeXml(ss, indent, "objektlista", {});
    ss << indent << "</objektlista>" << std::endl;

    writeXml(ss, indent, "verifikationer", {});
    indent = "\t\t";
    int last_unid = doc.getVerifikationer().back().getUnid();
    for (const auto& v : doc.getVerifikationer()) {
        // This is a hack to make sure the last "Nytt verifikat" empty
        // verifikat is not saved.
        if (v.getRader().empty() && v.getUnid() == last_unid) {
            continue;
        }
        writeXml(ss, indent, "verifikat",
                 {{"unid", std::to_string(v.getUnid())},
                  {"text", toXmlText(v.getText())},
                  {"transdatum", to_string(v.getTransdatum())}});
        indent = "\t\t\t";
        for (auto&& r : v.getRader()) {
            AttrVec attrs = {{"bokdatum", to_string(r.getBokdatum())},
                             {"konto", std::to_string(r.getKonto())},
                             {"pengar", toXmlString(r.getPengar())}};
            if (r.getStruken()) {
                attrs.push_back({"struken", to_string(*r.getStruken())});
            }
            writeXml(ss, indent, "rad", attrs, true);
        }
        indent = "\t\t";
        ss << indent << "</verifikat>" << std::endl;
    }
    indent = "\t";
    ss << indent << "</verifikationer>" << std::endl;
    ss << "</bollbok>";

    std::string xml = ss.str();
    std::string checksum = intToHex(Utils::calcChecksum(xml));
    for (int i = 0; i < 8; ++i) {
        xml[checksumpos + i] = checksum[i];
    }
    output << xml;
}

void Serialize::saveDocument(const BollDoc& bolldoc, std::ostream& output) {
    auto doc = std::make_unique<xml_document<>>();
    // xml declaration
    xml_node<>* decl = doc->allocate_node(node_declaration);
    decl->append_attribute(doc->allocate_attribute("version", "1.0"));
    decl->append_attribute(doc->allocate_attribute("encoding", "utf-8"));
    decl->append_attribute(doc->allocate_attribute("standalone", "yes"));
    doc->append_node(decl);

    xml_node<>* bollbok = doc->allocate_node(node_element, "bollbok");
    doc->append_node(bollbok);
    appendAttribute(doc, bollbok, "version", bolldoc.getVersion());

    xml_node<>* info = doc->allocate_node(node_element, "info");
    bollbok->append_node(info);
    if (bolldoc.getAvslutat()) {
        appendAttribute(doc, info, "avslutat", 1);
    }
    appendAttribute(doc, info, "firma", bolldoc.getFirma());
    appendAttribute(doc, info, "orgnummer", bolldoc.getOrgnummer());
    appendAttribute(doc, info, "bokföringsår", bolldoc.getBokforingsar());
    appendAttribute(doc, info, "valuta", bolldoc.getValuta());

    xml_node<>* kontoplan = doc->allocate_node(node_element, "kontoplan");
    bollbok->append_node(kontoplan);
    appendAttribute(doc, kontoplan, "kptyp", bolldoc.getKontoPlanTyp());

    for (auto&& k : bolldoc.getKontoPlan()) {
        const BollDoc::Konto& konto = k.second;
        xml_node<>* kontonode = doc->allocate_node(node_element, "konto");
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

    xml_node<>* kontogrupper = doc->allocate_node(node_element, "kontogrupper");
    bollbok->append_node(kontogrupper);
    for (auto&& kontogrupp : bolldoc.getKontoGrupper()) {
        xml_node<>* kontogruppnode = doc->allocate_node(node_element, "kontogrupp");
        kontogrupper->append_node(kontogruppnode);
        appendAttribute(doc, kontogruppnode, "namn", kontogrupp.first);
        appendAttribute(doc, kontogruppnode, "konton", kontogrupp.second);
    }

    xml_node<>* objektlista = doc->allocate_node(node_element, "objektlista");
    bollbok->append_node(objektlista);

    xml_node<>* verifikationer =
        doc->allocate_node(node_element, "verifikationer");
    bollbok->append_node(verifikationer);
    int last_unid = bolldoc.getVerifikationer().back().getUnid();
    for (auto&& v : bolldoc.getVerifikationer()) {
        // This is a hack to make sure the last "Nytt verifikat" empty
        // verifikat is not saved.
        if (v.getRader().empty() && v.getUnid() == last_unid) {
            continue;
        }
        xml_node<>* verifikat = doc->allocate_node(node_element, "verifikat");
        verifikationer->append_node(verifikat);
        appendAttribute(doc, verifikat, "unid", v.getUnid());
        appendAttribute(doc, verifikat, "text", v.getText());
        appendAttribute(doc, verifikat, "transdatum", v.getTransdatum());
        for (auto&& r : v.getRader()) {
            xml_node<>* rad = doc->allocate_node(node_element, "rad");
            verifikat->append_node(rad);
            appendAttribute(doc, rad, "bokdatum", r.getBokdatum());
            appendAttribute(doc, rad, "konto", r.getKonto());
            appendAttribute(doc, rad, "pengar", toXmlString(r.getPengar()));
            if (r.getStruken()) {
                appendAttribute(doc, rad, "struken", *r.getStruken());
            }
        }
    }

    std::stringstream ss;
    ss << *doc;
    uint32_t checksum = Utils::calcChecksum(ss.str());
    appendAttribute(doc, bollbok, "kontrollsumma",
                    doc->allocate_string(intToHex(checksum).c_str()));
    output << *doc;
}
