#include "gmock/gmock.h"
#include "../../mock.hpp"

#include <utility>

#include "../../driver/MockDriver.hpp"
#include "../aggregator/MockAggregatorRegistry.hpp"
#include "../aggregator/MockSubtaskAggregator.hpp"
#include "../aggregator/MockTestCaseAggregator.hpp"
#include "../evaluator/MockEvaluator.hpp"
#include "../evaluator/MockEvaluatorRegistry.hpp"
#include "../generator/MockGenerator.hpp"
#include "../grader/MockGrader.hpp"
#include "../grader/MockGraderLogger.hpp"
#include "../grader/MockGraderLoggerFactory.hpp"
#include "../logger/MockRunnerLogger.hpp"
#include "../os/MockOperatingSystem.hpp"
#include "tcframe/runner/core/Runner.hpp"

using ::testing::_;
using ::testing::Eq;
using ::testing::Ne;
using ::testing::Property;
using ::testing::Return;
using ::testing::Test;
using ::testing::Throw;
using ::testing::Truly;

using std::move;

namespace tcframe {

class BaseRunnerTests : public Test {
protected:
    class ProblemSpec : public BaseProblemSpec {
    protected:
        void InputFormat() {}
    };

    string specPath = "/Users/fushar/january contest/c_slug/spec.cpp";

    int argc = 1;
    char** argv =  new char*[1]{(char*) "./runner"};

    LoggerEngine* loggerEngine = new SimpleLoggerEngine();

    MOCK(RunnerLogger) runnerLogger;
    MOCK(GraderLogger) graderLogger;
    MOCK(Evaluator) evaluator;
    MOCK(TestCaseAggregator) testCaseAggregator;
    MOCK(SubtaskAggregator) subtaskAggregator;
    MOCK(Generator) generator;
    MOCK(Grader) grader;

    MOCK(OperatingSystem) os;
    MOCK(RunnerLoggerFactory) runnerLoggerFactory;
    MOCK(GraderLoggerFactory) graderLoggerFactory;
    MOCK(GeneratorFactory) generatorFactory;
    MOCK(GraderFactory) graderFactory;
    MOCK(EvaluatorRegistry) evaluatorRegistry;
    MOCK(AggregatorRegistry) aggregatorRegistry;

    void SetUp() {
        ON_CALL(runnerLoggerFactory, create(_)).WillByDefault(Return(&runnerLogger));
        ON_CALL(graderLoggerFactory, create(_, _)).WillByDefault(Return(&graderLogger));
        ON_CALL(generatorFactory, create(_, _, _, _)).WillByDefault(Return(&generator));
        ON_CALL(graderFactory, create(_, _, _, _, _)).WillByDefault(Return(&grader));
        ON_CALL(evaluatorRegistry, get(_, _, _)).WillByDefault(Return(&evaluator));
        ON_CALL(aggregatorRegistry, getTestCaseAggregator(_)).WillByDefault(Return(&testCaseAggregator));
        ON_CALL(aggregatorRegistry, getSubtaskAggregator()).WillByDefault(Return(&subtaskAggregator));
        ON_CALL(os, execute(_)).WillByDefault(Return(ExecutionResult()));
    }

    template<typename TProblemSpec>
    Runner<TProblemSpec> createRunner(BaseTestSpec<TProblemSpec>* testSpec) {
        auto driver = new Driver<TProblemSpec>(specPath, testSpec);
        return {driver, loggerEngine, &os, &runnerLoggerFactory, &graderLoggerFactory,
                &generatorFactory, &graderFactory, &evaluatorRegistry, &aggregatorRegistry};
    }

    struct HelperKeyIs {
        string key_;
        string value_;

        HelperKeyIs(string key, string value)
                : key_(move(key))
                , value_(move(value)) {}

        bool operator()(const map<string, string>& m) const {
            if (value_.empty()) {
                return !m.count(key_);
            }
            return m.count(key_) && m.at(key_) == value_;
        }
    };
};

}
