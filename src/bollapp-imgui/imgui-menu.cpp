#include "imgui-menu.h"

#include <imgui.h>

ImGuiMenuItem::ImGuiMenuItem(const std::string& label,
                             const std::string& shortcut, std::function<void()> event)
    : _label(label), _shortcut(shortcut), _event(event) {}

ImGuiMenuItem::ImGuiMenuItem(Options o) {
    if (o == SEPARATOR) {
        _is_separator = true;
    }
}

void ImGuiMenuItem::doit() {
    if (_is_separator) {
        ImGui::Separator();
    } else {
        if (ImGui::MenuItem(_label.c_str(), _shortcut.c_str())) {
            _event();
        }
    }
}

ImGuiMenuHeader::ImGuiMenuHeader(const std::string& name) : _name(name) {}

void ImGuiMenuHeader::addItem(ImGuiMenuItem&& item) {
    _items.push_back(std::move(item));
}

void ImGuiMenuHeader::addSeparator() {
    _items.push_back(ImGuiMenuItem(ImGuiMenuItem::SEPARATOR));
}

void ImGuiMenuHeader::doit() {
    if (ImGui::BeginMenu(_name.c_str())) {
        for (auto& item : _items) {
            item.doit();
        }
        ImGui::EndMenu();
    }
}

void ImGuiMenu::addHeader(ImGuiMenuHeader&& header) {
    _headers.push_back(std::move(header));
}

void ImGuiMenu::doit() {
    if (ImGui::BeginMainMenuBar()) {
        for (auto& header: _headers) {
            header.doit();
        }
        ImGui::EndMainMenuBar();
    }
}
