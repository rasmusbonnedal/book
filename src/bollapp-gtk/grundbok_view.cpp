#include "grundbok_view.h"

GrundbokView::GrundbokView() : Gtk::TreeView() {
    m_refTreeModel = Gtk::ListStore::create(m_columns);
    m_sortedModel = Gtk::TreeModelSort::create(m_refTreeModel);
    m_sortedModel->set_sort_column(1, Gtk::SORT_DESCENDING);
    m_sortedModel->set_sort_func(1, sigc::mem_fun(this, &GrundbokView::sortFunction));
    set_model(m_sortedModel);
    m_columns.addColumns(*this);
}

void GrundbokView::setOnSelectionChanged(const Glib::SignalProxy<void>::SlotType& onSelectionChanged) {
    m_selectionChangedConnection = get_selection()->signal_changed().connect(onSelectionChanged);
}

void GrundbokView::setOnEditedDate(const Glib::SignalProxy<void, unsigned int, const Date&>::SlotType& onEditedDate) {
    m_signalDateEdited.connect(onEditedDate);
}

void GrundbokView::setOnEditedText(const Glib::SignalProxy<void, unsigned int, const Glib::ustring&>::SlotType& onEditedText) {
    m_signalTextEdited.connect(onEditedText);
}

void GrundbokView::recalculate(const BollDoc& doc) {
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
            omslutningStr << " - obalanserad (" << omslutning << ") - ";
        }
        m_columns.setRow(treeRow, row.getUnid(), date.str(), row.getText(),
                            omslutningStr.str());
    }
}

void GrundbokView::updateWithDoc(const BollDoc& doc) {
    m_selectionChangedConnection.block();
    get_selection()->unselect_all();
    m_refTreeModel->clear();

    for (const auto& row : doc.getVerifikationer()) {
        auto treeRow = *(m_refTreeModel->append());
        Pengar omslutning;
        std::stringstream omslutningStr;
        if (row.getOmslutning(omslutning)) {
            omslutningStr << omslutning << " kr";
        } else {
            omslutningStr << " - obalanserad (" << omslutning << ") - ";
        }
        m_columns.setRow(treeRow, row.getUnid(), to_string(row.getTransdatum()),
                            row.getText(), omslutningStr.str());
    }
    m_selectionChangedConnection.unblock();
    m_year = doc.getBokforingsar();
}

void GrundbokView::addNewVerifikatRow(BollDoc& doc) {
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

void GrundbokView::onEditedText(const Glib::ustring& path_string,
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

void GrundbokView::onEditedDate(const Glib::ustring& path_string,
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

int GrundbokView::sortFunction(const Gtk::TreeModel::iterator& lhs, const Gtk::TreeModel::iterator& rhs) {
    std::string lhsDate = m_columns.getDate(*lhs);
    std::string rhsDate = m_columns.getDate(*rhs);
    if (lhsDate == rhsDate) {
        return m_columns.getId(*lhs) - m_columns.getId(*rhs);
    }
    return lhsDate.compare(rhsDate);
}

//
// Class ModelColumns
//
GrundbokView::ModelColumns::ModelColumns() {
    add(m_colId);
    add(m_colDate);
    add(m_colText);
    add(m_colOmslutning);
}

void GrundbokView::ModelColumns::addColumns(GrundbokView& treeView) {
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

void GrundbokView::ModelColumns::setRow(const Gtk::TreeRow& row, unsigned id, const std::string& date,
            const std::string& text, const std::string& omslutning) {
    row[m_colId] = id;
    row[m_colDate] = date;
    row[m_colText] = text;
    row[m_colOmslutning] = omslutning;
}

unsigned GrundbokView::ModelColumns::getId(const Gtk::TreeRow& row) const {
    return row[m_colId];
}

std::string GrundbokView::ModelColumns::getDate(const Gtk::TreeRow& row) const {
    return Glib::ustring(row[m_colDate]);         
}

void GrundbokView::ModelColumns::setDate(const Gtk::TreeRow& row, const std::string& date) {
    row[m_colDate] = date;
}

std::string GrundbokView::ModelColumns::getText(const Gtk::TreeRow& row) const {
    return Glib::ustring(row[m_colText]);
}

void GrundbokView::ModelColumns::setText(const Gtk::TreeRow& row, const std::string& text) {
    row[m_colText] = text;
}

std::string GrundbokView::ModelColumns::getOmslutning(const Gtk::TreeRow& row) const {
    return Glib::ustring(row[m_colOmslutning]);
}
