#pragma once

#include "tcframe/generator/TestCaseException.hpp"

#include <string>

using std::string;

namespace tcframe {
    
class ParsingException : public TestCaseException {
public:
    ParsingException(string message)
            : TestCaseException(message) { }
};

}
