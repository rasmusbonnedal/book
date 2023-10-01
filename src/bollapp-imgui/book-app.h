#pragma once

#include <memory>

#include "file-handler.h"
#include "imgui-app.h"

class VerifikatWindow;
class KontoWindow;
class ReportWindow;
class SaldoWindow;
class OneVerifikatWindow;
class EditKontoDialog;
class NewVerifikatDialog;
class SaveFileChangesDialog;
class ChecksumFailDialog;

class BookApp {
   public:
    BookApp();
    ~BookApp();

    void run();

    EditKontoDialog& editKontoDialog();
    NewVerifikatDialog& newVerifikatDialog();
    OneVerifikatWindow& oneVerifikatWindow();

   private:
    void event();

    void doOpCheckDirty(FileHandler::Operation op);

    std::shared_ptr<VerifikatWindow> _verifikat_window;
    std::shared_ptr<KontoWindow> _konto_window;
    std::shared_ptr<ReportWindow> _report_window;
    std::shared_ptr<SaldoWindow> _saldo_window;
    std::shared_ptr<OneVerifikatWindow> _one_verifikat_window;
    std::shared_ptr<EditKontoDialog> _edit_konto_dialog;
    std::shared_ptr<NewVerifikatDialog> _new_verifikat_dialog;
    std::shared_ptr<SaveFileChangesDialog> _save_file_changes_dialog;
    std::shared_ptr<ChecksumFailDialog> _checksum_fail_dialog;

    ImGuiApp _app;
    FileHandler _file_handler;
};
