#pragma once

#include <gtkmm/cellrenderertext.h>
#include <gtkmm/entrycompletion.h>

void makeCellRendererUseCompletion(Gtk::CellRenderer& cell, const Glib::RefPtr<Gtk::EntryCompletion>& completion);
