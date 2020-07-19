#include "main_window.h"

#include "utils.h"

#include <fstream>

namespace {
void setVerifikat(VerifikatView& view, const BollDoc::Verifikat& verifikat) {
    view.clear();
    view.addRow(0, 0, now(), std::nullopt);
    for (auto& rad : verifikat.getRader()) {
        view.addRow(rad.getKonto(), rad.getPengar(), rad.getBokdatum(),
                    rad.getStruken());
    }
}
} // namespace

MainWindow::MainWindow() {
    add_action("new", sigc::mem_fun(*this, &MainWindow::on_action_new));
    add_action("open", sigc::mem_fun(*this, &MainWindow::on_action_open));
    add_action("save", sigc::mem_fun(*this, &MainWindow::on_action_save));
    add_action("quit", sigc::mem_fun(*this, &MainWindow::on_action_quit));

    m_paned.set_orientation(Gtk::ORIENTATION_VERTICAL);
    m_grundbokScroll.set_size_request(400, 200);
    m_grundbokScroll.add(m_grundbokView);
    m_paned.add1(m_grundbokScroll);
    m_verifikatScroll.set_size_request(400, 50);
    m_verifikatScroll.add(m_verifikatView);
    m_paned.pack2(m_verifikatScroll, Gtk::AttachOptions::FILL);
    add(m_paned);

    m_grundbokView.setOnSelectionChanged(
        sigc::mem_fun(this, &MainWindow::onGrundbokSelectionChanged));

    m_verifikatView.signalEdited().connect(
        sigc::mem_fun(this, &MainWindow::onVerifikatViewEdited));

    m_verifikatView.signalNextVerifikat().connect([this] {
        this->m_grundbokView.startEditNewVerifikat();
    });

    m_grundbokView.setOnEditedDate(
        sigc::mem_fun(this, &MainWindow::onVerifikatDateEdited));

    m_grundbokView.setOnEditedText(
        sigc::mem_fun(this, &MainWindow::onVerifikatTextEdited));

    m_header.set_title("Untitled");
    m_header.set_show_close_button(true);
    set_titlebar(m_header);
    set_icon_from_file("src/bollapp-gtk/icon.png");
    loadFile("../docs/bok1.bollbok");
    show_all_children();
}

void MainWindow::on_action_new() {
    if (m_dirty && !doSaveDialog()) {
        return;
    }

    int version = 2074;
    std::string firma = "Ny firma";
    std::string orgnummer = "556614-1234";
    int bokforingsar = 2019;
    std::string valuta = "SEK";
    m_doc = std::make_unique<BollDoc>(version, firma, orgnummer, bokforingsar,
                                      valuta, false);
    m_dirty = true;
    m_filename.clear();
    updateDoc();
    m_header.set_title("Untitled");
}

void MainWindow::on_action_open() {
    if (m_dirty && !doSaveDialog()) {
        return;
    }
    Gtk::FileChooserDialog dialog("Please choose a file",
                                  Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);

    // Add response buttons the the dialog:
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Open", Gtk::RESPONSE_OK);

    auto filter_bollbok = Gtk::FileFilter::create();
    filter_bollbok->set_name("Bollbok files");
    filter_bollbok->add_pattern("*.bollbok");
    dialog.add_filter(filter_bollbok);

    // Show the dialog and wait for a user response:
    if (dialog.run() == Gtk::RESPONSE_OK) {
        loadFile(dialog.get_filename());
    }
}

