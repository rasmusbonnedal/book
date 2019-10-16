#include "bolldoc.h"
#include "date.h"
#include "serialize.h"

#include <gtkmm.h>

#include <fstream>
#include <iostream>
#include <memory>

class ModelColumns : public Gtk::TreeModel::ColumnRecord {
public:
    void addColumns(Gtk::TreeView& treeView) {
        add(m_colId);
        treeView.append_column("ID", m_colId);
        add(m_colDate);
        treeView.append_column("Datum", m_colDate);
        add(m_colText);
        treeView.append_column("Text", m_colText);
        add(m_colOmslutning);
        treeView.append_column("Omslutning", m_colOmslutning);
    }
    void setRow(Gtk::TreeRow& row, unsigned id, const std::string& date, const std::string& text, const std::string& omslutning) {
        row[m_colId] = id;
        row[m_colDate] = date;
        row[m_colText] = text;
        row[m_colOmslutning] = omslutning;
    }
protected:
    Gtk::TreeModelColumn<unsigned> m_colId;
    Gtk::TreeModelColumn<Glib::ustring> m_colDate;
    Gtk::TreeModelColumn<Glib::ustring> m_colText;
    Gtk::TreeModelColumn<Glib::ustring> m_colOmslutning;
};

class HelloWorld : public Gtk::ApplicationWindow {
public:
    HelloWorld() : 
        m_b1("Hej"),
        m_paned(Gtk::ORIENTATION_VERTICAL) {
        m_columns.addColumns(m_treeView);
        m_refTreeModel = Gtk::ListStore::create(m_columns);
        m_treeView.set_model(m_refTreeModel);
 
        m_paned.add1(m_treeView);
        m_paned.add2(m_b1);
        add(m_paned);
        m_header.set_title("Untitled");
        m_header.set_show_close_button(true);
        set_titlebar(m_header);
        loadFile("../docs/output.bollbok");
        show_all_children();
    }
private:
    void loadFile(const std::string& path) {
        std::ifstream ifs(path);
        if (ifs.good()) {
            m_header.set_title(path);
            set_titlebar(m_header);
            m_doc = std::make_unique<BollDoc>(Serialize::loadDocument(ifs));
            updateDoc();
            show_all_children();
        }
    }

    void updateDoc() {
        m_refTreeModel->clear();
        if (!m_doc) {
            return;
        }
        for (auto& row : m_doc->getVerifikationer()) {
            auto treeRow = *(m_refTreeModel->append());
            std::stringstream date;
            date << row.getTransdatum();
            Pengar omslutning;
            row.getOmslutning(omslutning);
            std::stringstream omslutningStr;
            omslutningStr << omslutning << " kr";
            m_columns.setRow(treeRow,
                row.getUnid(),
                date.str(),
                row.getText(),
                omslutningStr.str());
        }
    }
    Gtk::Button m_b1;
    Gtk::Paned m_paned;
    Gtk::TreeView m_treeView;
    ModelColumns m_columns;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
    Gtk::HeaderBar m_header;
    std::unique_ptr<BollDoc> m_doc;
};

class BollBokApp : public Gtk::Application {
public:
    BollBokApp() 
        :Gtk::Application("org.rasmus.bollbok") {
        Glib::set_application_name("Bollbok");
    }

    static Glib::RefPtr<BollBokApp> create() {
        return Glib::RefPtr<BollBokApp>(new BollBokApp());
    }
protected:
    void on_startup() override {
        Gtk::Application::on_startup();
        printf("on_startup()\n");

        add_action("something",
            sigc::mem_fun(*this, &BollBokApp::on_action_something));

        {
            Glib::RefPtr<Gio::Menu> app_menu = Gio::Menu::create();
            auto s1 = Gio::Menu::create();
            s1->append("_Something", "app.something");
            s1->append("_Quit", "app.quit");
            app_menu->append_section(s1);
            set_app_menu(app_menu);
        }

        Glib::RefPtr<Gio::Menu> win_menu = Gio::Menu::create();
        Glib::RefPtr<Gio::Menu> submenu_file = Gio::Menu::create();
        submenu_file->append("_New", "win.new");
        submenu_file->append("_Open", "win.open");
        submenu_file->append("_Close", "win.close");
        win_menu->append_submenu("File", submenu_file);
        set_menubar(win_menu);
    }

    void on_activate() override {
        create_window();
    }

private:
    void create_window() {
        printf("create_window()\n");
        auto window = new HelloWorld();
        window->set_default_size(300, 100);
        add_window(*window);
        std::cout << "Id: " << window->get_id() << std::endl;

        window->show_all();
    }

    void on_action_something() {
        std::cout << "Something!" << std::endl;
    }
};

int main(int argc, char** argv) {
//    auto app = Gtk::Application::create(argc, argv, "org.rasmus.bollbok");
    auto app = BollBokApp::create();
    return app->run(argc, argv);
//    HelloWorld hello;
//    app->add_window(hello);
//    return app->run(hello);
}