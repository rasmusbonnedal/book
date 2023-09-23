#pragma once

#include <string>

class ImGuiDialog {
   public:
    ImGuiDialog(const std::string& name) : _name(name), _launch(false) {}

    virtual void doit() = 0;
    void launch();
    void actualLaunch();
    const std::string& name() const {
        return _name;
    }
    void setName(const std::string& name) {
        _name = name;
    }

   private:
    bool _launch;
    std::string _name;
};
