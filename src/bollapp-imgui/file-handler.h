#pragma once

#include <filesystem>
#include <memory>
#include <string>

class BollDoc;

typedef enum {
    FDR_ERROR, /* programmatic error */
    FDR_OKAY,  /* user pressed okay, or successful return */
    FDR_CANCEL /* user pressed cancel */
} FileDialogResult;

FileDialogResult fileOpenDialog(const std::string& filter, std::string& filename);

FileDialogResult fileSaveDialog(const std::string& filter, std::string& filename);

class FileHandler {
   public:
    enum Operation { OP_NEW, OP_NEWYEAR, OP_OPEN, OP_OPEN_FILE, OP_IMPORT, OP_QUIT, OP_NOP };

    enum OpenError { OE_SUCCESS = 0, OE_FILE_ERROR = 1, OE_CHECKSUM_ERROR = 2, OE_CANCEL = 3 };

    enum ChecksumPolicy { DO_CHECKSUM = 0, IGNORE_CHECKSUM = 1 };

    FileHandler();

    ~FileHandler();

    void newFile();

    void newYear();

    OpenError open(std::string& chosen_file);

    OpenError openFile(const std::string& filename, ChecksumPolicy checkpol);

    bool save();

    bool saveas();

    bool export_sie();

    BollDoc& getDoc();

    std::vector<std::filesystem::path> getKvitton(int unid) const;

    bool canAttachKvitto() const;

    bool attachKvitto(int unid, const std::filesystem::path& kvitto) const;

    bool hasTitleChanged();

    const std::string& getTitle() const;

    std::string getFilename() const;

    const std::filesystem::path& getPath() const;

    void setOp(Operation o, const std::string& arg);

    void cancelOp();

    OpenError performOp(std::string& chosen_file);

    bool shouldQuit();

   private:
    FileHandler::OpenError import_sie();
       
    std::filesystem::path _filename;
    std::unique_ptr<BollDoc> _doc;
    std::string _title;
    Operation _op;
    std::string _op_arg;
    bool _quit;
};