void MainWindow::on_action_save() {
    if (m_filename.empty()) {
        Gtk::FileChooserDialog dialog("Please choose file to save",
                                      Gtk::FILE_CHOOSER_ACTION_SAVE);
        dialog.set_transient_for(*this);

        // Add response buttons the the dialog:
        dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        dialog.add_button("_Save", Gtk::RESPONSE_OK);

        auto filter_bollbok = Gtk::FileFilter::create();
        filter_bollbok->set_name("Bollbok files");
        filter_bollbok->add_pattern("*.bollbok");
        dialog.add_filter(filter_bollbok);

        // Show the dialog and wait for a user response:
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

void MainWindow::on_action_quit() { 
    if (m_dirty && !doSaveDialog()) {
        return;
    }
    hide(); 
}

// Ask the user if he wants to save, and saves if that is the case.
// Returns true on 'Don't Save' or 'Save', false on 'Cancel'
bool MainWindow::doSaveDialog() {
    std::string filename = Utils::getFilenameComponent(m_filename);
    if (filename.empty()) {
        filename = "Untitled document";
    }
    Gtk::MessageDialog msgDialog(
        *this, "Do you want to save the changes to " + filename + "?", false,
        Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE, true);
    msgDialog.set_secondary_text(
        "Your changes will be lost if you don't save them.");

    enum {
        DONT_SAVE = 0,
        CANCEL = 1,
        SAVE = 2
    };

    msgDialog.add_button("Don't Save", DONT_SAVE);
    msgDialog.add_button("Cancel", CANCEL);
    msgDialog.add_button("Save", SAVE);
    int result = msgDialog.run();
    msgDialog.hide();
    if (result == DONT_SAVE) {
        return true;
    } else if (result == CANCEL) {
        return false;
    } else if (result == SAVE) {
        on_action_save();
        return true;
    }
    return false;
}

bool MainWindow::on_delete_event(GdkEventAny* any_event) {
    if (m_dirty && !doSaveDialog()) {
        return true;
    }
    return false;
}

void MainWindow::onGrundbokSelectionChanged() {
    unsigned id;
    Gtk::TreeModel::iterator selected = m_grundbokView.get_selection()->get_selected();
    if (!selected) {
        std::cout << "Nothing selected." << std::endl;
        return;
    }
    selected->get_value(0, id);

    std::cout << "Changing from " << m_verifikatEditingId << " to " << id
              << std::endl;
    setVerifikat(m_verifikatView, m_doc->getVerifikat(id));
    m_verifikatEditingId = id;
}

void MainWindow::onVerifikatViewEdited(const std::vector<BollDoc::Rad>& rader) {
    std::cout << "onVerifikatViewEdited()" << std::endl;
    for (auto& x : rader) {
        std::cout << x << std::endl;
    }
    if (m_verifikatEditingId >= 0) {
        m_doc->updateVerifikat(m_verifikatEditingId, rader);
        m_grundbokView.recalculate(*m_doc);
        m_grundbokView.addNewVerifikatRow(*m_doc);
        m_dirty = true;
    }
}

void MainWindow::onVerifikatDateEdited(unsigned int id, const Date& date) {
    m_doc->setVerifikatTransdatum(id, date);
    m_dirty = true;
    m_grundbokView.startEditText();
}

void MainWindow::onVerifikatTextEdited(unsigned int id, const Glib::ustring& text) {
    m_doc->setVerifikatText(id, text);
    m_dirty = true;

    if (id == m_doc->getVerifikationer().size() - 1) {
        m_verifikatView.startEditing();
    }
}

void MainWindow::loadFile(const std::string& path) {
    std::ifstream ifs(path);
    if (ifs.good()) {
        size_t pos = path.find_last_of('/');
        std::string filename =
            (pos == std::string::npos) ? path : path.substr(pos + 1);
        m_header.set_title(filename);
        set_titlebar(m_header);
        m_doc = std::make_unique<BollDoc>(Serialize::loadDocument(ifs));
        updateDoc();
        m_filename = path;
        m_dirty = false;
    } else {
        std::cout << "Error loading " << path << std::endl;
    }
}

void MainWindow::saveFile(const std::string& path) {
    std::ofstream ofs(path);
    if (ofs.good()) {
        Serialize::saveDocumentCustom(*m_doc, ofs);
        m_dirty = false;
    } else {
        std::cout << "Error saving " << path << std::endl;
    }
}

void MainWindow::updateDoc() {
    if (m_doc) {
        m_grundbokView.updateWithDoc(*m_doc);
        m_grundbokView.addNewVerifikatRow(*m_doc);
        m_verifikatView.clear();
        m_verifikatView.updateKontoLista(m_doc->getKontoPlan());
        m_verifikatEditingId = -1;
    }
}
