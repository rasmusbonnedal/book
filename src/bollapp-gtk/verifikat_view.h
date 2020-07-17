#pragma once

#include "bolldoc.h"
#include "date.h"
#include "pengar.h"

#include <gtkmm.h>

#include <map>
#include <string>
#include <vector>

class VerifikatView : public Gtk::TreeView {
public:
    VerifikatView();
    void clear();
    void addRow(unsigned konto, const Pengar& pengar, const Date& date,
                const std::optional<Date>& struken, bool first = false);
    void updateKontoLista(const std::map<int, BollDoc::Konto>& kontoplan);
    sigc::signal<void, const std::vector<BollDoc::Rad>&> signalEdited();

private:
    void sendEditedSignal();
    Glib::RefPtr<Gtk::EntryCompletion> createEntryCompletion();

    // Convert the int konto to a string with kontonummer and text
    // Set formatting of the cell
    void onCellDataKontoColumn(Gtk::CellRenderer* cell,
                               const Gtk::TreeModel::iterator& iter);
    void onCellDataPengarColumn(Gtk::CellRenderer* cell,
                                const Gtk::TreeModel::iterator& iter);
    void onEditedKonto(const Glib::ustring& path_string,
                       const Glib::ustring& new_text);
    void onEditingStartedPengar(Gtk::CellEditable* editable,
                                const Glib::ustring& path);
    void onEditedPengar(const Glib::ustring& path_string,
                        const Glib::ustring& new_text);

    Pengar sumVerifikat() const;

    class CompletionRecord : public Gtk::TreeModel::ColumnRecord {
    public:
        CompletionRecord();
        void setRow(Gtk::TreeRow& row, const std::string& s,
                    const std::string& s2);
        Gtk::TreeModelColumn<Glib::ustring> m_colText;
        Gtk::TreeModelColumn<Glib::ustring> m_colText2;
    };
    CompletionRecord m_completionRecord;
    Gtk::CellRendererText m_completionCellRenderer;

    // Model for a row of a Verifikat
    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns();
        void addColumns(VerifikatView& treeView);
        void setRow(const Gtk::TreeRow& row, unsigned konto, Pengar pengar,
                    const Date& date, const std::optional<Date>& struken) const;
        void getRow(const Gtk::TreeRow& row, unsigned& konto, Pengar& pengar,
                    Date& date, std::optional<Date>& struken) const;
        Pengar getPengar(const Gtk::TreeRow& row) const;

        Gtk::TreeModelColumn<unsigned> m_colKonto;
        Gtk::TreeModelColumn<Pengar> m_colPengar;
        Gtk::TreeModelColumn<Date> m_colDate;
        Gtk::TreeModelColumn<bool> m_colStruken;
        Gtk::TreeModelColumn<Date> m_colStrukenDate;
    };

    ModelColumns m_columns;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
    Glib::RefPtr<Gtk::EntryCompletion> m_completion;
    Gtk::CellRendererText m_kontoCellRenderer;
    Gtk::CellRendererText m_pengarCellRenderer;
    std::map<unsigned int, std::string> m_kontoplan;
    sigc::signal<void, const std::vector<BollDoc::Rad>&> m_signalEdited;
};
