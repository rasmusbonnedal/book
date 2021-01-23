#include "serialize.h"

#include <rapidxml.hpp>

#include <fstream>
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: bless bolldoc-file" << std::endl
                  << "Fix checksum for bolldoc file" << std::endl;
        return 1;
    }

    std::ifstream ifs(argv[1]);
    if (!ifs.good()) {
        std::cout << "Could not open " << argv[1] << " for reading" << std::endl;
        return 1;
    }
    std::unique_ptr<BollDoc> doc;
    try {
        doc = std::make_unique<BollDoc>(Serialize::loadDocument(ifs, true));
    } catch (rapidxml::parse_error& e) {
        std::cout << "XML Parse error: " << e.what() << std::endl;

        std::string where(e.where<char>(), 1);
        std::cout << "Found at: " << where << std::endl;
        return 1;
    }
    ifs.close();

    std::ofstream ofs(argv[1]);
    if (!ofs.good()) {
        std::cout << "Could not open " << argv[1] << " for writing" << std::endl;
        return 1;
    }
    Serialize::saveDocumentCustom(*doc, ofs);
    std::cout << "Checksum fixed!" << std::endl;
    return 0;
}