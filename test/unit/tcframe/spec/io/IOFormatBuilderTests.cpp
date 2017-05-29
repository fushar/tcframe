#include "gmock/gmock.h"

#include "tcframe/spec/io/IOFormat.hpp"

using ::testing::A;
using ::testing::ElementsAre;
using ::testing::SizeIs;
using ::testing::StrEq;
using ::testing::Test;

namespace tcframe {

class IOFormatBuilderTests : public Test {
protected:
    int X;
    string S;
    vector<string> V;
    vector<int> Y;
    vector<vector<int>> Z;

    IOFormatBuilder builder;
};

TEST_F(IOFormatBuilderTests, Building_Successful) {
    builder.prepareForInputFormat();
    builder
            .newLineIOSegment()
            .addScalarVariable(Scalar::create(X, "X"));
    builder
            .newRawLineIOSegment()
            .addScalarVariable(Scalar::createRaw(S, "S"));
    builder
            .newLinesIOSegment()
            .addVectorVariable(Vector::create(Y, "Y"))
            .setSize([] {return 3;});
    builder
            .newGridIOSegment()
            .addMatrixVariable(Matrix::create(Z, "Z"))
            .setSize([] {return 2;}, [] {return 3;})
            .build();

    builder.newOutputFormat();
    builder
            .newGridIOSegment()
            .addMatrixVariable(Matrix::create(Z, "Z"))
            .setSize([] {return 2;}, [] {return 3;})
            .build();
    builder
            .newLinesIOSegment()
            .addVectorVariable(Vector::create(Y, "Y"))
            .setSize([] {return 3;});
    builder.newOutputFormat();
    builder
            .newRawLinesIOSegment()
            .addVectorVariable(Vector::createRaw(V, "V"))
            .setSize([] {return 3;});
    builder
            .newLineIOSegment()
            .addScalarVariable(Scalar::create(X, "X"));
    builder.newOutputFormat(); // this should be ignored

    IOFormat ioFormat = builder.build();

    EXPECT_THAT(ioFormat.inputFormat(), ElementsAre(
            A<LineIOSegment*>(),
            A<RawLineIOSegment*>(),
            A<LinesIOSegment*>(),
            A<GridIOSegment*>()));

    ASSERT_THAT(ioFormat.outputFormats(), SizeIs(2));

    EXPECT_THAT(ioFormat.outputFormats()[0], ElementsAre(
            A<GridIOSegment*>(),
            A<LinesIOSegment*>()));

    EXPECT_THAT(ioFormat.outputFormats()[1], ElementsAre(
            A<RawLinesIOSegment*>(),
            A<LineIOSegment*>()));
}

TEST_F(IOFormatBuilderTests, Building_Failed_LinesSegmentWithoutSizeNotLast) {
    try {
        builder.prepareForInputFormat();
        builder
                .newLineIOSegment()
                .addScalarVariable(Scalar::create(X, "X"));
        builder
                .newLinesIOSegment()
                .addVectorVariable(Vector::create(Y, "Y"));
        builder
                .newGridIOSegment()
                .addMatrixVariable(Matrix::create(Z, "Z"))
                .setSize([] {return 2;}, [] {return 3;});
        builder.build();
        FAIL();
    } catch (runtime_error& e) {
        EXPECT_THAT(e.what(), StrEq("Lines segment without size can only be the last segment"));
    }
}


TEST_F(IOFormatBuilderTests, Building_Failed_RawLinesSegmentWithoutSizeNotLast) {
    try {
        builder.prepareForInputFormat();
        builder
                .newLineIOSegment()
                .addScalarVariable(Scalar::create(X, "X"));
        builder
                .newRawLinesIOSegment()
                .addVectorVariable(Vector::createRaw(V, "V"));
        builder
                .newGridIOSegment()
                .addMatrixVariable(Matrix::create(Z, "Z"))
                .setSize([] {return 2;}, [] {return 3;});
        builder.build();
        FAIL();
    } catch (runtime_error& e) {
        EXPECT_THAT(e.what(), StrEq("Raw lines segment without size can only be the last segment"));
    }
}

}
