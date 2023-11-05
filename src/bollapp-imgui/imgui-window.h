#pragma once

#include <imgui.h>

#include <string>

class ImGuiWindowBase {
   public:
    ImGuiWindowBase(const std::string& name, const ImVec2& size = ImVec2(10, 10), const ImVec2& pos = ImVec2(80, 30))
        : _name(name), _size(size), _pos(pos) {}
    ~ImGuiWindowBase() {}
    virtual void doit() = 0;
    const std::string& name() const {
        return _name;
    }
    const ImVec2& pos() const {
        return _pos;
    }
    const ImVec2& size() const {
        return _size;
    }

   private:
    std::string _name;
    ImVec2 _size;
    ImVec2 _pos;
};
