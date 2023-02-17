#include "saldo_view.h"

#include "timer.h"

// SaldoView

SaldoView::SaldoView() {
    m_model = Gtk::ListStore::create(m_columns.getRecord());
    m_treeView.set_model(m_model);
    m_treeView.append_column("Datum", m_columns.m_colDate);
    m_treeView.append_column("Text", m_columns.m_colText);
    m_treeView.get_column(1)->set_expand(true);
    m_treeView.append_column("Debet / Kredit", m_columns.m_colPengar);
    m_treeView.get_column_cell_renderer(2)->property_xalign() = 1.0;
    m_treeView.append_column("Saldo", m_columns.m_colSaldo);
    m_treeView.get_column_cell_renderer(3)->property_xalign() = 1.0;
}

Gtk::TreeView& SaldoView::getWidget() { return m_treeView; }

void SaldoView::showKonto(const BollDoc& doc, int konto, const Date& lastDate, int verifikatId) {
    Stopwatch t1;
    t1.start();
    m_treeView.unset_model();
    m_model->freeze_notify();
    clear();
    auto verifikat = doc.getVerifikatRange(Date(0, 1, 1), lastDate.addDays(1));
    std::sort(verifikat.begin(), verifikat.end(),
              [](auto& lhs, auto& rhs) -> bool {
                  if (lhs->getTransdatum() == rhs->getTransdatum()) {
                      return lhs->getUnid() < rhs->getUnid();
                  }
                  return lhs->getTransdatum() < rhs->getTransdatum();
              });
    Pengar sum = {0};
    float lap = t1.lap();
    //auto row = *(m_model->append());
    for (const auto& v : verifikat) {
        // Skip later verifikats with same date
        if (v->getTransdatum() == lastDate && v->getUnid() > verifikatId) {
            continue;
        }
        for (const auto& r : v->getRader()) {
            if (r.getKonto() == konto && !r.getStruken()) {
                auto row = *(m_model->append());
                sum += r.getPengar();
                const Date& d = v->getTransdatum();
                row[m_columns.m_colDate] =
                    d.getYear() ? to_string(v->getTransdatum()) : "";
                row[m_columns.m_colText] = v->getText();
                row[m_columns.m_colPengar] = toString2(r.getPengar()) + " kr";
                row[m_columns.m_colSaldo] = toString2(sum) + " kr";
            }
        }
    }
    //row[m_columns.m_colText] = "asdf";
    m_model->thaw_notify();
    m_treeView.set_model(m_model);
    float t = t1.stop();
    std::cout << "showKonto(" << konto << ") " << (t * 1000) << " ms, first: " << (lap * 1000) << " ms" << std::endl;
}

void SaldoView::clear() { m_model->clear(); }

// SaldoColumns

SaldoColumns::SaldoColumns() {
    m_columnRecord.add(m_colDate);
    m_columnRecord.add(m_colText);
    m_columnRecord.add(m_colPengar);
    m_columnRecord.add(m_colSaldo);
}

Gtk::TreeModel::ColumnRecord& SaldoColumns::getRecord() {
    return m_columnRecord;
}
