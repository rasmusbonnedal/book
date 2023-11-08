#include <iostream>
#include <fstream>

#include "book-app.h"
#include "serialize.h"

int main(int, char**) {
    BookApp app;
    try {
        app.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        std::ofstream ofs("recovery.bollbok");
        Serialize::saveDocumentCustom(app.doc(), ofs);
        std::cerr << "Recovery file saved as recovery.bollbok" << std::endl;
    }

    return 0;
}
