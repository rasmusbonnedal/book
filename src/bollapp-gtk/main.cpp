#include <gtkmm/button.h>
#include <gtkmm/paned.h>
#include <gtkmm/window.h>
#include <gtkmm.h>

class ModelColumns : public Gtk::TreeModel::ColumnRecord {
public:
    ModelColumns() {
        add(m_colId);
        add(m_colDate);
        add(m_colText);
        add(m_colOmslutning);
    }
    Gtk::TreeModelColumn<unsigned> m_colId;
    Gtk::TreeModelColumn<Glib::ustring> m_colDate;
    Gtk::TreeModelColumn<Glib::ustring> m_colText;
    Gtk::TreeModelColumn<int> m_colOmslutning;
};

class HelloWorld : public Gtk::Window {
public:
    HelloWorld() : 
        m_b1("Hej"),
        m_paned(Gtk::ORIENTATION_VERTICAL) {
        m_refTreeModel = Gtk::ListStore::create(m_columns);
        m_treeView.set_model(m_refTreeModel);
        auto row = *(m_refTreeModel->append());
        row[m_columns.m_colId] = 1;
        row[m_columns.m_colDate] = "2019-01-01";
        row[m_columns.m_colText] = "Hej hopp";
        row[m_columns.m_colOmslutning] = 1000;
        m_treeView.append_column("ID", m_columns.m_colId);
        m_treeView.append_column("Date", m_columns.m_colDate);
        m_treeView.append_column("Text", m_columns.m_colText);
        m_treeView.append_column_numeric("Omslutning", m_columns.m_colOmslutning, "%d kr");

        m_paned.add1(m_treeView);
        m_paned.add2(m_b1);
        add(m_paned);
        show_all_children();
    }
private:
    Gtk::Button m_b1;
    Gtk::Paned m_paned;
    Gtk::TreeView m_treeView;
    ModelColumns m_columns;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
};

int main(int argc, char** argv) {
    auto app = Gtk::Application::create(argc, argv, "org.rasmus.bollbok");

    HelloWorld hello;
    return app->run(hello);
}