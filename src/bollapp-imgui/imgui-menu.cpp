#include "imgui-menu.h"

#include <imgui.h>

ImGuiMenuItem::ImGuiMenuItem(Operation operation) : _operation(operation) {}

ImGuiMenuItem::ImGuiMenuItem(Options o) {
    if (o == SEPARATOR) {
        _is_separator = true;
    }
}

void ImGuiMenuItem::doit() {
    if (_is_separator) {
        ImGui::Separator();
    } else {
        if (ImGui::MenuItem(_operation.label().c_str(), _operation.shortcut().c_str())) {
            _operation.trigger();
        }
    }
}

void ImGuiMenuItem::checkShortcut() {
    if (!_is_separator && _operation.isPressed()) {
        _operation.trigger();
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

void ImGuiMenuHeader::checkShortcut() {
    for (auto& item : _items) {
        item.checkShortcut();
    }
}

void ImGuiMenu::addHeader(ImGuiMenuHeader&& header) {
    _headers.push_back(std::move(header));
}

void ImGuiMenu::clear() {
    _headers.clear();
}

void ImGuiMenu::doit() {
    if (_headers.empty()) {
        return;
    }
    if (ImGui::BeginMainMenuBar()) {
        for (auto& header : _headers) {
            header.doit();
        }
        _height = ImGui::GetWindowSize().y;
        ImGui::EndMainMenuBar();
    }
    for (auto& header : _headers) {
        header.checkShortcut();
    }
}
