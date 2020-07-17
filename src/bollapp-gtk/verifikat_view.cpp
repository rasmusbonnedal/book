#include "verifikat_view.h"

#include "utils.h"

#include "cellrenderertextcompletion.h"
#include "treeview_enable_edit_next_field.h"

VerifikatView::VerifikatView()
    : Gtk::TreeView(), m_completion(createEntryCompletion()) {
    m_refTreeModel = Gtk::ListStore::create(m_columns);
    set_model(m_refTreeModel);
    m_columns.addColumns(*this);
    enableEditNextField({get_column(0), get_column(1)});
    clear();
}

void VerifikatView::clear() { m_refTreeModel->clear(); }

void VerifikatView::addRow(unsigned konto, const Pengar& pengar,
                           const Date& date, const std::optional<Date>& struken,
                           bool first) {
    Gtk::TreeIter treeIter =
        first ? m_refTreeModel->prepend() : m_refTreeModel->append();
    m_columns.setRow(*treeIter, konto, pengar, date, struken);
}

void VerifikatView::updateKontoLista(
    const std::map<int, BollDoc::Konto>& kontoplan) {
    auto listStore =
        Glib::RefPtr<Gtk::ListStore>::cast_dynamic(m_completion->get_model());
    listStore->clear();
    m_kontoplan.clear();

    for (const auto& it : kontoplan) {
        auto row = *(listStore->append());
        m_completionRecord.setRow(row, std::to_string(it.first),
                                  it.second.getText());
        m_kontoplan[it.first] = it.second.getText();
    }
}

sigc::signal<void, const std::vector<BollDoc::Rad>&>
VerifikatView::signalEdited() {
    return m_signalEdited;
}

void VerifikatView::startEditing() {
    Glib::signal_timeout().connect_once(
        [this]() {
            this->set_cursor(Gtk::TreePath("0"), *get_column(0), true);
        },
        1);
}

void VerifikatView::sendEditedSignal() {
    std::cout << "sendEditedSignal()" << std::endl;
    std::vector<BollDoc::Rad> result;
    for (const auto& row : m_refTreeModel->children()) {
        unsigned konto;
        Pengar pengar;
        Date date;
        std::optional<Date> struken;
        m_columns.getRow(row, konto, pengar, date, struken);
        if (pengar.get() != 0 && konto != 0) {
            result.emplace_back(date, konto, pengar, struken);
        }
    }
    m_signalEdited.emit(result);
}

Glib::RefPtr<Gtk::EntryCompletion> VerifikatView::createEntryCompletion() {
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

// Convert the int konto to a string with kontonummer and text
// Set formatting of the cell
void VerifikatView::onCellDataKontoColumn(
    Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter) {
    Gtk::TreeModel::Row row = *iter;
    Gtk::CellRendererText* crt = dynamic_cast<Gtk::CellRendererText*>(cell);

    bool struken = row[m_columns.m_colStruken];
    crt->property_strikethrough() = struken;
    crt->property_editable() = !struken;

    unsigned int konto = row[m_columns.m_colKonto];
    if (konto == 0) {
        crt->property_foreground_rgba() = Gdk::RGBA("#bbb");
        crt->property_text() = "Ny kontering";
        return;
    } else {
        crt->property_foreground_rgba() = Gdk::RGBA("#000");
    }

    auto it = m_kontoplan.find(konto);
    std::string kontoText = "(Ogiltigt konto)";
    if (it != m_kontoplan.end()) {
        kontoText = it->second;
    }
    crt->property_text() = std::to_string(konto) + "     " + kontoText;
}

void VerifikatView::onCellDataPengarColumn(
    Gtk::CellRenderer* cell, const Gtk::TreeModel::iterator& iter) {
    Gtk::TreeModel::Row row = *iter;
    Gtk::CellRendererText* crt = dynamic_cast<Gtk::CellRendererText*>(cell);
    bool struken = row[m_columns.m_colStruken];
    crt->property_strikethrough() = struken;
    crt->property_editable() = !struken;

    unsigned int konto = row[m_columns.m_colKonto];
    if (konto == 0) {
        crt->property_foreground_rgba() = Gdk::RGBA("#bbb");
    } else {
        crt->property_foreground_rgba() = Gdk::RGBA("#000");
    }

    Pengar pengar(row[m_columns.m_colPengar]);
    crt->property_text() = toString2(pengar) + " kr";
}

void VerifikatView::onEditedKonto(const Glib::ustring& path_string,
                                  const Glib::ustring& new_text) {
    std::cout << "VerifikatView::onEdited(" << path_string << ")" << std::endl;
    int new_value = atoi(new_text.c_str());
    if (new_value && m_kontoplan.find(new_value) != m_kontoplan.end()) {
        Gtk::TreePath path(path_string);
        Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            row[m_columns.m_colKonto] = new_value;
            sendEditedSignal();
        }
    }
}

