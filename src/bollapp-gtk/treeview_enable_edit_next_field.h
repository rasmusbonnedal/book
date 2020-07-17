//
// Copyright (C) 2019 Rasmus Bonnedal
//

#pragma once

#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gtkmm.h>
#pragma GCC diagnostic pop

/**
 * Add jump to next field when edited behavior to Gtk::TreeView columns
 *
 * Enables the TreeView to be a kind of input form where the user can input
 * data and press enter to start editing the next field. When the last field
 * is edited it jumps to the first field of the next row. If editing the last
 * row a new row will be appended to the model.
 * 
 * This has been hacked for bollapp to move upwards when edited last
 * column of a row. It depends on a new row prepended somewhere else.
 * 
 * PRECONDITION: At least two columns specified.
 * PRECONDITION: All columns have a GtkCellRendererText attached.
 * PRECONDITION: The TreeView has a Gtk::ListStore model.
 *
 * @param columns The columns in the order they should be edited.
 */
void enableEditNextField(const std::vector<Gtk::TreeViewColumn*>& columns);
