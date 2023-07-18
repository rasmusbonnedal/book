#include "checksum-fail-dialog.h"

#include <imgui.h>

ChecksumFailDialog::ChecksumFailDialog(FileHandler& file_handler) : ImGuiDialog(u8"Checksum fail"), m_file_handler(file_handler) {}

void ChecksumFailDialog::doit() {
    ImGui::Text("Checksum failed on %s. Open anyway?", m_filename.c_str());
    if (ImGui::Button("Open")) {
        m_file_handler.openFile(m_filename, FileHandler::IGNORE_CHECKSUM);   
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        ImGui::CloseCurrentPopup();
    }
}
