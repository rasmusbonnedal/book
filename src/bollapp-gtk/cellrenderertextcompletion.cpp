#include "cellrenderertextcompletion.h"

#include <gtkmm/entry.h>

#include <iostream>

void makeCellRendererUseCompletion(Gtk::CellRenderer& cell, const Glib::RefPtr<Gtk::EntryCompletion>& completion) {
    cell.signal_editing_started().connect(
        [completion](Gtk::CellEditable* editable, const Glib::ustring& path) {
        Gtk::Entry* entry = dynamic_cast<Gtk::Entry*>(editable);
        if (entry) {
            Glib::ustring s = entry->property_text();
            
            // TODO: Hard coded handling for konto, should be generalized
            // Only edit the part up to the first space, if numeric
            Glib::ustring text = "";
            try {
                if (s.size() > 0 && isdigit(s[0])) {
                    int value = std::stoi(s.c_str());
                    text = Glib::ustring::format(value);
                }
            } catch (std::invalid_argument& e) {
            }
            entry->set_text(text);
            entry->set_completion(completion);
        }
    });
}
