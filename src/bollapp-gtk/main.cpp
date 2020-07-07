#include "cellrenderertextcompletion.h"
#include "grundbok_view.h"
#include "treeview_enable_edit_next_field.h"
#include "verifikat_view.h"

#include "bolldoc.h"
#include "date.h"
#include "serialize.h"
#include "utils.h"

#include <gtkmm.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

void setVerifikat(VerifikatView& view, const BollDoc::Verifikat& verifikat) {
    view.clear();
    view.addRow(0, 0, Date(), std::nullopt);
    for (auto& rad : verifikat.getRader()) {
        view.addRow(rad.getKonto(), rad.getPengar(), rad.getBokdatum(), rad.getStruken());
    }
}

class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow() {
        add_action("new",
                   sigc::mem_fun(*this, &MainWindow::on_action_new));
        add_action("open",
                   sigc::mem_fun(*this, &MainWindow::on_action_open));
        add_action("save",
                   sigc::mem_fun(*this, &MainWindow::on_action_save));
        add_action("quit",
                   sigc::mem_fun(*this, &MainWindow::on_action_quit));

        m_paned.set_orientation(Gtk::ORIENTATION_VERTICAL);
        m_scrolledWindow.set_size_request(400, 200);
        m_scrolledWindow.add(m_grundbokView);
        m_paned.add1(m_scrolledWindow);
        m_verifikatView.set_size_request(400, 50);
        m_paned.pack2(m_verifikatView, Gtk::AttachOptions::FILL);
        add(m_paned);

        m_grundbokView.setOnSelectionChanged(sigc::mem_fun(this, &MainWindow::onGrundbokSelectionChanged));

        m_verifikatView.signalEdited().connect(sigc::mem_fun(this, &MainWindow::onVerifikatViewEdited));

        m_grundbokView.setOnEditedDate(sigc::mem_fun(this, &MainWindow::onVerifikatDateEdited));

        m_grundbokView.setOnEditedText(sigc::mem_fun(this, &MainWindow::onVerifikatTextEdited));

        m_header.set_title("Untitled");
        m_header.set_show_close_button(true);
        set_titlebar(m_header);
        set_icon_from_file("src/bollapp-gtk/icon.png");
        loadFile("../docs/bok1.bollbok");
        m_filename.clear();
        show_all_children();
    }