void VerifikatView::onEditingStartedPengar(Gtk::CellEditable* editable,
                                           const Glib::ustring& path) {
    Gtk::Entry* entry = dynamic_cast<Gtk::Entry*>(editable);
    if (entry) {
        std::string s = entry->get_text();

        auto row = m_refTreeModel->get_iter(path);
        if (m_columns.getPengar(*row) == 0) {
            Pengar balance = sumVerifikat();
            s = toString2(-balance);
        }

        // Remove spaces and "kr" from the pengar string
        size_t s_len = s.size();
        if (s_len > 2 && s.substr(s_len - 2) == "kr") {
            s = s.substr(0, s_len - 2);
        }
        entry->set_text(Utils::removeSpaces(s));
    }
}

void VerifikatView::onEditedPengar(const Glib::ustring& path_string,
                                   const Glib::ustring& new_text) {
    std::cout << "VerifikatView::onEditedPengar(" << path_string << ")"
              << std::endl;
    Gtk::TreePath path(path_string);
    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        unsigned konto = row[m_columns.m_colKonto];
        try {
            Pengar p = parsePengar(new_text);
            row[m_columns.m_colPengar] = p;
            if (path_string == "0" && konto != 0 && p.get() != 0) {
                addRow(0, 0, Date(), std::nullopt, true);
            }
            sendEditedSignal();
        } catch (std::exception&) {
        }
    }
}

Pengar VerifikatView::sumVerifikat() const {
    Pengar sum(0);
    for (auto row : m_refTreeModel->children()) {
        unsigned int konto;
        Pengar pengar;
        Date date;
        std::optional<Date> struken;
        m_columns.getRow(row, konto, pengar, date, struken);
        if (!struken) {
            sum += pengar;
        }
    }
    return sum;
}

//
// Class CompletionRecord
//
VerifikatView::CompletionRecord::CompletionRecord() {
    add(m_colText);
    add(m_colText2);
}

void VerifikatView::CompletionRecord::setRow(Gtk::TreeRow& row,
                                             const std::string& s,
                                             const std::string& s2) {
    row[m_colText] = s;
    row[m_colText2] = s2;
}

//
// Class ModelColumns
//
VerifikatView::ModelColumns::ModelColumns() {
    add(m_colKonto);
    add(m_colPengar);
    add(m_colDate);
    add(m_colStruken);
    add(m_colStrukenDate);
}

void VerifikatView::ModelColumns::addColumns(VerifikatView& treeView) {
    auto& cell = treeView.m_kontoCellRenderer;
    cell.property_editable() = true;
    cell.signal_edited().connect(
        sigc::mem_fun(&treeView, &VerifikatView::onEditedKonto));
    makeCellRendererUseCompletion(cell, treeView.m_completion);
    treeView.append_column("Konto", cell);
    Gtk::TreeViewColumn* column = treeView.get_column(0);
    column->set_cell_data_func(
        cell, sigc::mem_fun(&treeView, &VerifikatView::onCellDataKontoColumn));

    auto& pengarCell = treeView.m_pengarCellRenderer;
    treeView.append_column("Debet / Kredit", pengarCell);
    pengarCell.signal_edited().connect(
        sigc::mem_fun(&treeView, &VerifikatView::onEditedPengar));
    // This callback removes spaces and "kr" from pengar string
    // "1 000 kr" -> "1000"
    pengarCell.signal_editing_started().connect(
        sigc::mem_fun(&treeView, &VerifikatView::onEditingStartedPengar));
    column = treeView.get_column(1);
    column->set_cell_data_func(
        pengarCell,
        sigc::mem_fun(&treeView, &VerifikatView::onCellDataPengarColumn));
}

void VerifikatView::ModelColumns::setRow(
    const Gtk::TreeRow& row, unsigned konto, Pengar pengar, const Date& date,
    const std::optional<Date>& struken) const {
    row[m_colKonto] = konto;
    row[m_colPengar] = pengar;
    row[m_colDate] = date;
    row[m_colStruken] = struken.has_value();
    if (struken.has_value()) {
        row[m_colStrukenDate] = *struken;
    }
}

void VerifikatView::ModelColumns::getRow(const Gtk::TreeRow& row,
                                         unsigned& konto, Pengar& pengar,
                                         Date& date,
                                         std::optional<Date>& struken) const {
    konto = row[m_colKonto];
    pengar = row[m_colPengar];
    date = row[m_colDate];
    if (row[m_colStruken]) {
        struken = row[m_colStrukenDate];
    } else {
        struken.reset();
    }
}

Pengar VerifikatView::ModelColumns::getPengar(const Gtk::TreeRow& row) const {
    return row[m_colPengar];
}
