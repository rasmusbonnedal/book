#include "bollbok_app.h"

#include "main_window.h"

BollBokApp::BollBokApp() : Gtk::Application("org.rasmus.bollbok") {
    Glib::set_application_name("Bollbok");
}

Glib::RefPtr<BollBokApp> BollBokApp::create() {
    return Glib::RefPtr<BollBokApp>(new BollBokApp());
}

void BollBokApp::on_activate() { create_window(); }

void BollBokApp::on_startup() {
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
    submenu_file->append("New _year same firm", "win.new_year");
    submenu_file->append("_Open", "win.open");
    submenu_file->append("_Save", "win.save");
    submenu_file->append("_Quit", "win.quit");
    win_menu->append_submenu("File", submenu_file);
    Glib::RefPtr<Gio::Menu> submenu_reports = Gio::Menu::create();
    submenu_reports->append("_Rapporter", "win.report.report");
    win_menu->append_submenu("Reports", submenu_reports);
    Glib::RefPtr<Gio::Menu> submenu_help = Gio::Menu::create();
    submenu_help->append("_About", "win.about");
    win_menu->append_submenu("Help", submenu_help);
    set_menubar(win_menu);

    add_accelerator("<control>N", "win.new", 0);
    add_accelerator("<control>O", "win.open", 0);
    add_accelerator("<control>S", "win.save", 0);
    add_accelerator("<control>Q", "win.quit", 0);
    add_accelerator("<control>R", "win.report.report", 0);
}

void BollBokApp::create_window() {
    auto window = new MainWindow();
    window->set_default_size(640, 480);
    add_window(*window);
    window->show_all();
}

void BollBokApp::on_action_something() {
    std::cout << "Something!" << std::endl;
}
