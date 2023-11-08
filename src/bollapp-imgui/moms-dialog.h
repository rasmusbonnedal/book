#pragma once

#include "imgui-dialog.h"

class FileHandler;

class MomsDialog : public ImGuiDialog {
   public:
    MomsDialog(FileHandler& file_handler);
    void doit() final;
    void launch();

   private:
    FileHandler& m_file_handler;
    int m_datum;
};
