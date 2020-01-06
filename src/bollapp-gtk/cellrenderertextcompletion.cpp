#include "cellrenderertextcompletion.h"

#include <gtkmm/entry.h>

CellRendererTextCompletion::CellRendererTextCompletion(
    Glib::RefPtr<Gtk::EntryCompletion> entryCompletion)
    : m_completion(entryCompletion) {}

Glib::RefPtr<Gtk::EntryCompletion> CellRendererTextCompletion::getCompletion() {
    return m_completion;
}

Gtk::CellEditable* CellRendererTextCompletion::start_editing_vfunc(
    GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path,
    const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area,
    Gtk::CellRendererState flags) {
    if (!property_editable()) {
        return 0;
    }
    Gtk::Entry* entry = Gtk::make_managed<Gtk::Entry>();
    Glib::ustring s = property_text();
    // Only edit the part up to the first space
    entry->set_text(s.substr(0, s.find(" ")));
    entry->set_completion(m_completion);
    entry->signal_editing_done().connect(sigc::bind(
        sigc::mem_fun(*this, &CellRendererTextCompletion::editing_done),
        sigc::ref(*entry), path));
    entry->show();
    entry->grab_focus();
    return entry;
}

void CellRendererTextCompletion::editing_done(const Gtk::Entry& entry,
                                              const Glib::ustring& path) {
    if (!entry.property_editing_canceled()) {
        edited(path, entry.get_text());
    }
}
