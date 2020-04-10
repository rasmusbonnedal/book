//
// Copyright (C) 2019 Rasmus Bonnedal
//

#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gtkmm.h>
#pragma GCC diagnostic pop

/**
 * Make a int Gtk::TreeView column able to have an empty value.
 *
 * The purpose is to use a TreeView for entering data by adding an empty row
 * at the end. String columns can have an empty string as default but this lets
 * an int column be uninitialized which shows as an empty string. Editing the
 * value with an int will make it valid.
 * 
 * PRECONDITION: column has a GtkCellRendererText attached.
 *
 * @param column The TreeView column.
 * @param dataCol The source data column in the model.
 * @param validCol A column in the model to track if the data value is valid.
 */
void enableNumericColumnWithValid(Gtk::TreeViewColumn* column,
                                  const Gtk::TreeModelColumn<int>& dataCol,
                                  const Gtk::TreeModelColumn<bool>& validCol);
