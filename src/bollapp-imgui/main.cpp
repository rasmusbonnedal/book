#include <iostream>
#include <fstream>

#include "book-app.h"
#include "serialize.h"

int main(int, char**) {
    std::unique_ptr<BookApp> app;
    try {
        app = std::make_unique<BookApp>();
        app->run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        if (app) {
            std::ofstream ofs("recovery.bollbok");
            Serialize::saveDocumentCustom(app->doc(), ofs);
            std::cerr << "Recovery file saved as recovery.bollbok" << std::endl;
        }
    }
    return 0;
}
