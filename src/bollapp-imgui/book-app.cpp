#include "book-app.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "edit-konto-dialog.h"
#include "imgui-menu.h"
#include "konto-window.h"
#include "moms-dialog.h"
#include "report-window.h"
#include "saldo-window.h"
#include "one-verifikat-window.h"
#include "new-verifikat-dialog.h"
#include "save-file-changes-dialog.h"
#include "verifikat-window.h"
#include "checksum-fail-dialog.h"

BookApp::BookApp() : _app("Bokföring") {
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Create Windows
    _verifikat_window = std::make_shared<VerifikatWindow>(_file_handler, *this);
    _app.addWindow(_verifikat_window);
    _konto_window = std::make_shared<KontoWindow>(_file_handler, *this);
    _app.addWindow(_konto_window);
    _report_window = std::make_shared<ReportWindow>(_file_handler);
    _app.addWindow(_report_window);
    _saldo_window = std::make_shared<SaldoWindow>(_file_handler);
    _app.addWindow(_saldo_window);
    _one_verifikat_window = std::make_shared<OneVerifikatWindow>(_file_handler);
    _app.addWindow(_one_verifikat_window);
    _edit_konto_dialog = std::make_shared<EditKontoDialog>(_file_handler);
    _app.addDialog(_edit_konto_dialog);
    _new_verifikat_dialog = std::make_shared<NewVerifikatDialog>(_file_handler);
    _app.addDialog(_new_verifikat_dialog);
    _moms_dialog = std::make_shared<MomsDialog>(_file_handler);
    _app.addDialog(_moms_dialog);
    _save_file_changes_dialog = std::make_shared<SaveFileChangesDialog>(_file_handler);
    _app.addDialog(_save_file_changes_dialog);
    _checksum_fail_dialog = std::make_shared<ChecksumFailDialog>(_file_handler);
    _app.addDialog(_checksum_fail_dialog);

    // Create events
    _app.addEvent(std::bind(&BookApp::event, this));

    _app.setStyle(false);
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg].w = 0.97f;
    style.Colors[ImGuiCol_PopupBg].w = 0.98f;
    style.Colors[ImGuiCol_TitleBg].w = 0.95f;
    style.Colors[ImGuiCol_TitleBgActive].w = 0.97f;
    style.Colors[ImGuiCol_TitleBgCollapsed].w = 0.90f;
    style.Colors[ImGuiCol_TitleBgCollapsed].w = 0.90f;

    ImGui::GetCurrentContext()->PlatformLocaleDecimalPoint = ',';
    initMenu();
}

BookApp::~BookApp() {}

void BookApp::run() {
    std::string filename;
    if (_app.getSettings().get("recent0", filename)) {
        _file_handler.openFile(filename, FileHandler::DO_CHECKSUM);
    }
    _app.run();
}

BollDoc& BookApp::doc() {
    return _file_handler.getDoc();
}

EditKontoDialog& BookApp::editKontoDialog() {
    return *_edit_konto_dialog;
}

NewVerifikatDialog& BookApp::newVerifikatDialog() {
    return *_new_verifikat_dialog;
}

OneVerifikatWindow& BookApp::oneVerifikatWindow() {
    return *_one_verifikat_window;
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

void BookApp::initMenu() {
    _app.getMenu().clear();
    // Create Menu
    ImGuiMenuHeader file_menu("File");
    file_menu.addItem(ImGuiMenuItem(Operation("New", std::bind(&BookApp::doOpCheckDirty, this, FileHandler::OP_NEW, ""))));
    file_menu.addItem(ImGuiMenuItem(
        Operation("Open", Shortcut(ImGuiKey_O, ImGuiMod_Ctrl), std::bind(&BookApp::doOpCheckDirty, this, FileHandler::OP_OPEN, ""))));
    file_menu.addItem(ImGuiMenuItem(Operation("Import SIE", {}, std::bind(&BookApp::doOpCheckDirty, this, FileHandler::OP_IMPORT, ""))));
    file_menu.addItem(ImGuiMenuItem(Operation("Save", Shortcut(ImGuiKey_S, ImGuiMod_Ctrl), std::bind(&FileHandler::save, &_file_handler))));
    file_menu.addItem(ImGuiMenuItem(Operation("Save As...", {}, std::bind(&FileHandler::saveas, &_file_handler))));
    file_menu.addItem(ImGuiMenuItem(Operation("Export SIE", {}, std::bind(&FileHandler::export_sie, &_file_handler))));
    file_menu.addItem(ImGuiMenuItem(ImGuiMenuItem::SEPARATOR));
    for (int i = 0; i < 4; ++i) {
        std::string value;
        if (_app.getSettings().get("recent" + std::to_string(i), value)) {
            auto path = std::filesystem::u8path(value);
            file_menu.addItem(
                ImGuiMenuItem(Operation(std::to_string(i) + ": " + path.filename().u8string(),
                                        std::bind(&BookApp::doOpCheckDirty, this, FileHandler::OP_OPEN_FILE, path.u8string()))));
        }
    }
    file_menu.addItem(ImGuiMenuItem(ImGuiMenuItem::SEPARATOR));
    file_menu.addItem(ImGuiMenuItem(
        Operation("Quit", Shortcut(ImGuiKey_F4, ImGuiMod_Alt), std::bind(&BookApp::doOpCheckDirty, this, FileHandler::OP_QUIT, ""))));
    _app.getMenu().addHeader(std::move(file_menu));

    ImGuiMenuHeader edit_menu("Edit");
    edit_menu.addItem(ImGuiMenuItem(
        Operation("New Verifikat", Shortcut(ImGuiKey_N, ImGuiMod_Ctrl), std::bind(&NewVerifikatDialog::launchVer, _new_verifikat_dialog))));
    edit_menu.addItem(
        ImGuiMenuItem(Operation("Momsredovisning", Shortcut(ImGuiKey_M, ImGuiMod_Ctrl), std::bind(&MomsDialog::launch, _moms_dialog))));
    _app.getMenu().addHeader(std::move(edit_menu));
}


void BookApp::doOpCheckDirty(FileHandler::Operation op, const std::string& arg) {
    _file_handler.setOp(op, arg);
    if (_file_handler.getDoc().isDirty()) {
        _save_file_changes_dialog->launch();
    } else {
        std::string chosen_file;
        if (_file_handler.performOp(chosen_file) == FileHandler::OE_CHECKSUM_ERROR) {
            _checksum_fail_dialog->launch(chosen_file);
        }
        auto file = _file_handler.getPath();
        if (!file.empty()) {
            std::string new_file = file.u8string();
            std::vector<std::string> recent_files;
            recent_files.push_back(new_file);
            for (int i = 4; i >= 0; --i) {
                std::string value;
                if (_app.getSettings().get("recent" + std::to_string(i), value) && value != new_file) {
                    recent_files.push_back(value);
                }
            }
            for (int i = std::min(4, (int)recent_files.size() - 1); i >= 0; --i) {
                _app.getSettings().set("recent" + std::to_string(i), recent_files[i]);
            }
            initMenu();
        }
    }
}
