#pragma once
#include "imgui-window.h"

#include <memory>

class FileHandler;
class ReportCache;

class ReportWindow : public ImGuiWindowBase {
   public:
    ReportWindow(FileHandler& file_handler);
    ~ReportWindow();

    void doit() final;

   private:
    std::unique_ptr<ReportCache> m_cache;
    FileHandler& m_file_handler;
    int m_datum;
    int m_report_type;
};
