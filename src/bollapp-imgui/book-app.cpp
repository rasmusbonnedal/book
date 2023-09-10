#include "book-app.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "edit-konto-dialog.h"
#include "imgui-menu.h"
#include "konto-window.h"
#include "saldo-window.h"
#include "new-verifikat-dialog.h"
#include "save-file-changes-dialog.h"
#include "verifikat-window.h"
#include "checksum-fail-dialog.h"

BookApp::BookApp() : _app("Bokföring") {
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Create Windows
    _verifikat_window = std::make_shared<VerifikatWindow>(_file_handler);
    _app.addWindow(_verifikat_window);
    _konto_window = std::make_shared<KontoWindow>(_file_handler, *this);
    _app.addWindow(_konto_window);
    _saldo_window = std::make_shared<SaldoWindow>(_file_handler);
    _app.addWindow(_saldo_window);
    _edit_konto_dialog = std::make_shared<EditKontoDialog>(_file_handler);
    _app.addDialog(_edit_konto_dialog);
    _new_verifikat_dialog = std::make_shared<NewVerifikatDialog>(_file_handler);
    _app.addDialog(_new_verifikat_dialog);
    _save_file_changes_dialog = std::make_shared<SaveFileChangesDialog>(_file_handler);
    _app.addDialog(_save_file_changes_dialog);
    _checksum_fail_dialog = std::make_shared<ChecksumFailDialog>(_file_handler);
    _app.addDialog(_checksum_fail_dialog);

    // Create events
    _app.addEvent(std::bind(&BookApp::event, this));

    _app.setStyle(false);

    // Create Menu
    ImGuiMenuHeader file_menu("File");
    file_menu.addItem(ImGuiMenuItem(
        Operation("New", std::bind(&BookApp::doOpCheckDirty, this, FileHandler::OP_NEW))));
    file_menu.addItem(ImGuiMenuItem(
        Operation("Open", Shortcut(ImGuiKey_O, ImGuiMod_Ctrl), std::bind(&BookApp::doOpCheckDirty, this, FileHandler::OP_OPEN))));
    file_menu.addItem(ImGuiMenuItem(Operation("Import SIE", {}, std::bind(&BookApp::doOpCheckDirty, this, FileHandler::OP_IMPORT))));
    file_menu.addItem(ImGuiMenuItem(Operation("Save", Shortcut(ImGuiKey_S, ImGuiMod_Ctrl), std::bind(&FileHandler::save, &_file_handler))));
    file_menu.addItem(ImGuiMenuItem(Operation("Save As...", {}, std::bind(&FileHandler::saveas, &_file_handler))));
    file_menu.addItem(ImGuiMenuItem(
        Operation("Quit", Shortcut(ImGuiKey_F4, ImGuiMod_Alt), std::bind(&BookApp::doOpCheckDirty, this, FileHandler::OP_QUIT))));
    _app.getMenu().addHeader(std::move(file_menu));
    ImGuiMenuHeader edit_menu("Edit");
    edit_menu.addItem(ImGuiMenuItem(
        Operation("New Verifikat", Shortcut(ImGuiKey_N, ImGuiMod_Ctrl), std::bind(&NewVerifikatDialog::launchVer, _new_verifikat_dialog))));
    _app.getMenu().addHeader(std::move(edit_menu));
}

BookApp::~BookApp() {}

void BookApp::run() {
    std::string filename;
    if (_app.getSettings().get("current_file", filename)) {
        _file_handler.openFile(filename, FileHandler::DO_CHECKSUM);
    }
    _app.run();
}

EditKontoDialog& BookApp::editKontoDialog() {
    return *_edit_konto_dialog;
}

void BookApp::event() {
    if (_file_handler.hasTitleChanged()) {
        _app.setTitle("ImGui Bollbok: " + _file_handler.getTitle());
    }
    if (_app.wantsToQuit()) {
        doOpCheckDirty(FileHandler::OP_QUIT);
    }
    if (_file_handler.shouldQuit()) {
        _app.quit();
    }
}

void BookApp::doOpCheckDirty(FileHandler::Operation op) {
    _file_handler.setOp(op);
    if (_file_handler.getDoc().isDirty()) {
        _save_file_changes_dialog->launch();
    } else {
        std::string chosen_file;
        if (_file_handler.performOp(chosen_file) == FileHandler::OE_CHECKSUM_ERROR) {
            _checksum_fail_dialog->launch(chosen_file);
        }
        auto file = _file_handler.getPath();
        if (!file.empty()) {
            _app.getSettings().set("current_file", file.u8string());
        }
    }
}
