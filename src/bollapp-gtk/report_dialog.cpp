#include "report_dialog.h"

#include <gtkmm.h>

bool ReportDialog::doReportDialog(Gtk::Window& parent, int year,
                                  ReportDetails& reportDetails) {
    Gtk::Dialog dialog("Rapporter", parent,
                       GTK_DIALOG_MODAL | GTK_DIALOG_USE_HEADER_BAR);
    Gtk::Grid grid;
    grid.set_halign(Gtk::ALIGN_FILL);
    const int spacing = 8;
    grid.set_column_spacing(spacing);
    grid.set_row_spacing(spacing);
    grid.property_margin() = spacing;

    Gtk::Label labelRapport("Rapport:");
    labelRapport.set_xalign(0.0);
    Gtk::ComboBoxText reportType;
    for (int i = 0; i < REPORT_TYPE_COUNT; ++i) {
        reportType.append(reportTypeString((ReportType)i));
    }
    reportType.set_active(reportDetails.reportType);

    Gtk::Label labelDatum("Datum:");
    labelDatum.set_xalign(0.0);
    Gtk::ComboBoxText datumRange;
    for (int i = 0; i < DATETYPE_COUNT; ++i) {
        datumRange.append(dateTypeToString((DateType)i));
    }
    datumRange.set_active(reportDetails.dateRange);

    grid.attach(labelRapport, 0, 0);
    grid.attach(reportType, 1, 0);
    grid.attach(labelDatum, 0, 1);
    grid.attach(datumRange, 1, 1);

    dialog.get_content_area()->pack_start(grid);
    dialog.get_content_area()->show_all();
    dialog.add_button("OK", 0)->set_can_default();
    dialog.add_button("Cancel", 1);
    dialog.set_default_response(0);
    if (dialog.run() == 0) {
        reportDetails.reportType = (ReportType)reportType.get_active_row_number();
        reportDetails.dateRange = (DateType)datumRange.get_active_row_number();
        return true;
    } else {
        return false;
    }
}
