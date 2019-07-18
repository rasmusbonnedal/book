#include <iostream>

#include "bolldoc.h"

class Serialize {
public:
    static BollDoc loadDocument(std::istream& input);
};
