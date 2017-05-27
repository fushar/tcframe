#include "gmock/gmock.h"
#include "../mock.hpp"

#include "BaseRunnerTests.hpp"

namespace tcframe {

class RunnerTests : public BaseRunnerTests {
public:
    static int T;

protected:
    class ProblemSpecWithConfig : public ProblemSpec {
    protected:
        void StyleConfig() {
            NoOutput();
        }

        void MultipleTestCasesConfig() {
            Counter(T);
        }

        void GradingConfig() {
            TimeLimit(3);
            MemoryLimit(128);
        }
    };

    class ProblemSpecWithSubtasks : public ProblemSpec {
    protected:
        void Subtask1() {}
        void Subtask2() {}
    };

    class TestSpec : public BaseTestSpec<ProblemSpec> {};
    class TestSpecWithConfig : public BaseTestSpec<ProblemSpecWithConfig> {};
    class TestSpecWithSubtasks : public BaseTestSpec<ProblemSpecWithSubtasks> {};

    class BadTestSpec : public BaseTestSpec<ProblemSpec> {
    public:
        Spec buildSpec(const string&) {
            throw runtime_error("An error");
        }
    };

    Runner<ProblemSpec> runner = BaseRunnerTests::createRunner(new TestSpec());
    Runner<ProblemSpecWithConfig> runnerWithConfig = BaseRunnerTests::createRunner(new TestSpecWithConfig());
    Runner<ProblemSpecWithSubtasks> runnerWithSubtasks = BaseRunnerTests::createRunner(new TestSpecWithSubtasks());

