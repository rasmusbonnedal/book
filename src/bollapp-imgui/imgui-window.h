#pragma once

#include <string>

class ImGuiWindowBase {
   public:
    ImGuiWindowBase(const std::string& name) : _name(name) {}
    ~ImGuiWindowBase() {}
    virtual void doit() = 0;
    const std::string& name() const {
        return _name;
    }

   private:
    std::string _name;
};
