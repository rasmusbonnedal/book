#pragma once

#include <string>

class ImGuiDialog {
   public:
    ImGuiDialog(const std::string& name) : _name(name) {}

    virtual void doit() = 0;
    void launch();
    const std::string& name() const {
        return _name;
    }

   private:
    std::string _name;
};
