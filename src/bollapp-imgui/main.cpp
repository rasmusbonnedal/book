#include <iostream>

#include "book-app.h"

int main(int, char**) {
    try {
        BookApp app;
        app.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
