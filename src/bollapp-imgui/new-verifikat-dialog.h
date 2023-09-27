#pragma once

#include <filesystem>

#include "bolldoc.h"
#include "imgui-dialog.h"
#include "imgui_combo_autoselect.h"

class FileHandler;

class NewVerifikatDialog : public ImGuiDialog {
   public:
    NewVerifikatDialog(FileHandler& file_handler);
    void doit() final;
    void launchVer();
    void launchEdit(const BollDoc::Verifikat& verifikat);

   private:
    FileHandler& m_file_handler;
    std::unique_ptr<BollDoc::Verifikat> m_verifikat;
    std::vector<std::string> m_konton;
    std::vector<int> m_konton_id;
    std::vector<ImGui::ComboAutoSelectData> m_konto_rad_data;
    std::vector<double> m_pengar_rad;
    std::vector<std::filesystem::path> m_kvitton;
    std::vector<std::filesystem::path> m_attached_kvitton;
    bool m_can_attach_kvitto;
    std::string m_date;
    bool m_date_ok;

    enum DialogMode {
        NEW,
        EDIT
    };
    DialogMode m_dialog_mode;
};
