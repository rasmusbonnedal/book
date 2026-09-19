#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <sstream>
#include <fstream>
#include "serialize.h"

#include "bolldoc.h"
#include "file-handler.h"
#include "sieparse.h"

TEST_CASE("Bokforingsorder is not exported to SIE") {
    BollDoc doc(2074, "Ruffel & Båg", "551122-1234", 2018, "SEK", false);
    doc.addVerifikat({0, "Ingående saldon", Date(0, 1, 1)});

    BollDoc::Verifikat verifikat{1, "Bokförd hyra", Date(2018, 2, 1)};
    verifikat.addRad({Date(2018, 2, 1), 1910, parsePengar("-8000")});
    verifikat.addRad({Date(2018, 2, 1), 5010, parsePengar("8000")});
    doc.addVerifikat(std::move(verifikat));

    BollDoc::Verifikat bokforingsorder{
        2, "Preliminar hyra", Date(2018, 3, 1), true};
    bokforingsorder.addRad(
        {Date(2018, 3, 1), 1910, parsePengar("-9000")});
    bokforingsorder.addRad(
        {Date(2018, 3, 1), 5010, parsePengar("9000")});
    doc.addVerifikat(std::move(bokforingsorder));

    doc.addOrUpdateKonto({1910, "Bankkonto", 1});
    doc.addOrUpdateKonto({5010, "Hyra", 3});

    std::stringstream exported;
    REQUIRE(export_sie(doc, exported));

    SIEData siedata;
    REQUIRE(parse(siedata, exported));
    REQUIRE(siedata.verifikat.size() == 1);
    CHECK(siedata.verifikat[0].id == 1);
    CHECK(siedata.verifikat[0].text == "Bokförd hyra");
}

namespace {
struct ReceiptFixture {
    std::filesystem::path dir;
    ReceiptFixture() {
        for (int i = 0; ; ++i) {
            auto candidate = std::filesystem::current_path() / ("receipt-test-" + std::to_string(i));
            if (std::filesystem::create_directory(candidate)) { dir = candidate; break; }
        }
        std::filesystem::create_directory(dir / "book.kvitton");
    }
    ~ReceiptFixture() {
        std::error_code ec;
        std::filesystem::remove_all(dir, ec);
    }
    std::filesystem::path book() const { return dir / "book.bollbok"; }
    std::filesystem::path receipt(const std::string& name) const { return dir / "book.kvitton" / name; }
    void writeBook() {
        BollDoc doc(2074, "Test", "551122-1234", 2018, "SEK", false);
        doc.addOrUpdateKonto({1910, "Bank", 1});
        doc.addOrUpdateKonto({5010, "Rent", 3});
        doc.addVerifikat({0, "Opening", Date(0, 1, 1)});
        // A posted order and a converted verifikat have exchanged numbers.
        BollDoc::Verifikat posted{1, "Posted", Date(2018, 1, 1), false, 1000000};
        posted.addRad({Date(2018, 1, 1), 1910, -100});
        posted.addRad({Date(2018, 1, 1), 5010, 100});
        doc.addVerifikat(std::move(posted));
        BollDoc::Verifikat draft{1000000, "Draft", Date(2018, 1, 1), true, 1};
        draft.addRad({Date(2018, 1, 1), 1910, -200});
        draft.addRad({Date(2018, 1, 1), 5010, 200});
        doc.addVerifikat(std::move(draft));
        std::ofstream out(book());
        Serialize::saveDocumentCustom(doc, out);
    }
};
std::string readFile(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}
}

TEST_CASE("Saving renames receipts to SIE numbers without mixing reused numbers") {
    ReceiptFixture files;
    files.writeBook();
    std::ofstream(files.receipt("V1000000.pdf")) << "posted receipt";
    std::ofstream(files.receipt("V1000000_1.png")) << "second posted receipt";
    std::ofstream(files.receipt("V1.pdf")) << "draft receipt";
    FileHandler handler;
    REQUIRE(handler.openFile(files.book().u8string(), FileHandler::DO_CHECKSUM) == FileHandler::OE_SUCCESS);
    REQUIRE(handler.save());
    CHECK(readFile(files.receipt("V1.pdf")) == "posted receipt");
    CHECK(readFile(files.receipt("V1_1.png")) == "second posted receipt");
    CHECK(readFile(files.receipt("V1000000.pdf")) == "draft receipt");
    CHECK_FALSE(std::filesystem::exists(files.receipt("V1000000_1.png")));
    REQUIRE(handler.openFile(files.book().u8string(), FileHandler::DO_CHECKSUM) == FileHandler::OE_SUCCESS);
    CHECK(handler.getDoc().getVerifikat(1).getKvittoId() == 1);
    CHECK(handler.getKvitton(1).size() == 2);
    std::stringstream exported;
    REQUIRE(export_sie(handler.getDoc(), exported));
    SIEData sie;
    REQUIRE(parse(sie, exported));
    REQUIRE(sie.verifikat.size() == 1);
    CHECK(sie.verifikat.front().id == 1);
    REQUIRE(handler.save());
    CHECK(readFile(files.receipt("V1.pdf")) == "posted receipt");
    auto draft = handler.getDoc().getVerifikat(1000000);
    draft.promoteToVerifikat();
    REQUIRE(handler.getDoc().updateVerifikat(std::move(draft)) == 2);
    REQUIRE(handler.save());
    CHECK(readFile(files.receipt("V2.pdf")) == "draft receipt");
    CHECK_FALSE(std::filesystem::exists(files.receipt("V1000000.pdf")));
}

TEST_CASE("Conflicting receipt filenames block save without overwriting files") {
    ReceiptFixture files;
    files.writeBook();
    std::ofstream(files.receipt("V1000000_1.png")) << "source";
    std::ofstream(files.receipt("V1_1.png")) << "unrelated";
    // V1 belongs to the draft, so its existing file is a valid permutation.
    // Use a book with only the posted entry to make the destination unowned.
    BollDoc doc(2074, "Test", "551122-1234", 2018, "SEK", false);
    doc.addVerifikat({0, "Opening", Date(0, 1, 1)});
    BollDoc::Verifikat v{1, "Posted", Date(2018, 1, 1), false, 1000000};
    v.addRad({Date(2018, 1, 1), 1910, 100});
    doc.addVerifikat(std::move(v));
    { std::ofstream out(files.book()); Serialize::saveDocumentCustom(doc, out); }
    const auto before = readFile(files.book());
    FileHandler handler;
    REQUIRE(handler.openFile(files.book().u8string(), FileHandler::DO_CHECKSUM) == FileHandler::OE_SUCCESS);
    CHECK_FALSE(handler.save());
    CHECK(readFile(files.receipt("V1000000_1.png")) == "source");
    CHECK(readFile(files.receipt("V1_1.png")) == "unrelated");
    CHECK(readFile(files.book()) == before);
    CHECK(handler.getDoc().getVerifikat(1).getKvittoId() == 1000000);
}