    Runner<ProblemSpec> createRunner(const string& specPath) {
        return Runner<ProblemSpec>(
                specPath, new TestSpec(), loggerEngine, &os, &runnerLoggerFactory, &graderLoggerFactory,
                &generatorFactory, &graderFactory, &evaluatorRegistry, &aggregatorRegistry);
    }
};

int RunnerTests::T;

TEST_F(RunnerTests, Run_SlugParsing_Failed) {
    Runner<ProblemSpec> badRunner = createRunner("/Users/fushar/january contest/c_slug slug/spec.cpp");

    EXPECT_THAT(badRunner.run(1, new char*[1]{(char*) "./runner"}), Ne(0));
}

TEST_F(RunnerTests, Run_ArgsParsing_Failed) {
    EXPECT_THAT(runner.run(2, new char*[2]{(char*) "./runner", (char*) "--blah"}), Ne(0));
}

TEST_F(RunnerTests, Run_Specification_Failed) {
    Runner<ProblemSpec> badRunner = BaseRunnerTests::createRunner(new BadTestSpec());

    EXPECT_CALL(generator, generate(_, _)).Times(0);
    EXPECT_CALL(runnerLogger, logSpecificationFailure(vector<string>{"An error"}));

    EXPECT_THAT(badRunner.run(argc, argv), Ne(0));
}

TEST_F(RunnerTests, Run_Generation_Successful) {
    ON_CALL(generator, generate(_, _)).WillByDefault(Return(true));

    EXPECT_THAT(runner.run(argc, argv), Eq(0));
}

TEST_F(RunnerTests, Run_Generation_Failed) {
    ON_CALL(generator, generate(_, _)).WillByDefault(Return(false));

    EXPECT_THAT(runner.run(argc, argv), Ne(0));
}

TEST_F(RunnerTests, Run_Generation_UseDefaultOptions) {
    EXPECT_CALL(generator, generate(_, GenerationOptionsBuilder("slug")
            .setSeed(0)
            .setSolutionCommand("./solution")
            .setOutputDir("tc")
            .setNeedsOutput(true)
            .build()));

    runner.run(argc, argv);
}

TEST_F(RunnerTests, Run_Generation_UseConfigOptions) {
    EXPECT_CALL(generator, generate(_, GenerationOptionsBuilder("slug")
            .setMultipleTestCasesCounter(&T)
            .setSolutionCommand("./solution")
            .setOutputDir("tc")
            .setNeedsOutput(false)
            .build()));

    runnerWithConfig.run(argc, argv);
}

TEST_F(RunnerTests, Run_Generation_UseArgsOptions) {
    EXPECT_CALL(generator, generate(_, GenerationOptionsBuilder("slug")
            .setSeed(42)
            .setMultipleTestCasesCounter(&T)
            .setSolutionCommand("\"java Solution\"")
            .setOutputDir("testdata")
            .setNeedsOutput(false)
            .build()));

    runnerWithConfig.run(4, new char*[5]{
            (char*) "./runner",
            (char*) "--seed=42",
            (char*) "--solution=\"java Solution\"",
            (char*) "--output=testdata",
            nullptr});
}

TEST_F(RunnerTests, Run_Grading) {
    EXPECT_CALL(grader, grade(_, _, _));

    int exitStatus = runner.run(2, new char*[3]{
            (char*) "./runner",
            (char*) "grade",
            nullptr});

    EXPECT_THAT(exitStatus, Eq(0));
}

TEST_F(RunnerTests, Run_Grading_UseDefaultOptions) {
    EXPECT_CALL(grader, grade(_, _, GradingOptionsBuilder("slug")
            .setHasMultipleTestCases(false)
            .setTimeLimit(2)
            .setMemoryLimit(64)
            .setSolutionCommand("./solution")
            .setOutputDir("tc")
            .build()));

    runner.run(2, new char*[3]{
            (char*) "./runner",
            (char*) "grade",
            nullptr});
}

TEST_F(RunnerTests, Run_Grading_UseConfigOptions) {
    EXPECT_CALL(grader, grade(_, _, GradingOptionsBuilder("slug")
            .setHasMultipleTestCases(true)
            .setSolutionCommand("./solution")
            .setOutputDir("tc")
            .setTimeLimit(3)
            .setMemoryLimit(128)
            .build()));

    runnerWithConfig.run(2, new char*[3]{
            (char*) "./runner",
            (char*) "grade",
            nullptr});
}

TEST_F(RunnerTests, Run_Grading_UseArgsOptions) {
    EXPECT_CALL(grader, grade(_, _, GradingOptionsBuilder("slug")
            .setHasMultipleTestCases(true)
            .setSolutionCommand("\"java Solution\"")
            .setOutputDir("testdata")
            .setTimeLimit(4)
            .setMemoryLimit(256)
            .build()));

    runnerWithConfig.run(6, new char*[7]{
            (char*) "./runner",
            (char*) "grade",
            (char*) "--solution=\"java Solution\"",
            (char*) "--output=testdata",
            (char*) "--time-limit=4",
            (char*) "--memory-limit=256",
            nullptr});
}

TEST_F(RunnerTests, Run_Grading_UseArgsOptions_NoLimits) {
    EXPECT_CALL(grader, grade(_, _, GradingOptionsBuilder("slug")
            .setHasMultipleTestCases(true)
            .setSolutionCommand("\"java Solution\"")
            .setOutputDir("testdata")
            .build()));

    runnerWithConfig.run(6, new char*[7]{
            (char*) "./runner",
            (char*) "grade",
            (char*) "--solution=\"java Solution\"",
            (char*) "--output=testdata",
            (char*) "--no-time-limit",
            (char*) "--no-memory-limit",
            nullptr});
}

TEST_F(RunnerTests, Run_Grading_DefaultLogger) {
    EXPECT_CALL(graderLoggerFactory, create(_, false));

    runner.run(2, new char*[3]{
            (char*) "./runner",
            (char*) "grade",
            nullptr});
}

TEST_F(RunnerTests, Run_Grading_BriefLogger) {
    EXPECT_CALL(graderLoggerFactory, create(_, true));

    runner.run(3, new char*[4]{
            (char*) "./runner",
            (char*) "grade",
            (char*) "--brief",
            nullptr});
}

TEST_F(RunnerTests, Run_AggregatorRegistry) {
    EXPECT_CALL(aggregatorRegistry, get(false));
    runner.run(2, new char*[3]{
            (char*) "./runner",
            (char*) "grade",
            nullptr});
}

TEST_F(RunnerTests, Run_AggregatorRegistry_WithSubtasks) {
    EXPECT_CALL(aggregatorRegistry, get(true));
    runnerWithSubtasks.run(2, new char*[3]{
            (char*) "./runner",
            (char*) "grade",
            nullptr});
}

}
