#pragma once

#include <gtkmm/cellrenderertext.h>
#include <gtkmm/entrycompletion.h>

/**
 * Adapted from Python https://stackoverflow.com/a/13769663
 **/

class CellRendererTextCompletion : public Gtk::CellRendererText {
public:
    CellRendererTextCompletion(
        Glib::RefPtr<Gtk::EntryCompletion> entryCompletion);

protected:
    virtual Gtk::CellEditable* start_editing_vfunc(
        GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path,
        const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area,
        Gtk::CellRendererState flags);
    void editing_done(const Gtk::Entry& entry, const Glib::ustring& path);
    Glib::RefPtr<Gtk::EntryCompletion> m_completion;
};
