#pragma once

#include "bolldoc.h"
#include "file-handler.h"
#include "imgui_combo_autoselect.h"
#include "imgui-window.h"

class BookApp;

// A window which lists all transactions for a specified account
// At the top there is a combo box to select account
class SaldoWindow : public ImGuiWindowBase {
   public:
    SaldoWindow(FileHandler& file_handler, BookApp& book_app);
    void doit() final;
    void setKonto(int konto);

   private:
    void makeSelectData();

    BookApp& _app;
    FileHandler& _file_handler;
    std::vector<std::string> _konton;
    std::vector<int> _konton_id;
    std::unique_ptr<ImGui::ComboAutoSelectData> _konto_select;
    std::vector<BollDoc::Verifikat> _verifikat_cache;
    uint64_t _cache_rev;
};
