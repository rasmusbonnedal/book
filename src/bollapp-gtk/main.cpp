#include "cellrenderertextcompletion.h"
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

class GrundbokView : public Gtk::TreeView {
public:
    GrundbokView() : Gtk::TreeView() {
        m_refTreeModel = Gtk::ListStore::create(m_columns);
        m_sortedModel = Gtk::TreeModelSort::create(m_refTreeModel);
        m_sortedModel->set_sort_column(1, Gtk::SORT_DESCENDING);
        m_sortedModel->set_sort_func(1, sigc::mem_fun(this, &GrundbokView::sortFunction));
        set_model(m_sortedModel);
        m_columns.addColumns(*this);
    }

    void setOnSelectionChanged(const Glib::SignalProxy<void>::SlotType& onSelectionChanged) {
        get_selection()->signal_changed().connect(onSelectionChanged);
    }

    void setOnEditedDate(const Glib::SignalProxy<void, unsigned int, const Date&>::SlotType& onEditedDate) {
        m_signalDateEdited.connect(onEditedDate);
    }

    void setOnEditedText(const Glib::SignalProxy<void, unsigned int, const Glib::ustring&>::SlotType& onEditedText) {
        m_signalTextEdited.connect(onEditedText);
    }

    void recalculate(const BollDoc& doc) {
        for (auto& treeRow : m_refTreeModel->children()) {
            unsigned id = m_columns.getId(treeRow);
            const auto& row = doc.getVerifikat(id);
            std::stringstream date;
            date << row.getTransdatum();
            Pengar omslutning;
            std::stringstream omslutningStr;
            if (row.getRader().size() == 0) {
            } else if (row.getOmslutning(omslutning)) {
                omslutningStr << omslutning << " kr";
            } else {
                omslutningStr << " - obalanserad - ";
            }
            m_columns.setRow(treeRow, row.getUnid(), date.str(), row.getText(),
                             omslutningStr.str());
        }
    }

    void updateWithDoc(const BollDoc& doc) {
        m_refTreeModel->clear();
        for (const auto& row : doc.getVerifikationer()) {
            auto treeRow = *(m_refTreeModel->append());
            Pengar omslutning;
            std::stringstream omslutningStr;
            if (row.getOmslutning(omslutning)) {
                omslutningStr << omslutning << " kr";
            } else {
                omslutningStr << " - obalanserad - ";
            }
            m_columns.setRow(treeRow, row.getUnid(), to_string(row.getTransdatum()),
                                row.getText(), omslutningStr.str());
        }
        m_year = doc.getBokforingsar();
    }

