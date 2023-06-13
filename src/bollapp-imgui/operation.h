#pragma once

#include <functional>
#include <string>

#include "shortcut.h"

class Operation {
   public:
    Operation() {}

    Operation(const std::string& label, Shortcut shortcut, std::function<void()> event)
        : _label(label), _shortcut(shortcut), _event(event) {}

    Operation(const std::string& label, std::function<void()> event)
        : _label(label), _event(event) {}

    const std::string& label() const {
        return _label;
    }

    const std::string& shortcut() const {
        return _shortcut.text();
    }

    bool isPressed() const {
        return _shortcut.isPressed();
    }

    void trigger() const {
        _event();
    }

   private:
    std::string _label;
    Shortcut _shortcut;
    std::function<void()> _event;
};
