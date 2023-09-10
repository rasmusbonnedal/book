#pragma once

#include <filesystem>
#include <memory>
#include <string>

class BollDoc;

class FileHandler {
   public:
    enum Operation { OP_NEW, OP_OPEN, OP_IMPORT, OP_QUIT, OP_NOP };

    enum OpenError { OE_SUCCESS = 0, OE_FILE_ERROR = 1, OE_CHECKSUM_ERROR = 2, OE_CANCEL = 3 };

    enum ChecksumPolicy { DO_CHECKSUM = 0, IGNORE_CHECKSUM = 1 };

    FileHandler();

    ~FileHandler();

    void newFile();

    OpenError open(std::string& chosen_file);

    OpenError openFile(const std::string& filename, ChecksumPolicy checkpol);

    bool save();

    bool saveas();

    BollDoc& getDoc();

    bool hasTitleChanged();

    const std::string& getTitle() const;

    std::string getFilename() const;

    const std::filesystem::path& getPath() const;

    void setOp(Operation o);

    void cancelOp();

    OpenError performOp(std::string& chosen_file);

    bool shouldQuit();

   private:
    FileHandler::OpenError import_sie();
       
    std::filesystem::path _filename;
    std::unique_ptr<BollDoc> _doc;
    std::string _title;
    Operation _op;
    bool _quit;
};
