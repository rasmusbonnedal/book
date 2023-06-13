#pragma once

#include <imgui.h>

#include <string>

class Shortcut {
   public:
    Shortcut();
    Shortcut(ImGuiKey key, ImGuiKey modifier);

    bool isPressed() const;

    const std::string& text() const {
        return _text;
    }

   private:
    bool ctrl() const {
        return _modifier & ImGuiMod_Ctrl;
    }
    bool shift() const {
        return _modifier & ImGuiMod_Shift;
    }
    bool alt() const {
        return _modifier & ImGuiMod_Alt;
    }

    std::string to_string() const;

    ImGuiKey _key;
    ImGuiKey _modifier;
    std::string _text;
};
