#pragma once

#include "imgui-dialog.h"
#include "moms.h"

class FileHandler;

class MomsDialog : public ImGuiDialog {
   public:
    MomsDialog(FileHandler& file_handler);
    void doit() final;
    void launch();

   private:
    FileHandler& m_file_handler;
    int m_datum;
    // Mapping from SKV form field number to acct numbers
    KontoMap m_konto_map;
    // Mapping from SKV form field number to SKV XML tag name
    FieldToSkv m_field_to_skv;
};
