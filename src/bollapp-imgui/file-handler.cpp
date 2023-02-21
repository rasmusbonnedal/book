#include "file-handler.h"

#include <nfd.h>

#include <iostream>
#include <malloc.h>

void FileHandler::open() {
    nfdchar_t* out_path = NULL;
    nfdresult_t result = NFD_OpenDialog("bollbok", NULL, &out_path);

    if (result == NFD_OKAY) {
        // filename = out_path;
        std::cout << out_path << std::endl;
        free(out_path);
    } else if (result == NFD_CANCEL) {
    } else {
        std::cout << "Error: " << NFD_GetError() << std::endl;;
    }
}
