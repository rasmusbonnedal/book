#include "file-handler.h"

#include <malloc.h>
#include <nfd.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "bolldoc.h"
#include "serialize.h"

FileHandler::FileHandler() : _op(OP_NOP), _quit(false) {
#if 0
    newFile();
#else
    openFile(std::filesystem::path(R"(C:\git\bokforing\test.bollbok)").u8string());
#endif
}

FileHandler::~FileHandler() {}

void FileHandler::newFile() {
    _doc = std::make_unique<BollDoc>(1, "Rasmus", "123456-7890", 2013, "kr", false);
    _filename.clear();
}

bool FileHandler::open() {
    nfdchar_t* out_path = NULL;
    nfdresult_t result = NFD_OpenDialog("bollbok", NULL, &out_path);

    if (result == NFD_OKAY) {
        std::string path(out_path);
        free(out_path);
        openFile(path);
        return true;
    } else if (result == NFD_CANCEL) {
    } else {
        std::cout << "Error: " << NFD_GetError() << std::endl;
    }
    return false;
}

void FileHandler::openFile(const std::string& filename) {
    auto p = std::filesystem::u8path(filename);
    std::ifstream ifs(p);
    if (ifs.good()) {
        _doc = std::make_unique<BollDoc>(Serialize::loadDocument(ifs));
        _filename = p;
    } else {
        std::cout << "Error: Opening " << filename << std::endl;
    }
}

bool FileHandler::save() {
    if (_filename.empty()) {
        return saveas();
    }
    std::ofstream ofs(_filename);
    Serialize::saveDocumentCustom(*_doc, ofs);
    _doc->clearDirty();
    return true;
}

bool FileHandler::saveas() {
    nfdchar_t* out_path = NULL;
    nfdresult_t result = NFD_SaveDialog("bollbok", NULL, &out_path);

    if (result == NFD_OKAY) {
        _filename = std::filesystem::u8path(out_path);
        free(out_path);
        return save();
    } else if (result == NFD_CANCEL) {
    } else {
        std::cout << "Error: " << NFD_GetError() << std::endl;
    }
    return false;
}

BollDoc& FileHandler::getDoc() {
    return *_doc;
}

bool FileHandler::hasTitleChanged() {
    std::string title = getFilename();
    if (_doc->isDirty()) {
        title += " *";
    }
    if (title != _title) {
        _title = std::move(title);
        return true;
    }
    return false;
}

const std::string& FileHandler::getTitle() const {
    return _title;
}

std::string FileHandler::getFilename() const {
    if (_filename.empty()) {
        return "Untitled";
    } else {
        return _filename.filename().u8string();
    }
}

void FileHandler::setOp(Operation o) {
    _op = o;
}

void FileHandler::cancelOp() {
    _op = OP_NOP;
}

bool FileHandler::shouldQuit() {
    return _quit;
}

bool FileHandler::performOp() {
    if (_op == OP_NEW) {
        newFile();
    } else if (_op == OP_OPEN) {
        return open();
    } else if (_op == OP_QUIT) {
        _quit = true;
    }
    _op = OP_NOP;
    return true;
}
