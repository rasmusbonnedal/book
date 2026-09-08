#pragma once

#include <optional>
#include <string>
#include "date.h"
#include <vector>

#include "imgui-window.h"

class BookApp;
class FileHandler;

// A window shich shows one verifikat
class OneVerifikatWindow : public ImGuiWindowBase {
   public:
    OneVerifikatWindow(FileHandler& file_handler, BookApp& app);
    void doit() final;
    void setVerifikat(int v);

   private:
    void update(bool need_update);

    FileHandler& _file_handler;
    BookApp& _app;
    struct Row {
        int konto_id;
        std::string konto;
        std::string namn;
        std::string saldo;
        std::optional<Date> struken;
    };
    std::vector<Row> _rows;
    std::string _text;
    std::string _datum;
    int64_t _revision;
    int _verifikat;
};