private:
    void on_action_new() {
        int version = 2074;
        std::string firma = "Ny firma";
        std::string orgnummer = "556614-1234";
        int bokforingsar = 2019;
        std::string valuta = "SEK";
        m_doc = std::make_unique<BollDoc>(version, firma, orgnummer, bokforingsar, valuta, false);
        updateDoc();
        m_header.set_title("Untitled");
    }

    void on_action_open() {
        Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
        dialog.set_transient_for(*this);

        //Add response buttons the the dialog:
        dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        dialog.add_button("_Open", Gtk::RESPONSE_OK);

        auto filter_bollbok = Gtk::FileFilter::create();
        filter_bollbok->set_name("Bollbok files");
        filter_bollbok->add_pattern("*.bollbok");
        dialog.add_filter(filter_bollbok);

        //Show the dialog and wait for a user response:
        if (dialog.run() == Gtk::RESPONSE_OK) {
            loadFile(dialog.get_filename());
        }
    }

    void on_action_save() {
        if (m_filename.empty()) {
            Gtk::FileChooserDialog dialog("Please choose file to save", Gtk::FILE_CHOOSER_ACTION_SAVE);
            dialog.set_transient_for(*this);

            //Add response buttons the the dialog:
            dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
            dialog.add_button("_Save", Gtk::RESPONSE_OK);

            auto filter_bollbok = Gtk::FileFilter::create();
            filter_bollbok->set_name("Bollbok files");
            filter_bollbok->add_pattern("*.bollbok");
            dialog.add_filter(filter_bollbok);

            //Show the dialog and wait for a user response:
            if (dialog.run() == Gtk::RESPONSE_OK) {
                m_filename = dialog.get_filename();
                if (!Utils::endsWith(m_filename, ".bollbok")) {
                    m_filename += ".bollbok";
                }
            } else {
                return;
            }
        }        
        saveFile(m_filename);
    }

    void on_action_quit() {
        hide();
    }

    void onGrundbokSelectionChanged() {
        unsigned id;
        m_grundbokView.get_selection()->get_selected()->get_value(0, id);
        std::cout << "Changing from " << m_verifikatEditingId << " to " << id << std::endl;
        setVerifikat(m_verifikatView, m_doc->getVerifikat(id));
        m_verifikatEditingId = id;
    }

    void onVerifikatViewEdited(const std::vector<BollDoc::Rad>& rader) {
        std::cout << "onVerifikatViewEdited()" << std::endl;
        for (auto& x : rader) {
            std::cout << x << std::endl;
        }
        if (m_verifikatEditingId >= 0) {
            m_doc->updateVerifikat(m_verifikatEditingId, rader);
            m_grundbokView.recalculate(*m_doc);
            m_grundbokView.addNewVerifikatRow(*m_doc);
        }
    }

    void onVerifikatDateEdited(unsigned int id, const Date& date) {
        m_doc->setVerifikatTransdatum(id, date);
    }

    void onVerifikatTextEdited(unsigned int id, const Glib::ustring& text) {
        m_doc->setVerifikatText(id, text);
    }

    void loadFile(const std::string& path) {
        std::ifstream ifs(path);
        if (ifs.good()) {
            size_t pos = path.find_last_of('/');
            std::string filename = (pos == std::string::npos) ? path : path.substr(pos + 1);
            m_header.set_title(filename);
            set_titlebar(m_header);
            m_doc = std::make_unique<BollDoc>(Serialize::loadDocument(ifs));
            updateDoc();
            m_filename = path;
        } else {
            std::cout << "Error loading " << path << std::endl;
        }
    }

    void saveFile(const std::string& path) {
        std::ofstream ofs(path);
        if (ofs.good()) {
            Serialize::saveDocument(*m_doc, ofs);
        } else {
            std::cout << "Error saving " << path << std::endl;
        }
    }

    void updateDoc() {
        if (m_doc) {
            m_grundbokView.updateWithDoc(*m_doc);
            m_grundbokView.addNewVerifikatRow(*m_doc);
            m_verifikatView.clear();
            m_verifikatView.updateKontoLista(m_doc->getKontoPlan());
            m_verifikatEditingId = -1;
        }
    }
    Gtk::ScrolledWindow m_scrolledWindow;
    GrundbokView m_grundbokView;
    VerifikatView m_verifikatView;
    Gtk::Box m_box;
    Gtk::Paned m_paned;
    Gtk::HeaderBar m_header;
    std::unique_ptr<BollDoc> m_doc;
    int m_verifikatEditingId;
    std::string m_filename;
};

class BollBokApp : public Gtk::Application {
public:
    BollBokApp() : Gtk::Application("org.rasmus.bollbok") {
        Glib::set_application_name("Bollbok");
    }

    static Glib::RefPtr<BollBokApp> create() {
        return Glib::RefPtr<BollBokApp>(new BollBokApp());
    }

protected:
    void on_startup() override {
        Gtk::Application::on_startup();

        add_action("something",
                   sigc::mem_fun(*this, &BollBokApp::on_action_something));

        Glib::RefPtr<Gio::Menu> app_menu = Gio::Menu::create();
        auto s1 = Gio::Menu::create();
        s1->append("_Something", "app.something");
        s1->append("_Quit", "app.quit");
        app_menu->append_section(s1);
        set_app_menu(app_menu);

        Glib::RefPtr<Gio::Menu> win_menu = Gio::Menu::create();
        Glib::RefPtr<Gio::Menu> submenu_file = Gio::Menu::create();
        submenu_file->append("_New", "win.new");
        submenu_file->append("_Open", "win.open");
        submenu_file->append("_Save", "win.save");
        submenu_file->append("_Quit", "win.quit");
        win_menu->append_submenu("File", submenu_file);
        set_menubar(win_menu);
    }

    void on_activate() override { create_window(); }

private:
    void create_window() {
        auto window = new MainWindow();
        window->set_default_size(640, 480);
        add_window(*window);
        window->show_all();
    }

    void on_action_something() { std::cout << "Something!" << std::endl; }
};

int main(int argc, char** argv) {
    auto app = BollBokApp::create();
    return app->run(argc, argv);
}
