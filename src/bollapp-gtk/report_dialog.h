#pragma once

#include "date.h"
#include "report.h"

#include <gtkmm.h>

#include <string>
#include <vector>

struct ReportDetails {
    DateType dateRange;
    ReportType reportType;
};

class ReportDialog {
public:
    static bool doReportDialog(Gtk::Window& parent, int year,
                               ReportDetails& reportDetails);
};
