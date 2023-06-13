#pragma once

#include "bolldoc.h"
#include "file-handler.h"
#include "imgui-dialog.h"

class EditKontoDialog : public ImGuiDialog {
   public:
    EditKontoDialog(FileHandler& file_handler);
    void doit() final;

    void launch(int kontonr) {
        m_konto = m_file_handler.getDoc().getKonto(kontonr);
        ImGuiDialog::launch();
    }

   private:
    FileHandler& m_file_handler;
    BollDoc::Konto m_konto;
};
