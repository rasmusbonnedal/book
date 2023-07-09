#pragma once

#include <map>
#include <string>

struct ImGuiContext;
struct ImGuiSettingsHandler;
struct ImGuiTextBuffer;

class ImGuiSettings {
   public:
    void init();
    void set(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value) const;

   private:
    static void ClearAllFn(ImGuiContext* ctx, ImGuiSettingsHandler* handler);
    static void* ReadOpenFn(ImGuiContext* ctx, ImGuiSettingsHandler* handler, const char* name);
    static void ReadLineFn(ImGuiContext* ctx, ImGuiSettingsHandler* handler, void* entry, const char* line);
    static void ApplyAllFn(ImGuiContext* ctx, ImGuiSettingsHandler* handler);
    static void WriteAllFn(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf);
    std::map<std::string, std::string> m_config;
};
