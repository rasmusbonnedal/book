#pragma once

#include <memory>

#include "file-handler.h"
#include "imgui-app.h"

class VerifikatWindow;
class KontoWindow;
class EditKontoDialog;
class NewVerifikatDialog;
class SaveFileChangesDialog;

class BookApp {
   public:
    BookApp();
    ~BookApp();

    void run();

    EditKontoDialog& editKontoDialog();

   private:
    void event();

    void doOpCheckDirty(FileHandler::Operation op);

    std::shared_ptr<VerifikatWindow> _verifikat_window;
    std::shared_ptr<KontoWindow> _konto_window;
    std::shared_ptr<EditKontoDialog> _edit_konto_dialog;
    std::shared_ptr<NewVerifikatDialog> _new_verifikat_dialog;
    std::shared_ptr<SaveFileChangesDialog> _save_file_changes_dialog;

    ImGuiApp _app;
    FileHandler _file_handler;
};
