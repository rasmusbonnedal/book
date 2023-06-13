#pragma once

#include "file-handler.h"
#include "imgui-window.h"

class BookApp;

class KontoWindow : public ImGuiWindowBase {
   public:
    KontoWindow(FileHandler& file_handler, BookApp& book_app);
    void doit() final;

   private:
    FileHandler& _file_handler;
    BookApp& _book_app;
    int _selected_row;
};
