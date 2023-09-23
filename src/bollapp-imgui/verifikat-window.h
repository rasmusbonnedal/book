#pragma once

#include "file-handler.h"
#include "imgui-window.h"

class BookApp;

class VerifikatWindow : public ImGuiWindowBase {
   public:
    VerifikatWindow(FileHandler& file_handler, BookApp& book_app);
    void doit() final;

   private:
    FileHandler& _file_handler;
    int _selected_row;
    std::vector<int> _verifikat_index;
    int64_t _sorted_revision;
    BookApp& m_app;
};
