#pragma once

#include "tcframe/generator/TestCaseException.hpp"

#include <string>

using std::string;

namespace tcframe {

class PrintingException : public TestCaseException {
public:
    PrintingException(string message)
            : TestCaseException(message) { }
};

}