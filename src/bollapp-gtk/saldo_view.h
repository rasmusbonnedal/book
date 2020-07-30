#pragma once

#include "bolldoc.h"
#include "date.h"

#include <gtkmm.h>

class SaldoColumns {
public:
    SaldoColumns();

    Gtk::TreeModel::ColumnRecord& getRecord();

    Gtk::TreeModelColumn<Glib::ustring> m_colDate;
    Gtk::TreeModelColumn<Glib::ustring> m_colText;
    Gtk::TreeModelColumn<Glib::ustring> m_colPengar;
    Gtk::TreeModelColumn<Glib::ustring> m_colSaldo;

private:
    Gtk::TreeModel::ColumnRecord m_columnRecord;
};

class SaldoView {
public:
    SaldoView();

    Gtk::TreeView& getWidget();

    void showKonto(const BollDoc& doc, int konto, const Date& lastDate, int verifikatId);

    void clear();

private:
    Gtk::TreeView m_treeView;
    SaldoColumns m_columns;
    Glib::RefPtr<Gtk::ListStore> m_model;
};
