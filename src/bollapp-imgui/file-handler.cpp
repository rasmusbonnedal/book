#include "file-handler.h"

#include "bolldoc.h"
#include "serialize.h"

#include <nfd.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <malloc.h>

FileHandler::FileHandler() {
    _doc = std::make_unique<BollDoc>(1, "Rasmus", "123456-7890", 2013, "kr", false);
}

FileHandler::~FileHandler() {}

void FileHandler::open() {
    nfdchar_t* out_path = NULL;
    nfdresult_t result = NFD_OpenDialog("bollbok", NULL, &out_path);

    if (result == NFD_OKAY) {
        std::string path(out_path);
        free(out_path);
        std::ifstream ifs(std::filesystem::u8path(path));
        if (ifs.good()) {
            _doc = std::make_unique<BollDoc>(Serialize::loadDocument(ifs));
            _filename = path;
            std::cout << path << std::endl;
        } else {
            std::cout << "Error: Opening " << path << std::endl;
        }
        free(out_path);
    } else if (result == NFD_CANCEL) {
    } else {
        std::cout << "Error: " << NFD_GetError() << std::endl;
    }
}

BollDoc& FileHandler::getDoc() {
    return *_doc;
}
