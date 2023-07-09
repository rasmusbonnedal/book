#include "imgui-settings.h"

#include <imgui.h>
#include <imgui_internal.h>

void ImGuiSettings::init() {
    ImGuiSettingsHandler handler;
    handler.TypeName = "rbcustom";
    handler.TypeHash = ImHashStr(handler.TypeName);
    handler.ClearAllFn = &ImGuiSettings::ClearAllFn;
    handler.ReadOpenFn = &ImGuiSettings::ReadOpenFn;
    handler.ReadLineFn = &ImGuiSettings::ReadLineFn;
    handler.ApplyAllFn = &ImGuiSettings::ApplyAllFn;
    handler.WriteAllFn = &ImGuiSettings::WriteAllFn;
    handler.UserData = this;
    GImGui->SettingsHandlers.push_back(handler);
}

void ImGuiSettings::set(const std::string& key, const std::string& value) {
    m_config[key] = value;
}

bool ImGuiSettings::get(const std::string& key, std::string& value) const {
    auto it = m_config.find(key);
    if (it != m_config.end()) {
        value = it->second;
        return true;
    }
    return false;
}

void ImGuiSettings::ClearAllFn(ImGuiContext* ctx, ImGuiSettingsHandler* handler) {
    ImGuiSettings* this_ = (ImGuiSettings*)handler->UserData;
    this_->m_config.clear();
}

void* ImGuiSettings::ReadOpenFn(ImGuiContext* ctx, ImGuiSettingsHandler* handler, const char* name) {
    ImGuiSettings* this_ = (ImGuiSettings*)handler->UserData;
    return this_;
}

void ImGuiSettings::ReadLineFn(ImGuiContext* ctx, ImGuiSettingsHandler* handler, void* entry, const char* line) {
    ImGuiSettings* this_ = (ImGuiSettings*)handler->UserData;
    std::string s(line);
    auto pos = s.find('=');
    if (pos != std::string::npos) {
        this_->m_config[s.substr(0, pos)] = s.substr(pos + 1);
    }
}

void ImGuiSettings::ApplyAllFn(ImGuiContext* ctx, ImGuiSettingsHandler* handler) {
    ImGuiSettings* this_ = (ImGuiSettings*)handler->UserData;
}

void ImGuiSettings::WriteAllFn(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf) {
    ImGuiSettings* this_ = (ImGuiSettings*)handler->UserData;
    buf->appendf("[%s][%s]\n", handler->TypeName, "default");
    for (const auto& [k, v] : this_->m_config) {
        buf->appendf("%s=%s\n", k.c_str(), v.c_str());
    }
    buf->appendf("\n");
}
