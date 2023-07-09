#pragma once

#include "bolldoc.h"
#include "file-handler.h"
#include "imgui_combo_autoselect.h"
#include "imgui-window.h"

class SaldoWindow : public ImGuiWindowBase {
   public:
    SaldoWindow(FileHandler& file_handler);
    void doit() final;

   private:
    void makeSelectData();

    FileHandler& _file_handler;
    std::vector<std::string> _konton;
    std::vector<int> _konton_id;
    std::unique_ptr<ImGui::ComboAutoSelectData> _konto_select;
    std::vector<BollDoc::Verifikat> _verifikat_cache;
    uint64_t _cache_rev;
};
