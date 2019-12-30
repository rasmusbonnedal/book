#include "cellrenderertextcompletion.h"

#include "bolldoc.h"
#include "date.h"
#include "serialize.h"

#include <gtkmm.h>

#include <fstream>
#include <iostream>
#include <memory>

class VerifikatView : public Gtk::TreeView {
public:
    VerifikatView()
        : Gtk::TreeView(), m_cellRendererCompletion(createEntryCompletion()) {
        m_refTreeModel = Gtk::ListStore::create(m_columns);
        set_model(m_refTreeModel);
        m_columns.addColumns(*this);
        clear();
    }

    void clear() { m_refTreeModel->clear(); }

    void addRow(unsigned konto, const Pengar& pengar) {
        auto treeRow = *(m_refTreeModel->append());
        m_columns.setRow(treeRow, konto, pengar);
    }

    void updateKontoLista(const std::map<int, BollDoc::Konto>& kontoplan) {
        auto listStore = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(m_cellRendererCompletion.getCompletion()->get_model());
        listStore->clear();

        for (const auto& it : kontoplan) {
            auto row = *(listStore->append());
            m_completionRecord.setRow(row, std::to_string(it.first), it.second.getText());
        }
    }

private:
    Glib::RefPtr<Gtk::EntryCompletion> createEntryCompletion() {
        Glib::RefPtr<Gtk::EntryCompletion> completion =
            Gtk::EntryCompletion::create();
        Glib::RefPtr<Gtk::ListStore> listStore =
            Gtk::ListStore::create(m_completionRecord);
        completion->set_model(listStore);
        completion->set_text_column(m_completionRecord.m_colText);
        completion->pack_start(m_completionCellRenderer);
        completion->add_attribute(m_completionCellRenderer.property_text(),
                                  m_completionRecord.m_colText2);
        //        completion->set_popup_completion(true);
        completion->set_inline_selection(true);
        return completion;
    }

    void onEdited(const Glib::ustring& path_string,
                  const Glib::ustring& new_text) {
        int new_value = atoi(new_text.c_str());
        if (new_value) {
            Gtk::TreePath path(path_string);
            Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
            if (iter) {
                Gtk::TreeModel::Row row = *iter;
                row[m_columns.m_colKonto] = new_value;
            }
        }
    }

    class CompletionRecord : public Gtk::TreeModel::ColumnRecord {
    public:
        CompletionRecord() {
            add(m_colText);
            add(m_colText2);
        }
        void setRow(Gtk::TreeRow& row, const std::string& s,
                    const std::string& s2) {
            row[m_colText] = s;
            row[m_colText2] = s2;
        }
        Gtk::TreeModelColumn<Glib::ustring> m_colText;
        Gtk::TreeModelColumn<Glib::ustring> m_colText2;
    };
    CompletionRecord m_completionRecord;
    Gtk::CellRendererText m_completionCellRenderer;
    Gtk::TreeView::Column m_kontoColumn;

    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns() {
            add(m_colKonto);
            add(m_colPengar);
        }
        void addColumns(VerifikatView& treeView) {
            auto& cell = treeView.m_cellRendererCompletion;
            cell.property_editable() = true;
            cell.signal_edited().connect(
                sigc::mem_fun(&treeView, &VerifikatView::onEdited));
            auto& column = treeView.m_kontoColumn;
            column.set_title("Konto");
            column.pack_start(cell);
            column.add_attribute(cell.property_text(), m_colKonto);
            treeView.append_column(column);
            treeView.append_column_numeric_editable("Debet / Kredit",
                                                    m_colPengar, "%d kr");
        }
        void setRow(Gtk::TreeRow& row, unsigned konto, Pengar pengar) {
            row[m_colKonto] = konto;
            row[m_colPengar] = pengar.get() / 100;
        }
        Gtk::TreeModelColumn<unsigned> m_colKonto;
        Gtk::TreeModelColumn<int> m_colPengar;
    };
    ModelColumns m_columns;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
    CellRendererTextCompletion m_cellRendererCompletion;
};

void setVerifikat(VerifikatView& view, const BollDoc::Verifikat& verifikat) {
    view.clear();
    for (auto& rad : verifikat.getRader()) {
        view.addRow(rad.getKonto(), rad.getPengar());
    }
}

