#pragma once

#include <assert.h>
#include <imgui.h>

#include <functional>
#include <string>
#include <vector>

#include "operation.h"

class ImGuiMenuItem {
   public:
    enum Options { SEPARATOR };
    ImGuiMenuItem(Operation operation);
    ImGuiMenuItem(Options o);
    void doit();
    void checkShortcut();

   private:
    bool _is_separator = false;
    Operation _operation;
};

class ImGuiMenuHeader {
   public:
    ImGuiMenuHeader(const std::string& name);
    void addItem(ImGuiMenuItem&& item);
    void addSeparator();
    void doit();
    void checkShortcut();

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
