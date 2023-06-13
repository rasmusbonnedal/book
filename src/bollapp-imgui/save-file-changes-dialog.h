#pragma once

#include "file-handler.h"
#include "imgui-dialog.h"

class SaveFileChangesDialog : public ImGuiDialog {
   public:
    SaveFileChangesDialog(FileHandler& file_handler);
    void doit() final;

   private:
    FileHandler& _file_handler;
};
