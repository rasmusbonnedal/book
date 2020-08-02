#include "serialize.h"

#include <fstream>
#include <iostream>

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
    BollDoc doc = Serialize::loadDocument(ifs, true);
    ifs.close();

    std::ofstream ofs(argv[1]);
    if (!ofs.good()) {
        std::cout << "Could not open " << argv[1] << " for writing" << std::endl;
        return 1;
    }
    Serialize::saveDocumentCustom(doc, ofs);
    std::cout << "Checksum fixed!" << std::endl;
    return 0;
}