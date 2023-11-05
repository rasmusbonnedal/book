#include <doctest.h>

#include "bolldoc.h"
#include "moms.h"

#include <vector>

namespace {
BollDoc createDoc() {
    BollDoc doc(2074, "Ruffel & Båg", "551122-1234", 2018, "SEK", false);

    doc.addOrUpdateKonto(BollDoc::Konto(1910, "Kassa", 1));
    doc.addOrUpdateKonto(BollDoc::Konto(5010, "Hyra", 3));

    doc.addVerifikat({0, "Ingående saldon", Date(0, 1, 1)});
    for (int i = 1; i <= 12; ++i) {
        for (int j = 0; j < 10; ++j) {
            BollDoc::Verifikat v{(i - 1) * 10 + j + 1, "Hyra", Date(2018, i, 1)};
            v.addRad({Date(2018, 12, 25), 1910 + j, Pengar(i * 100 + j * 10)});
            v.addRad({Date(2018, 12, 25), 5010 + j, Pengar(-i * 100 + j * 10)});
            doc.addVerifikat(std::move(v));
        }
    }
    doc.addOrUpdateKonto({0, "Foo", 0});
    return doc;
}

using Huvudbok = std::vector<std::vector<std::pair<int, Pengar>>>;

BollDoc createDoc(const Huvudbok& huvudbok, int year, int month) {
    BollDoc doc(2074, "Foo", "551122-1234", year, "SEK", false);
    doc.addVerifikat({0, "Ingående saldon", Date(0, 1, 1)});
    int i = 1;
    for (auto ver : huvudbok) {
        BollDoc::Verifikat v(i++, "Foo", Date(year, month, 01));
        Pengar sum(0);
        for (auto rad : ver) {
            v.addRad(BollDoc::Rad(Date(year, month, 01), rad.first, rad.second));
            sum += rad.second;
        }
        CHECK(sum == 0);
        doc.addVerifikat(std::move(v));
    }
    return doc;
}

const KontoMap konto_map{{05, {3011}}, {10, {2611}}, {42, {3014, 3690}}, {48, {2641}}};
const FieldToSkv field_to_skv = {{5, "ForsMomsEjAnnan"}, {10, "MomsUtgHog"}, {42, "ForsOvrigt"}, {48, "MomsIngAvdr"}, {49, "MomsBetala"}};

}  // namespace

TEST_CASE("Moms") {
    auto doc = createDoc();
    KontoMap konto_map{{51, {1910}}, {52, {1911, 1912, 1913}}, {53, {1914, 1915}}};
    Verifikat redovisning;
    FieldSaldo result = summarize_moms(doc, 3, konto_map, redovisning);
    CHECK(result[51] == Pengar(300));
    CHECK(result[52] == Pengar(960));
    CHECK(result[53] == Pengar(690));
}

TEST_CASE("Invalid date") {
    auto doc = createDoc();
    KontoMap konto_map;
    Verifikat redovisning;
    CHECK_THROWS(summarize_moms(doc, 0, konto_map, redovisning));
    CHECK_THROWS(summarize_moms(doc, 13, konto_map, redovisning));
}

const std::string ref_aug2021 = R"(<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eSKDUpload PUBLIC "-//Skatteverket, Sweden//DTD Skatteverket eSKDUpload-DTD Version 6.0//SV" "https://www.skatteverket.se/download/18.3f4496fd14864cc5ac99cb1/1415022101213/eSKDUpload_6p0.dtd">
<eSKDUpload Version="6.0">
  <OrgNr>551122-1234</OrgNr>
  <Moms>
    <Period>201808</Period>
    <MomsBetala>0</MomsBetala>
  </Moms>
</eSKDUpload>
)";

TEST_CASE("eSKD empty") {
    auto doc = createDoc();
    FieldSaldo result;
    sum_moms(result);
    std::string xml = gen_moms_eskd(doc, 8, result, field_to_skv);
    CHECK(xml == ref_aug2021);
}

const std::string ref_nov2021 = R"(<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eSKDUpload PUBLIC "-//Skatteverket, Sweden//DTD Skatteverket eSKDUpload-DTD Version 6.0//SV" "https://www.skatteverket.se/download/18.3f4496fd14864cc5ac99cb1/1415022101213/eSKDUpload_6p0.dtd">
<eSKDUpload Version="6.0">
  <OrgNr>551122-1234</OrgNr>
  <Moms>
    <Period>201803</Period>
    <ForsMomsEjAnnan>230162</ForsMomsEjAnnan>
    <MomsUtgHog>57540</MomsUtgHog>
    <MomsIngAvdr>557</MomsIngAvdr>
    <MomsBetala>56983</MomsBetala>
  </Moms>
</eSKDUpload>
)";

TEST_CASE("Moms nov 2021") {
    auto doc = createDoc();

    FieldSaldo field_saldo = {{5, Pengar(-23016285)}, {10, Pengar(-5754072)}, {48, Pengar(55700)}};
    sum_moms(field_saldo);
    CHECK(field_saldo[49] == -5698300);

    std::string xml = gen_moms_eskd(doc, 3, field_saldo, field_to_skv);
    CHECK(xml == ref_nov2021);
}

