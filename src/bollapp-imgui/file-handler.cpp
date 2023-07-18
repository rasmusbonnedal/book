#include "file-handler.h"

#include <malloc.h>
#include <nfd.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "bolldoc.h"
#include "serialize.h"

FileHandler::FileHandler() : _op(OP_NOP), _quit(false) {
    newFile();
}

FileHandler::~FileHandler() {}

void FileHandler::newFile() {
    _doc = std::make_unique<BollDoc>(1, "Rasmus", "123456-7890", 2013, "kr", false);
    _filename.clear();
}

FileHandler::OpenError FileHandler::open(std::string& chosen_file) {
    nfdchar_t* out_path = NULL;
    nfdresult_t result = NFD_OpenDialog("bollbok", NULL, &out_path);

    if (result == NFD_OKAY) {
        chosen_file = out_path;
        free(out_path);
        return openFile(chosen_file, DO_CHECKSUM);
    } else if (result == NFD_CANCEL) {
    } else {
        std::cout << "Error: " << NFD_GetError() << std::endl;
    }
    return OE_CANCEL;
}

FileHandler::OpenError FileHandler::openFile(const std::string& filename, ChecksumPolicy checkpol) {
    auto p = std::filesystem::u8path(filename);
    std::ifstream ifs(p);
    if (ifs.good()) {
        try {
            _doc = std::make_unique<BollDoc>(Serialize::loadDocument(ifs, checkpol == IGNORE_CHECKSUM));
        } catch (std::exception&) {
            return OE_CHECKSUM_ERROR;
        }
        _filename = p;
        return OE_SUCCESS;
    } else {
        std::cout << "Error: Opening " << filename << std::endl;
        return OE_FILE_ERROR;
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

const std::filesystem::path& FileHandler::getPath() const {
    return _filename;
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

FileHandler::OpenError FileHandler::performOp(std::string& chosen_file) {
    if (_op == OP_NEW) {
        newFile();
    } else if (_op == OP_OPEN) {
        return open(chosen_file);
    } else if (_op == OP_QUIT) {
        _quit = true;
    }
    _op = OP_NOP;
    return OE_SUCCESS;
}
