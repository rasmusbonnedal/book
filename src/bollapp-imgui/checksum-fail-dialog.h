#pragma once

#include "file-handler.h"
#include "imgui-dialog.h"

class ChecksumFailDialog : public ImGuiDialog {
   public:
    ChecksumFailDialog(FileHandler& file_handler);
    void doit() final;

    void launch(const std::string& filename) {
        m_filename = filename;
        ImGuiDialog::launch();
    }

   private:
    FileHandler& m_file_handler;
    std::string m_filename;
};
