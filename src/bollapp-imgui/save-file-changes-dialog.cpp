#include "save-file-changes-dialog.h"

#include <imgui.h>

#include "bolldoc.h"

SaveFileChangesDialog::SaveFileChangesDialog(FileHandler& file_handler) : ImGuiDialog("Save Changes?"), _file_handler(file_handler) {}

void SaveFileChangesDialog::doit() {
    ImGui::TextUnformatted((_file_handler.getFilename() + " has been modified, save changes?").c_str());
    ImGui::Separator();
    if (ImGui::Button("Yes")) {
        // Don't proceed with op if user cancels save
        if (_file_handler.save()) {
            _file_handler.performOp();
        }
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("No")) {
        // Don't clear dirty if user cancels operation
        if (_file_handler.performOp()) {
            _file_handler.getDoc().clearDirty();
        }
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        _file_handler.cancelOp();
        ImGui::CloseCurrentPopup();
    }
}
