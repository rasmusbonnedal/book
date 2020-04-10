//
// Copyright (C) 2019 Rasmus Bonnedal
//

#include "treeview_enable_edit_next_field.h"

#include <iostream>

namespace {
void setCursorNextFieldHandler(const Glib::ustring& path_string,
                               const Glib::ustring& new_text,
                               Gtk::TreeViewColumn* column,
                               Gtk::TreeViewColumn* colNext, bool nextRow) {
    Gtk::TreeView* treeView = column->get_tree_view();
    Gtk::TreePath currpath(path_string);
    Gtk::TreePath nextpath(currpath);
    std::cout << "nextpath: " << nextpath << std::endl;
    if (nextRow) {
        nextpath.next();
        Glib::RefPtr<Gtk::ListStore> model =
            Glib::RefPtr<Gtk::ListStore>::cast_dynamic(treeView->get_model());
        // If path.next() is not a valid row, append new row for editing
        if (!model->get_iter(nextpath)) {
            model->append();
        }
    }
    // Hack to work around problem setting cursor
    // See:
    // https://github.com/bbuhler/HostsManager/commit/bfa95e32dce484b79d4dc023cb48f3249dc3ff7a
    // https://gitlab.com/inkscape/inkscape/merge_requests/801/diffs?commit_id=40b6f70127755d2b44597e56f5bd965083eda3f2
    Glib::signal_timeout().connect_once(
        [treeView, currpath, column, nextpath, colNext]() {
            Gtk::TreePath focusPath;
            Gtk::TreeViewColumn* focusColumn;
            treeView->get_cursor(focusPath, focusColumn);
            if (focusPath == currpath && focusColumn == column) {
                std::cout << "Setting path to " << nextpath << std::endl;
                treeView->set_cursor(nextpath, *colNext, true);
            }
        },
        1);
}

void setCursorNextField(Gtk::TreeViewColumn* column,
                        Gtk::TreeViewColumn* colNext, bool nextRow) {
    Gtk::CellRendererText* crt =
        dynamic_cast<Gtk::CellRendererText*>(column->get_first_cell());
    g_return_if_fail(crt != nullptr);
    crt->signal_edited().connect(
        sigc::bind(&setCursorNextFieldHandler, column, colNext, nextRow));
}
} // namespace

void enableEditNextField(const std::vector<Gtk::TreeViewColumn*>& columns) {
    size_t nColumn = columns.size();
    g_return_if_fail(nColumn >= 2);

    Gtk::TreeView* treeView = columns[0]->get_tree_view();
    Glib::RefPtr<Gtk::ListStore> model =
        Glib::RefPtr<Gtk::ListStore>::cast_dynamic(treeView->get_model());
    g_return_if_fail(model);

    for (size_t i = 0; i < nColumn - 1; ++i) {
        setCursorNextField(columns[i], columns[i + 1], false);
    }
    setCursorNextField(columns[nColumn - 1], columns[0], true);
}
