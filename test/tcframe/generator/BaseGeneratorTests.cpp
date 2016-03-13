#include "gtest/gtest.h"

#include "../commons/Commons.hpp"
#include "tcframe/generator/BaseGenerator.hpp"
#include "tcframe/os/ExecutionResult.hpp"

#include <map>
#include <string>

using std::map;
using std::string;

using tcframe::BaseGenerator;
using tcframe::ExecutionResult;

class MyGenerator : public BaseGenerator<DefaultProblem> {
    void Config() {
        setTestCasesDir("testdata");
        setSolutionCommand("java Solution");
    }
};

TEST(BaseGeneratorTest, DefaultOptions) {
    DefaultGenerator generator;
    generator.applyGeneratorConfiguration();

    EXPECT_EQ("tc", generator.getTestCasesDir());
    EXPECT_EQ("./solution", generator.getSolutionCommand());
}

TEST(BaseGeneratorTest, MyOptions) {
    MyGenerator generator;
    generator.applyGeneratorConfiguration();

    EXPECT_EQ("testdata", generator.getTestCasesDir());
    EXPECT_EQ("java Solution", generator.getSolutionCommand());
}

TEST(BaseGeneratorTest, CommandLineOptions) {
    char* argv[] = {(char*)"<runner>", (char*)"--tc-dir=testcases", (char*)"--solution-command=\"python sol.py\""};

    MyGenerator generator;
    generator.applyGeneratorConfiguration();
    generator.applyGeneratorCommandLineOptions(3, argv);

    EXPECT_EQ("testcases", generator.getTestCasesDir());
    EXPECT_EQ("\"python sol.py\"", generator.getSolutionCommand());
}

TEST(BaseGeneratorTest, GenerationWithSubtasksAndTestGroups) {
    FakeGeneratorLogger logger;
    GeneratorWithTestGroups<ProblemWithSubtasks> generator(&logger, new FakeGeneratorOperatingSystem());
    int exitCode = generator.generate();

    EXPECT_EQ(0, exitCode);

    EXPECT_EQ(0, logger.getFailures("problem_sample_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_1_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_1_2").size());
    EXPECT_EQ(0, logger.getFailures("problem_2_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_2_2").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_2").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_3").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_4").size());
}

TEST(BaseGeneratorTest, FailedGenerationWithSubtasksAndTestGroups) {
    FakeGeneratorLogger logger;
    InvalidGeneratorWithTestGroups<ProblemWithSubtasks> generator(&logger, new FakeGeneratorOperatingSystem());
    int exitCode = generator.generate();

    EXPECT_NE(0, exitCode);

    auto failures_sample_1 = logger.getFailures("problem_sample_1");
    ASSERT_EQ(2, failures_sample_1.size());
    EXPECT_EQ(Failure("Does not satisfy subtask 1, on constraints:", 0), failures_sample_1[0]);
    EXPECT_EQ(Failure("1 <= B && B <= 1000", 1), failures_sample_1[1]);

    auto failures_1_1 = logger.getFailures("problem_1_1");
    ASSERT_EQ(0, failures_1_1.size());

    auto failures_1_2 = logger.getFailures("problem_1_2");
    ASSERT_EQ(2, failures_1_2.size());
    EXPECT_EQ(Failure("Does not satisfy subtask 1, on constraints:", 0), failures_1_2[0]);
    EXPECT_EQ(Failure("1 <= B && B <= 1000", 1), failures_1_2[1]);

    auto failures_2_1 = logger.getFailures("problem_2_1");
    ASSERT_EQ(1, failures_2_1.size());
    EXPECT_EQ(Failure("Satisfies subtask 1 but is not assigned to it", 0), failures_2_1[0]);

    auto failures_2_2 = logger.getFailures("problem_2_2");
    ASSERT_EQ(4, failures_2_2.size());
    EXPECT_EQ(Failure("Does not satisfy subtask 2, on constraints:", 0), failures_2_2[0]);
    EXPECT_EQ(Failure("1 <= A && A <= 2000000000", 1), failures_2_2[1]);
    EXPECT_EQ(Failure("1 <= B && B <= 2000000000", 1), failures_2_2[2]);
    EXPECT_EQ(Failure("Satisfies subtask 3 but is not assigned to it", 0), failures_2_2[3]);

    EXPECT_EQ(0, logger.getFailures("problem_3_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_2").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_3").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_4").size());
}

TEST(BaseGeneratorTest, GenerationWithSubtasksAndTestGroupsAndMultipleTestCasesPerFile) {
    FakeGeneratorLogger logger;
    GeneratorWithTestGroups<ProblemWithMultipleTestCasesPerFile<ProblemWithSubtasks>> generator(&logger, new FakeGeneratorOperatingSystem());

    generator.applyProblemConfiguration();

    int exitCode = generator.generate();

    EXPECT_EQ(0, exitCode);

    EXPECT_EQ(0, logger.getFailures("problem_sample_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_1_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_2_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_2_2").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_2").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_3").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_4").size());

    EXPECT_EQ(0, logger.getFailures("problem_sample").size());
    EXPECT_EQ(0, logger.getFailures("problem_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_2").size());
    EXPECT_EQ(0, logger.getFailures("problem_3").size());
}

