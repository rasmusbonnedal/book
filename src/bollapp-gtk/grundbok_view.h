#pragma once

#include "bolldoc.h"
#include "date.h"

#include <gtkmm.h>

#include <string>

class GrundbokView : public Gtk::TreeView {
public:
    GrundbokView();

    void setOnSelectionChanged(const Glib::SignalProxy<void>::SlotType& onSelectionChanged);

    void setOnEditedDate(const Glib::SignalProxy<void, unsigned int, const Date&>::SlotType& onEditedDate);

    void setOnEditedText(const Glib::SignalProxy<void, unsigned int, const Glib::ustring&>::SlotType& onEditedText);

    void recalculate(const BollDoc& doc);

    void updateWithDoc(const BollDoc& doc);

    void addNewVerifikatRow(BollDoc& doc);

    int sortFunction(const Gtk::TreeModel::iterator& lhs, const Gtk::TreeModel::iterator& rhs);

private:
    void onEditedText(const Glib::ustring& path_string,
                      const Glib::ustring& new_text);

    void onEditedDate(const Glib::ustring& path_string,
                      const Glib::ustring& new_text);

    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns();

        void addColumns(GrundbokView& treeView);

        void setRow(const Gtk::TreeRow& row, unsigned id, const std::string& date,
                    const std::string& text, const std::string& omslutning);

        unsigned getId(const Gtk::TreeRow& row) const;

        std::string getDate(const Gtk::TreeRow& row) const;

        void setDate(const Gtk::TreeRow& row, const std::string& date);

        std::string getText(const Gtk::TreeRow& row) const;

        void setText(const Gtk::TreeRow& row, const std::string& text);

        std::string getOmslutning(const Gtk::TreeRow& row) const;

    private:
        Gtk::TreeModelColumn<unsigned> m_colId;
        Gtk::TreeModelColumn<Glib::ustring> m_colDate;
        Gtk::TreeModelColumn<Glib::ustring> m_colText;
        Gtk::TreeModelColumn<Glib::ustring> m_colOmslutning;
    };

    ModelColumns m_columns;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
    Glib::RefPtr<Gtk::TreeModelSort> m_sortedModel;
    sigc::signal<void, unsigned int, const Glib::ustring&> m_signalTextEdited;
    sigc::signal<void, unsigned int, const Date&> m_signalDateEdited;
    int m_year;
};
