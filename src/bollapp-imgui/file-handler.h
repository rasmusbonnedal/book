#pragma once

#include <filesystem>
#include <memory>
#include <string>

class BollDoc;

class FileHandler {
   public:
    enum Operation { OP_NEW, OP_OPEN, OP_QUIT, OP_NOP };

    FileHandler();

    ~FileHandler();

    void newFile();

    bool open();

    void openFile(const std::string& filename);

    bool save();

    bool saveas();

    BollDoc& getDoc();

    bool hasTitleChanged();

    const std::string& getTitle() const;

    std::string getFilename() const;

    const std::filesystem::path& getPath() const;

    void setOp(Operation o);

    void cancelOp();

    bool performOp();

    bool shouldQuit();

   private:
    std::filesystem::path _filename;
    std::unique_ptr<BollDoc> _doc;
    std::string _title;
    Operation _op;
    bool _quit;
};
