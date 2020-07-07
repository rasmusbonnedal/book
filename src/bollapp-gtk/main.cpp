#include "cellrenderertextcompletion.h"
#include "grundbok_view.h"
#include "main_window.h"
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
