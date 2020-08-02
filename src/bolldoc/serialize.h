#pragma once

#include <iostream>

#include "bolldoc.h"

class Serialize {
public:
    static BollDoc loadDocument(std::istream& input, bool ignoreChecksum = false);
    static void saveDocument(const BollDoc& doc, std::ostream& output);
    static void saveDocumentCustom(const BollDoc& doc, std::ostream& output);
};
