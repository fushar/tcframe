#pragma once

#include "tcframe/logger/Logger.hpp"

#include <string>

using std::string;

namespace tcframe {

class GeneratorLogger : public Logger {
public:
    virtual ~GeneratorLogger() { }

    virtual void logIntroduction() = 0;
    virtual void logTestCaseOkResult() = 0;
    virtual void logTestCaseFailedResult(string testCaseDescription) = 0;
    virtual void logMultipleTestCasesCombinationIntroduction(string testCaseBaseName) = 0;
    virtual void logMultipleTestCasesCombinationOkResult() = 0;
    virtual void logMultipleTestCasesCombinationFailedResult() = 0;
};

}
