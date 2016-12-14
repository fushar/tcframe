#include "tcframe/generator.hpp"
#include "tcframe/grader.hpp"
#include "tcframe/grading_style.hpp"
#include "tcframe/os.hpp"
#include "tcframe/runner.hpp"
#include "tcframe/spec.hpp"

#include __TCFRAME_SPEC_FILE__

using tcframe::Runner;

int main(int argc, char* argv[]) {
    auto testSpec = new TestSpec();
    auto loggerEngine = new SimpleLoggerEngine();
    auto os = new UnixOperatingSystem();
    auto runnerLoggerFactory = new RunnerLoggerFactory();
    auto gradingStyleFactory = new GradingStyleFactory();
    auto generatorFactory = new GeneratorFactory();
    auto graderFactory = new GraderFactory();

    Runner<ProblemSpec> runner(
            __TCFRAME_SPEC_FILE__,
            testSpec,
            loggerEngine,
            os,
            runnerLoggerFactory,
            gradingStyleFactory,
            generatorFactory,
            graderFactory);

    return runner.run(argc, argv);
}