TEST(BaseGeneratorTest, FailedGenerationWithSubtasksAndTestGroupsAndMultipleTestCasesPerFile) {
    FakeGeneratorLogger logger;
    InvalidGeneratorWithTestGroups<ProblemWithMultipleTestCasesPerFile<ProblemWithSubtasks>> generator(&logger, new FakeGeneratorOperatingSystem());

    generator.applyProblemConfiguration();

    int exitCode = generator.generate();

    EXPECT_NE(0, exitCode);

    EXPECT_EQ(2, logger.getFailures("problem_sample_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_1_1").size());
    EXPECT_EQ(1, logger.getFailures("problem_2_1").size());
    EXPECT_EQ(4, logger.getFailures("problem_2_2").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_2").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_3").size());
    EXPECT_EQ(0, logger.getFailures("problem_3_4").size());

    auto failures_3 = logger.getFailures("problem_3");
    ASSERT_EQ(2, failures_3.size());
    EXPECT_EQ(Failure("Does not satisfy multiple test cases constraints, on:", 0), failures_3[0]);
    EXPECT_EQ(Failure("1 <= T && T <= 3", 1), failures_3[1]);
}

TEST(BaseGeneratorTest, GenerationWithoutSubtasksAndWithoutTestGroups) {
    FakeGeneratorLogger logger;
    GeneratorWithoutTestGroups<ProblemWithoutSubtasks> generator(&logger, new FakeGeneratorOperatingSystem());
    int exitCode = generator.generate();

    EXPECT_EQ(0, exitCode);

    EXPECT_EQ(0, logger.getFailures("problem_sample_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_sample_2").size());
    EXPECT_EQ(0, logger.getFailures("problem_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_2").size());
}

TEST(BaseGeneratorTest, FailedGenerationWithoutSubtasksAndWithoutTestGroups) {
    FakeGeneratorLogger logger;
    InvalidGeneratorWithoutTestGroups generator(&logger, new FakeGeneratorOperatingSystem());
    int exitCode = generator.generate();

    EXPECT_NE(0, exitCode);

    auto failures_sample_1 = logger.getFailures("problem_sample_1");
    ASSERT_EQ(0, failures_sample_1.size());

    auto failures_sample_2 = logger.getFailures("problem_sample_2");
    ASSERT_EQ(2, failures_sample_2.size());
    EXPECT_EQ(Failure("Does not satisfy constraints, on:", 0), failures_sample_2[0]);
    EXPECT_EQ(Failure("0 <= K <= 100", 1), failures_sample_2[1]);

    auto failures_1 = logger.getFailures("problem_1");
    ASSERT_EQ(0, failures_1.size());

    auto failures_2 = logger.getFailures("problem_2");
    ASSERT_EQ(2, failures_sample_2.size());
    EXPECT_EQ(Failure("Does not satisfy constraints, on:", 0), failures_2[0]);
    EXPECT_EQ(Failure("1 <= B && B <= 1000", 1), failures_2[1]);
}

TEST(BaseGeneratorTest, GenerationWithoutSubtasksAndWithoutTestGroupsAndWithMultipleTestCasesPerFile) {
    FakeGeneratorLogger logger;
    GeneratorWithoutTestGroups<ProblemWithMultipleTestCasesPerFile<ProblemWithoutSubtasks>> generator(&logger, new FakeGeneratorOperatingSystem());

    generator.applyProblemConfiguration();

    int exitCode = generator.generate();

    EXPECT_EQ(0, exitCode);

    EXPECT_EQ(0, logger.getFailures("problem_sample_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_sample_2").size());
    EXPECT_EQ(0, logger.getFailures("problem_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_2").size());

    EXPECT_EQ(0, logger.getFailures("problem_sample").size());
    EXPECT_EQ(0, logger.getFailures("problem").size());
}

TEST(BaseGeneratorTest, FailedGenerationWithoutSubtasksAndWithoutTestGroupsAndWithMultipleTestCasesPerFile) {
    FakeGeneratorLogger logger;
    InvalidGeneratorWithoutTestGroupsAndWithMultipleTestCasesPerFile generator(&logger, new FakeGeneratorOperatingSystem());

    generator.applyProblemConfiguration();

    int exitCode = generator.generate();

    EXPECT_NE(0, exitCode);

    EXPECT_EQ(0, logger.getFailures("problem_1").size());
    EXPECT_EQ(0, logger.getFailures("problem_2").size());
    EXPECT_EQ(0, logger.getFailures("problem_3").size());
    EXPECT_EQ(0, logger.getFailures("problem_4").size());

    auto failures = logger.getFailures("problem");
    ASSERT_EQ(2, failures.size());
    EXPECT_EQ(Failure("Does not satisfy multiple test cases constraints, on:", 0), failures[0]);
    EXPECT_EQ(Failure("1 <= T && T <= 3", 1), failures[1]);
}

TEST(BaseGeneratorTest, GenerationWithFailedExecution) {
    FakeGeneratorLogger logger;

    map<string, ExecutionResult> arrangedResultsMap = {
            {"problem_1-generation-evaluation", ExecutionResult{1, new istringstream(), new istringstream("Intentionally failed")} }
    };

    GeneratorWithoutTestGroups<ProblemWithoutSubtasks> generator(&logger, new FakeGeneratorOperatingSystem(arrangedResultsMap));
    int exitCode = generator.generate();

    EXPECT_NE(0, exitCode);

    auto failures = logger.getFailures("problem_1");
    ASSERT_EQ(3, failures.size());
    EXPECT_EQ(Failure("Execution of solution failed:", 0), failures[0]);
    EXPECT_EQ(Failure("Exit code: 1", 1), failures[1]);
    EXPECT_EQ(Failure("Standard error: Intentionally failed", 1), failures[2]);
}
