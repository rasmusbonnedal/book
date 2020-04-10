//
// Copyright (C) 2019 Rasmus Bonnedal
//

#include "treeview_enable_valid.h"

#include <glib.h>

namespace {
void onEditedHandler(const Glib::ustring& path_string,
                     const Glib::ustring& new_text,
                     Glib::RefPtr<Gtk::TreeModel> treeModel,
                     const Gtk::TreeModelColumn<int>& dataCol,
                     const Gtk::TreeModelColumn<bool>& validCol) {
    Gtk::TreePath path(path_string);
    Gtk::TreeModel::iterator iter = treeModel->get_iter(path);
    if (!iter) {
        return;
    }
    Gtk::TreeModel::Row row = *iter;

    try {
        int new_value = std::stoi(new_text.c_str());
        row[dataCol] = new_value;
        row[validCol] = true;
    } catch (std::invalid_argument& e) {
        row[validCol] = false;
    }
}

void onCellDataHandler(Gtk::CellRenderer* renderer,
                       const Gtk::TreeModel::iterator& iter,
                       const Gtk::TreeModelColumn<int>& dataCol,
                       const Gtk::TreeModelColumn<bool>& validCol) {
    Gtk::CellRendererText* text_renderer =
        dynamic_cast<Gtk::CellRendererText*>(renderer);
    if (iter && text_renderer) {
        Gtk::TreeModel::Row row = *iter;
        int model_value = row[dataCol];
        bool valid = row[validCol];

        Glib::ustring text;
        if (valid) {
            text = Glib::ustring::format(model_value);
        }
        text_renderer->property_text() = text;
    }
}
} // namespace

void enableNumericColumnWithValid(Gtk::TreeViewColumn* column,
                                  const Gtk::TreeModelColumn<int>& dataCol,
                                  const Gtk::TreeModelColumn<bool>& validCol) {
    Gtk::CellRendererText* crt =
        dynamic_cast<Gtk::CellRendererText*>(column->get_first_cell());
    g_return_if_fail(crt != nullptr);
    crt->signal_edited().connect(
        sigc::bind(&onEditedHandler, column->get_tree_view()->get_model(),
                   dataCol, validCol));
    column->set_cell_data_func(
        *crt, sigc::bind(&onCellDataHandler, dataCol, validCol));
}