class GrundbokView : public Gtk::TreeView {
public:
    GrundbokView() : Gtk::TreeView() {
        m_refTreeModel = Gtk::ListStore::create(m_columns);
        set_model(m_refTreeModel);
        m_columns.addColumns(*this);
    }

    void setOnSelectionChanged(const Glib::SignalProxy<void>::SlotType& onSelectionChanged) {
        get_selection()->signal_changed().connect(onSelectionChanged);
    }

    void updateWithDoc(const BollDoc& doc) {
        m_refTreeModel->clear();
        for (int i = 0; i < 10; ++i) {
            for (auto& row : doc.getVerifikationer()) {
                auto treeRow = *(m_refTreeModel->append());
                std::stringstream date;
                date << row.getTransdatum();
                Pengar omslutning;
                row.getOmslutning(omslutning);
                std::stringstream omslutningStr;
                omslutningStr << omslutning << " kr";
                m_columns.setRow(treeRow, row.getUnid(), date.str(),
                                 row.getText(), omslutningStr.str());
            }
        }
    }

private:
    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns() {
            add(m_colId);
            add(m_colDate);
            add(m_colText);
            add(m_colOmslutning);
        }
        void addColumns(Gtk::TreeView& treeView) {
            treeView.append_column("ID", m_colId);
            treeView.append_column("Datum", m_colDate);
            treeView.append_column_editable("Text", m_colText);
            treeView.append_column("Omslutning", m_colOmslutning);
        }
        void setRow(Gtk::TreeRow& row, unsigned id, const std::string& date,
                    const std::string& text, const std::string& omslutning) {
            row[m_colId] = id;
            row[m_colDate] = date;
            row[m_colText] = text;
            row[m_colOmslutning] = omslutning;
        }

    private:
        Gtk::TreeModelColumn<unsigned> m_colId;
        Gtk::TreeModelColumn<Glib::ustring> m_colDate;
        Gtk::TreeModelColumn<Glib::ustring> m_colText;
        Gtk::TreeModelColumn<Glib::ustring> m_colOmslutning;
    };
    ModelColumns m_columns;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
};

class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow() {
        m_paned.set_orientation(Gtk::ORIENTATION_VERTICAL);
        m_scrolledWindow.set_size_request(400, 200);
        m_scrolledWindow.add(m_grundbokView);
        m_paned.add1(m_scrolledWindow);
        m_verifikatView.set_size_request(400, 50);
        m_paned.pack2(m_verifikatView, Gtk::AttachOptions::FILL);
        add(m_paned);

        m_grundbokView.setOnSelectionChanged(sigc::mem_fun(this, &MainWindow::onGrundbokSelectionChanged));

        m_header.set_title("Untitled");
        m_header.set_show_close_button(true);
        set_titlebar(m_header);
        loadFile("../docs/bok1.bollbok");
        show_all_children();
    }

private:
    void onGrundbokSelectionChanged() {
        unsigned id;
        m_grundbokView.get_selection()->get_selected()->get_value(0, id);
        setVerifikat(m_verifikatView, m_doc->getVerifikat(id));
    }

    void loadFile(const std::string& path) {
        std::ifstream ifs(path);
        if (ifs.good()) {
            m_header.set_title(path);
            set_titlebar(m_header);
            m_doc = std::make_unique<BollDoc>(Serialize::loadDocument(ifs));
            updateDoc();
        } else {
            std::cout << "Error loading " << path << std::endl;
        }
    }

    void updateDoc() {
        if (m_doc) {
            m_grundbokView.updateWithDoc(*m_doc);
            m_verifikatView.clear();
            m_verifikatView.updateKontoLista(m_doc->getKontoPlan());
        }
    }
    Gtk::ScrolledWindow m_scrolledWindow;
    GrundbokView m_grundbokView;
    VerifikatView m_verifikatView;
    Gtk::Box m_box;
    Gtk::Paned m_paned;
    Gtk::HeaderBar m_header;
    std::unique_ptr<BollDoc> m_doc;
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
        submenu_file->append("_Close", "win.close");
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