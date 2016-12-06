#pragma once

#include <vector>

#include "GradingConfig.hpp"
#include "MultipleTestCasesConfig.hpp"
#include "StyleConfig.hpp"
#include "tcframe/spec/constraint.hpp"
#include "tcframe/spec/io.hpp"
#include "tcframe/util.hpp"

using std::vector;

namespace tcframe {

class BaseProblemSpec
        : protected MultipleTestCasesConfigBuilder,
          protected GradingConfigBuilder,
          protected StyleConfigBuilder,
          protected IOFormatBuilder,
          protected ConstraintSuiteBuilder {
private:
    vector<void(BaseProblemSpec::*)()> subtasks_ = {
            &BaseProblemSpec::Subtask1,
            &BaseProblemSpec::Subtask2,
            &BaseProblemSpec::Subtask3,
            &BaseProblemSpec::Subtask4,
            &BaseProblemSpec::Subtask5,
            &BaseProblemSpec::Subtask6,
            &BaseProblemSpec::Subtask7,
            &BaseProblemSpec::Subtask8,
            &BaseProblemSpec::Subtask9,
            &BaseProblemSpec::Subtask10,
            &BaseProblemSpec::Subtask11,
            &BaseProblemSpec::Subtask12,
            &BaseProblemSpec::Subtask13,
            &BaseProblemSpec::Subtask14,
            &BaseProblemSpec::Subtask15,
            &BaseProblemSpec::Subtask16,
            &BaseProblemSpec::Subtask17,
            &BaseProblemSpec::Subtask18,
            &BaseProblemSpec::Subtask19,
            &BaseProblemSpec::Subtask20,
            &BaseProblemSpec::Subtask21,
            &BaseProblemSpec::Subtask22,
            &BaseProblemSpec::Subtask23,
            &BaseProblemSpec::Subtask24,
            &BaseProblemSpec::Subtask25};

public:
    virtual ~BaseProblemSpec() {}

    tcframe::StyleConfig buildStyleConfig() {
        StyleConfig();
        return StyleConfigBuilder::build();
    }

    tcframe::MultipleTestCasesConfig buildMultipleTestCasesConfig() {
        MultipleTestCasesConfig();
        return MultipleTestCasesConfigBuilder::build();
    }

    tcframe::GradingConfig buildGradingConfig() {
        GradingConfig();
        return GradingConfigBuilder::build();
    }

    IOFormat buildIOFormat() {
        IOFormatBuilder::prepareForInputFormat();
        InputFormat();
        IOFormatBuilder::prepareForOutputFormat();
        OutputFormat();
        return IOFormatBuilder::build();
    }

    ConstraintSuite buildConstraintSuite() {
        Constraints();
        for (auto subtask : subtasks_) {
            try {
                ConstraintSuiteBuilder::newSubtask();
                (this->*subtask)();
            } catch (NotImplementedException&) {
                break;
            }
        }

        ConstraintSuiteBuilder::prepareForMultipleTestCasesConstraints();
        MultipleTestCasesConstraints();
        return ConstraintSuiteBuilder::build();
    }

protected:
    virtual void InputFormat() = 0;
    virtual void OutputFormat() {}
    virtual void StyleConfig() {}
    virtual void GradingConfig() {}
    virtual void MultipleTestCasesConfig() {}
    virtual void MultipleTestCasesConstraints() {}
    virtual void Constraints() {}
    virtual void Subtask1() {throw NotImplementedException();}
    virtual void Subtask2() {throw NotImplementedException();}
    virtual void Subtask3() {throw NotImplementedException();}
    virtual void Subtask4() {throw NotImplementedException();}
    virtual void Subtask5() {throw NotImplementedException();}
    virtual void Subtask6() {throw NotImplementedException();}
    virtual void Subtask7() {throw NotImplementedException();}
    virtual void Subtask8() {throw NotImplementedException();}
    virtual void Subtask9() {throw NotImplementedException();}
    virtual void Subtask10() {throw NotImplementedException();}
    virtual void Subtask11() {throw NotImplementedException();}
    virtual void Subtask12() {throw NotImplementedException();}
    virtual void Subtask13() {throw NotImplementedException();}
    virtual void Subtask14() {throw NotImplementedException();}
    virtual void Subtask15() {throw NotImplementedException();}
    virtual void Subtask16() {throw NotImplementedException();}
    virtual void Subtask17() {throw NotImplementedException();}
    virtual void Subtask18() {throw NotImplementedException();}
    virtual void Subtask19() {throw NotImplementedException();}
    virtual void Subtask20() {throw NotImplementedException();}
    virtual void Subtask21() {throw NotImplementedException();}
    virtual void Subtask22() {throw NotImplementedException();}
    virtual void Subtask23() {throw NotImplementedException();}
    virtual void Subtask24() {throw NotImplementedException();}
    virtual void Subtask25() {throw NotImplementedException();}
};

}