Huvudbok dec_2021 = {
    {{1930, -5994710},
     {2641, 2040},
     {2641, 2560},
     {2710, -1296000},
     {2730, -2262240},
     {5410, 10240},
     {5810, 8160},
     {7220, 7200000},
     {7331, 67710},
     {7510, 2262240}},
    {{1930, -78125}, {5960, 78125}},
    {{1930, -6600}, {5960, 6600}},
    {{1930, -248600}, {2641, 49720}, {7631, 198880}},
    {{1510, 13631100}, {2611, -2726210}, {3011, -10904841}, {3740, -49}},
    {{1510, 9555500}, {2611, -1911107}, {3011, -7644429}, {3740, 36}},
};

const std::string ref_dec2021 = R"(<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eSKDUpload PUBLIC "-//Skatteverket, Sweden//DTD Skatteverket eSKDUpload-DTD Version 6.0//SV" "https://www.skatteverket.se/download/18.3f4496fd14864cc5ac99cb1/1415022101213/eSKDUpload_6p0.dtd">
<eSKDUpload Version="6.0">
  <OrgNr>551122-1234</OrgNr>
  <Moms>
    <Period>202112</Period>
    <ForsMomsEjAnnan>185492</ForsMomsEjAnnan>
    <MomsUtgHog>46373</MomsUtgHog>
    <MomsIngAvdr>543</MomsIngAvdr>
    <MomsBetala>45830</MomsBetala>
  </Moms>
</eSKDUpload>
)";

TEST_CASE("Moms dec 2021") {
    BollDoc doc = createDoc(dec_2021, 2021, 12);

    Verifikat redovisning;
    FieldSaldo result = summarize_moms(doc, 12, konto_map, redovisning);

    CHECK(result[5] == -18549270);
    CHECK(result[10] == -4637317);
    CHECK(result[48] == 54320);
    CHECK(result[49] == -4583000);

    CHECK(redovisning[2611] == 4637317);
    CHECK(redovisning[2641] == -54320);
    CHECK(redovisning[2650] == -4583000);
    CHECK(redovisning[3740] == 3);

    std::string xml = gen_moms_eskd(doc, 12, result, field_to_skv);
    CHECK(xml == ref_dec2021);
}

Huvudbok jan_2022 = {{{1930, -22900}, {2641, 4600}, {5410, 18300}}, {{1930, -89400}, {2641, 17880}, {5410, 35000}, {6210, 36520}}};
const std::string ref_jan2022 = R"(<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eSKDUpload PUBLIC "-//Skatteverket, Sweden//DTD Skatteverket eSKDUpload-DTD Version 6.0//SV" "https://www.skatteverket.se/download/18.3f4496fd14864cc5ac99cb1/1415022101213/eSKDUpload_6p0.dtd">
<eSKDUpload Version="6.0">
  <OrgNr>551122-1234</OrgNr>
  <Moms>
    <Period>202201</Period>
    <MomsIngAvdr>224</MomsIngAvdr>
    <MomsBetala>-224</MomsBetala>
  </Moms>
</eSKDUpload>
)";

TEST_CASE("Moms jan 2022") {
    BollDoc doc = createDoc(jan_2022, 2022, 1);
    Verifikat redovisning;
    FieldSaldo result = summarize_moms(doc, 1, konto_map, redovisning);
    CHECK(redovisning[1650] == 22400);
    CHECK(redovisning[2641] == -22480);
    CHECK(redovisning[3740] == 80);
    CHECK(result[49] == 22400);

    std::string xml = gen_moms_eskd(doc, 1, result, field_to_skv);
    CHECK(xml == ref_jan2022);
}

Huvudbok dec_2022 = {{{2896, 103}, {3690, -103}},
                     {{2641, 11995}, {1933, -59975}, {5810, 47980}},
                     {{1933, 12976100}, {2611, -2595210}, {3011, -10380840}, {3740, -50}},
                     {{1933, -74900}, {2641, 14975}, {3740, 25}, {5410, 35000}, {6210, 24900}},
                     {{1510, 391500}, {2611, -78300}, {3011, -313200}},
                     {{1510, 7449400}, {2611, -1489875}, {3011, -5959500}, {3740, -25}},
                     {{1510, 1250000}, {2611, -250000}, {3011, -1000000}},
                     {{1790, 1500000}, {3014, -1500000}}};
const std::string ref_dec2022 = R"(<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eSKDUpload PUBLIC "-//Skatteverket, Sweden//DTD Skatteverket eSKDUpload-DTD Version 6.0//SV" "https://www.skatteverket.se/download/18.3f4496fd14864cc5ac99cb1/1415022101213/eSKDUpload_6p0.dtd">
<eSKDUpload Version="6.0">
  <OrgNr>551122-1234</OrgNr>
  <Moms>
    <Period>202212</Period>
    <ForsMomsEjAnnan>176535</ForsMomsEjAnnan>
    <MomsUtgHog>44133</MomsUtgHog>
    <ForsOvrigt>15001</ForsOvrigt>
    <MomsIngAvdr>269</MomsIngAvdr>
    <MomsBetala>43864</MomsBetala>
  </Moms>
</eSKDUpload>
)";

TEST_CASE("Moms dec 2022") {
    BollDoc doc = createDoc(dec_2022, 2022, 12);
    Verifikat redovisning;
    FieldSaldo result = summarize_moms(doc, 12, konto_map, redovisning);
    CHECK(redovisning[2611] == 4413385);
    CHECK(redovisning[2641] == -26970);
    CHECK(redovisning[2650] == -4386400);
    CHECK(redovisning[3740] == -15);
    CHECK(result[49] == -4386400);
    CHECK(result[05] == -17653540);

    std::string xml = gen_moms_eskd(doc, 12, result, field_to_skv);
    CHECK(xml == ref_dec2022);
}
