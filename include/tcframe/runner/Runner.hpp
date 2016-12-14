#pragma once

#include <iostream>

#include "Args.hpp"
#include "ArgsParser.hpp"
#include "RunnerLogger.hpp"
#include "RunnerLoggerFactory.hpp"
#include "SlugParser.hpp"
#include "tcframe/evaluator.hpp"
#include "tcframe/generator.hpp"
#include "tcframe/grader.hpp"
#include "tcframe/grading_style.hpp"
#include "tcframe/os.hpp"
#include "tcframe/scorer.hpp"
#include "tcframe/spec.hpp"
#include "tcframe/util.hpp"
#include "tcframe/verifier.hpp"

using std::cout;
using std::endl;

namespace tcframe {

template<typename TProblemSpec>
class Runner {
private:
    string specPath_;

    BaseTestSpec<TProblemSpec>* testSpec_;

    LoggerEngine* loggerEngine_;
    OperatingSystem* os_;

    RunnerLoggerFactory* runnerLoggerFactory_;
    GradingStyleFactory* gradingStyleFactory_;
    GeneratorFactory* generatorFactory_;
    GraderFactory* graderFactory_;

public:
    Runner(
            const string& specPath,
            BaseTestSpec<TProblemSpec>* testSpec,
            LoggerEngine* loggerEngine,
            OperatingSystem* os,
            RunnerLoggerFactory* runnerLoggerFactory,
            GradingStyleFactory* gradingStyleFactory,
            GeneratorFactory* generatorFactory,
            GraderFactory* graderFactory)
            : specPath_(specPath)
            , testSpec_(testSpec)
            , loggerEngine_(loggerEngine)
            , os_(os)
            , runnerLoggerFactory_(runnerLoggerFactory)
            , gradingStyleFactory_(gradingStyleFactory)
            , generatorFactory_(generatorFactory)
            , graderFactory_(graderFactory) {}

    int run(int argc, char* argv[]) {
        auto runnerLogger = runnerLoggerFactory_->create(loggerEngine_);

        try {
            string slug = parseSlug();
            Args args = parseArgs(argc, argv);
            Spec spec = buildSpec(slug, runnerLogger);

            int result;
            if (args.command() == Args::Command::GENERATE) {
                result = generate(slug, args, spec);
            } else {
                result = grade(slug, args, spec);
            }
            cleanUp();
            return result;
        } catch (...) {
            return 1;
        }
    }

private:
    string parseSlug() {
        try {
            return SlugParser::parse(specPath_);
        } catch (runtime_error& e) {
            cout << e.what() << endl;
            throw;
        }
    }

    Args parseArgs(int argc, char* argv[]) {
        try {
            return ArgsParser::parse(argc, argv);
        } catch (runtime_error& e) {
            cout << e.what() << endl;
            throw;
        }
    }

    Spec buildSpec(const string& slug, RunnerLogger* runnerLogger) {
        try {
            return testSpec_->buildSpec(slug);
        } catch (runtime_error& e) {
            runnerLogger->logSpecificationFailure({e.what()});
            throw;
        }
    }

    int generate(const string& slug, const Args& args, const Spec& spec) {
        const MultipleTestCasesConfig& multipleTestCasesConfig = spec.multipleTestCasesConfig();

        GeneratorConfig generatorConfig = GeneratorConfigBuilder(slug)
                .setMultipleTestCasesCounter(multipleTestCasesConfig.counter())
                .setMultipleTestCasesOutputPrefix(multipleTestCasesConfig.outputPrefix())
                .setSeed(args.seed())
                .setSolutionCommand(args.solution())
                .setOutputDir(args.output())
                .setNeedsOutput(spec.styleConfig().needsOutput())
                .build();

        auto ioManipulator = new IOManipulator(spec.ioFormat());
        auto verifier = new Verifier(spec.constraintSuite());
        auto gradingStyle = getGradingStyle(args, spec);
        auto evaluator = gradingStyle.evaluator();
        auto scorer = gradingStyle.scorer();
        auto logger = new GeneratorLogger(loggerEngine_);
        auto testCaseGenerator = new TestCaseGenerator(verifier, ioManipulator, os_, evaluator, scorer, logger);
        auto generator = generatorFactory_->create(spec.seedSetter(), testCaseGenerator, verifier, os_, logger);

        return generator->generate(spec.testSuite(), generatorConfig) ? 0 : 1;
    }

    int grade(const string& slug, const Args& args, const Spec& spec) {
        const MultipleTestCasesConfig& multipleTestCasesConfig = spec.multipleTestCasesConfig();
        const GradingConfig& gradingConfig = spec.gradingConfig();

        GraderConfigBuilder configBuilder = GraderConfigBuilder(slug)
                .setHasMultipleTestCases(optional<bool>(multipleTestCasesConfig.counter()))
                .setSolutionCommand(args.solution())
                .setOutputDir(args.output());

        if (!args.noTimeLimit()) {
            configBuilder.setTimeLimit(args.timeLimit().value_or(gradingConfig.timeLimit()));
        }
        if (!args.noMemoryLimit()) {
            configBuilder.setMemoryLimit(args.memoryLimit().value_or(gradingConfig.memoryLimit()));
        }

        GraderConfig graderConfig = configBuilder.build();

        auto logger = new GraderLogger(loggerEngine_);
        auto gradingStyle = getGradingStyle(args, spec);
        auto evaluator = gradingStyle.evaluator();
        auto scorer = gradingStyle.scorer();
        auto testCaseGrader = new TestCaseGrader(evaluator, scorer, logger);
        auto grader = graderFactory_->create(testCaseGrader, logger);

        grader->grade(spec.testSuite(), spec.constraintSuite(), graderConfig);
        return 0;
    }

    GradingStyle getGradingStyle(const Args& args, const Spec& spec) {
        optional<string> scorerCommand;
        if (spec.styleConfig().needsCustomScorer()) {
            scorerCommand = optional<string>(args.scorer().value_or(CommonConfig::scorerCommand()));
        }
        return gradingStyleFactory_->createBatch(os_, scorerCommand);
    }

    void cleanUp() {
        os_->execute(ExecutionRequestBuilder().setCommand("rm _*.out").build());
    }
};

}
