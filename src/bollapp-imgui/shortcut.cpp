#include "shortcut.h"

#include <assert.h>

Shortcut::Shortcut() : _key(ImGuiKey_None), _modifier(ImGuiKey_None) {}

Shortcut::Shortcut(ImGuiKey key, ImGuiKey modifier) : _key(key), _modifier(modifier) {
    assert((key >= ImGuiKey_A && key <= ImGuiKey_Z) || (key >= ImGuiKey_F1 && key <= ImGuiKey_F12) ||
           (key >= ImGuiKey_0 && key <= ImGuiKey_9));
    _text = to_string();
}

bool Shortcut::isPressed() const {
    ImGuiIO& io = ImGui::GetIO();
    if (!ImGui::IsKeyPressed(_key, false)) {
        return false;
    }
    bool ctrl_ok = !ctrl() ^ io.KeyCtrl;
    bool shift_ok = !shift() ^ io.KeyShift;
    bool alt_ok = !alt() ^ io.KeyAlt;
    return ctrl_ok && shift_ok && alt_ok;
}

std::string Shortcut::to_string() const {
    std::string retval;
    if (_key == ImGuiKey_None) {
        return retval;
    }

    if (ctrl()) {
        retval += "CTRL-";
    }
    if (shift()) {
        retval += "SHIFT-";
    }
    if (alt()) {
        retval += "ALT-";
    }
    if (_key >= ImGuiKey_A && _key <= ImGuiKey_Z) {
        retval += ('A' + _key - ImGuiKey_A);
    } else if (_key >= ImGuiKey_F1 && _key <= ImGuiKey_F12) {
        retval += 'F';
        retval += std::to_string(_key + 1 - ImGuiKey_F1);
    } else if (_key >= ImGuiKey_0 && _key <= ImGuiKey_9) {
        retval += ('0' + _key - ImGuiKey_0);
    } else {
        retval += "UnknownKey";
    }
    return retval;
}
