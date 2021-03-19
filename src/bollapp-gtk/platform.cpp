#include "platform.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <gtkmm.h>
#endif

void openUri(const std::string& uri) {
#if defined(_WIN32)
    ShellExecute(NULL, "open", uri.c_str(), NULL, NULL, SW_SHOWNORMAL);
#else
    gtk_show_uri_on_window(0, uri.c_str(), 0, 0);
#endif
}
