#pragma once

#include <memory>
#include <string>

class BollDoc;

class FileHandler {
public:
    FileHandler();
    ~FileHandler();
    void open();
    void save();
    void saveas();
    BollDoc& getDoc();
private:
    std::string _filename;
    std::unique_ptr<BollDoc> _doc;
};