    void addNewVerifikatRow(BollDoc& doc) {
        const std::string newVerifikatText("Nytt verifikat");
        // Don't add if last row has newVerifikatText or empty omslutning
        if (!m_refTreeModel->children().empty()) {
            auto lastTreeRow = *(--m_refTreeModel->children().end());
            if (m_columns.getText(lastTreeRow) == newVerifikatText || m_columns.getOmslutning(lastTreeRow).empty()) {
                std::cout << "Text: " << m_columns.getText(lastTreeRow) << std::endl;
                std::cout << "Omslutning: " << m_columns.getOmslutning(lastTreeRow) << std::endl;
                return;
            }
        }

        int nextId = doc.getNextVerifikatId();
        Date nextDate(0, 1, 1);
        for (const auto& row : m_refTreeModel->children()) {
            nextDate = std::max(nextDate, parseDate(m_columns.getDate(row)));
        }
        
        auto treeRow = *(m_refTreeModel->append());
        m_columns.setRow(treeRow, nextId, to_string(nextDate), newVerifikatText, "");
        BollDoc::Verifikat v(nextId, newVerifikatText, nextDate);
        doc.addVerifikat(std::move(v));
    }

private:
    void onEditedText(const Glib::ustring& path_string,
                      const Glib::ustring& new_text) {
        Gtk::TreePath path = m_sortedModel->convert_path_to_child_path(Gtk::TreePath(path_string));
        Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            m_columns.setText(row, new_text);
            unsigned int id = m_columns.getId(row);
            m_signalTextEdited(id, new_text);
        }
    }

    void onEditedDate(const Glib::ustring& path_string,
                      const Glib::ustring& new_text) {
        Gtk::TreePath path = m_sortedModel->convert_path_to_child_path(Gtk::TreePath(path_string));
        Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
        if (iter) {
            std::optional<Date> date = parseDateNothrow(new_text);
            if (date && date->getYear() == m_year) {
                Gtk::TreeModel::Row row = *iter;
                m_columns.setDate(row, new_text);
                unsigned int id = m_columns.getId(row);
                m_signalDateEdited(id, *date);
            }
        }
    }

    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns() {
            add(m_colId);
            add(m_colDate);
            add(m_colText);
            add(m_colOmslutning);
        }

        void addColumns(GrundbokView& treeView) {
            treeView.append_column("ID", m_colId);
            treeView.get_column(0)->set_sort_column(m_colId);
            treeView.append_column("Datum", m_colDate);
            treeView.get_column(1)->set_sort_column(m_colDate);
            Gtk::CellRendererText* dateCell = dynamic_cast<Gtk::CellRendererText*>(treeView.get_column_cell_renderer(1));
            if (dateCell) {
                dateCell->property_editable() = true;
                dateCell->signal_edited().connect(sigc::mem_fun(&treeView, &GrundbokView::onEditedDate));
            }            

            treeView.append_column_editable("Text", m_colText);
            Gtk::CellRendererText* textCell = dynamic_cast<Gtk::CellRendererText*>(treeView.get_column_cell_renderer(2));
            if (textCell) {
                textCell->signal_edited().connect(sigc::mem_fun(&treeView, &GrundbokView::onEditedText));
            }            
            treeView.append_column("Omslutning", m_colOmslutning);
        }

        void setRow(const Gtk::TreeRow& row, unsigned id, const std::string& date,
                    const std::string& text, const std::string& omslutning) {
            row[m_colId] = id;
            row[m_colDate] = date;
            row[m_colText] = text;
            row[m_colOmslutning] = omslutning;
        }

        unsigned getId(const Gtk::TreeRow& row) const {
            return row[m_colId];
        }

        std::string getDate(const Gtk::TreeRow& row) const {
            return Glib::ustring(row[m_colDate]);         
        }

        void setDate(const Gtk::TreeRow& row, const std::string& date) {
            row[m_colDate] = date;
        }

        std::string getText(const Gtk::TreeRow& row) const {
            return Glib::ustring(row[m_colText]);
        }

        void setText(const Gtk::TreeRow& row, const std::string& text) {
            row[m_colText] = text;
        }

        std::string getOmslutning(const Gtk::TreeRow& row) const {
            return Glib::ustring(row[m_colOmslutning]);
        }

    private:
        Gtk::TreeModelColumn<unsigned> m_colId;
        Gtk::TreeModelColumn<Glib::ustring> m_colDate;
        Gtk::TreeModelColumn<Glib::ustring> m_colText;
        Gtk::TreeModelColumn<Glib::ustring> m_colOmslutning;
    };

    int sortFunction(const Gtk::TreeModel::iterator& lhs, const Gtk::TreeModel::iterator& rhs) {
        std::string lhsDate = m_columns.getDate(*lhs);
        std::string rhsDate = m_columns.getDate(*rhs);
        if (lhsDate == rhsDate) {
            return m_columns.getId(*lhs) - m_columns.getId(*rhs);
        }
        return lhsDate.compare(rhsDate);
    }

    ModelColumns m_columns;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
    Glib::RefPtr<Gtk::TreeModelSort> m_sortedModel;
    sigc::signal<void, unsigned int, const Glib::ustring&> m_signalTextEdited;
    sigc::signal<void, unsigned int, const Date&> m_signalDateEdited;
    int m_year;
};

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
