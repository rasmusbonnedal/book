#include <iostream>

#include "bolldoc.h"

class Serialize {
public:
    static BollDoc loadDocument(std::istream& input);
    static void saveDocument(const BollDoc& doc, std::ostream& output);
};
