#pragma once

#include "file-handler.h"
#include "imgui-window.h"

class OneVerifikatWindow : public ImGuiWindowBase {
   public:
    OneVerifikatWindow(FileHandler& file_handler);
    void doit() final;
    void setVerifikat(int v);

   private:
    void update(bool need_update);

    FileHandler& _file_handler;
    struct Row {
        std::string konto;
        std::string namn;
        std::string saldo;
    };
    std::vector<Row> _rows;
    std::string _text;
    std::string _datum;
    int64_t _revision;
    int _verifikat;
};
