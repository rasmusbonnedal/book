#pragma once

#include "grundbok_view.h"
#include "verifikat_view.h"

#include "bolldoc.h"
#include "date.h"
#include "serialize.h"

#include <gtkmm.h>

#include <memory>
#include <string>

class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow();

private:
    void on_action_new();

    void on_action_open();

    void on_action_save();

    void on_action_quit();

    void onGrundbokSelectionChanged();

    void onVerifikatViewEdited(const std::vector<BollDoc::Rad>& rader);

    void onVerifikatDateEdited(unsigned int id, const Date& date);

    void onVerifikatTextEdited(unsigned int id, const Glib::ustring& text);

    void loadFile(const std::string& path);

    void saveFile(const std::string& path);

    void updateDoc();

    Gtk::ScrolledWindow m_grundbokScroll;
    GrundbokView m_grundbokView;
    Gtk::ScrolledWindow m_verifikatScroll;
    VerifikatView m_verifikatView;
    Gtk::Box m_box;
    Gtk::Paned m_paned;
    Gtk::HeaderBar m_header;
    std::unique_ptr<BollDoc> m_doc;
    int m_verifikatEditingId;
    std::string m_filename;
};
