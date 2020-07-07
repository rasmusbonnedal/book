#include "bollbok_app.h"

int main(int argc, char** argv) {
    auto app = BollBokApp::create();
    return app->run(argc, argv);
}
