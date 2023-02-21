#pragma once

#include <functional>
#include <string>
#include <vector>

class ImGuiMenuItem {
public:
    enum Options { SEPARATOR };
    ImGuiMenuItem(const std::string& label, const std::string& shortcut, std::function<void()> event);
    ImGuiMenuItem(Options o);
    void doit();

private:
    std::string _label, _shortcut;
    bool _is_separator = false;
    std::function<void()> _event;
};

class ImGuiMenuHeader {
public:
    ImGuiMenuHeader(const std::string& name);
    void addItem(ImGuiMenuItem&& item);
    void addSeparator();
    void doit();

private:
    std::vector<ImGuiMenuItem> _items;
    std::string _name;
};

class ImGuiMenu {
public:
    void addHeader(ImGuiMenuHeader&& header);
    void doit();

private:
    std::vector<ImGuiMenuHeader> _headers;
};
