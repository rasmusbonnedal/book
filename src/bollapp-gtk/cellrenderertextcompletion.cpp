#include "cellrenderertextcompletion.h"

#include <gtkmm/entry.h>

void makeCellRendererUseCompletion(Gtk::CellRenderer& cell, const Glib::RefPtr<Gtk::EntryCompletion>& completion) {
    cell.signal_editing_started().connect(
        [completion](Gtk::CellEditable* editable, const Glib::ustring& path) {
        Gtk::Entry* entry = dynamic_cast<Gtk::Entry*>(editable);
        if (entry) {
            Glib::ustring s = entry->property_text();
            // Only edit the part up to the first space
            entry->set_text(s.substr(0, s.find(" ")));
            entry->set_completion(completion);
        }
    });
}
