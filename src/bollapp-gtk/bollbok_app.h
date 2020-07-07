#pragma once

#include <gtkmm.h>

class BollBokApp : public Gtk::Application {
public:
    BollBokApp();

    static Glib::RefPtr<BollBokApp> create();

protected:
    void on_startup() override;

    void on_activate() override;

private:
    void create_window();

    void on_action_something();
};
