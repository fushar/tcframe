#pragma once

#include <string>
#include <tuple>
#include <utility>

#include "tcframe/util.hpp"

using std::move;
using std::string;
using std::tie;

namespace tcframe {

struct MultipleTestCasesConfig {
    friend class MultipleTestCasesConfigBuilder;

public:
    static constexpr bool DEFAULT_HAS_EMPTY_LINE_BEFORE_INPUTS = false;
    static constexpr bool DEFAULT_HAS_EMPTY_LINE_BETWEEN_OUTPUTS = false;

private:
    optional<int*> counter_;
    optional<string> outputPrefix_;
    bool hasEmptyLineBeforeInputs_;
    bool hasEmptyLineBetweenOutputs_;

public:
    const optional<int*>& counter() const {
        return counter_;
    }

    const optional<string>& outputPrefix() const {
        return outputPrefix_;
    }

    const bool hasEmptyLineBeforeInputs() const {
        return hasEmptyLineBeforeInputs_;
    }

    const bool hasEmptyLineBetweenOutputs() const {
        return hasEmptyLineBetweenOutputs_;
    }

    bool operator==(const MultipleTestCasesConfig& o) const {
        return tie(counter_, outputPrefix_, hasEmptyLineBeforeInputs_, hasEmptyLineBetweenOutputs_) == tie(o.counter_, o.outputPrefix_, hasEmptyLineBeforeInputs_, hasE);
    }
};

class MultipleTestCasesConfigBuilder {
private:
    MultipleTestCasesConfig subject_;

public:
    MultipleTestCasesConfigBuilder() {
        subject_.hasEmptyLineBeforeInputs_ = MultipleTestCasesConfig::DEFAULT_HAS_EMPTY_LINE_BEFORE_INPUTS;
        subject_.hasEmptyLineBetweenOutputs_ = MultipleTestCasesConfig::DEFAULT_HAS_EMPTY_LINE_BETWEEN_OUTPUTS;
    }

    MultipleTestCasesConfigBuilder& Counter(int& var) {
        subject_.counter_ = optional<int*>(&var);
        return *this;
    }

    MultipleTestCasesConfigBuilder& OutputPrefix(string outputPrefix) {
        subject_.outputPrefix_ = optional<string>(outputPrefix);
        return *this;
    }

    MultipleTestCasesConfigBuilder& EmptyLineBeforeInputs() {
        subject_.hasEmptyLineBeforeInputs_ = true;
        return *this;
    }

    MultipleTestCasesConfigBuilder& EmptyLineBetweenOutputs() {
        subject_.hasEmptyLineBetweenOutputs_ = true;
        return *this;
    }

    MultipleTestCasesConfig build() {
        return move(subject_);
    }
};

}
