#include "gmock/gmock.h"

#include <sstream>

#include "tcframe/io_manipulator/IOManipulator.hpp"

using ::testing::Eq;
using ::testing::StrEq;
using ::testing::Test;

using std::istringstream;
using std::ostringstream;

namespace tcframe {

class IOManipulatorTests : public Test {
protected:
    int A;
    vector<int> V;
    vector<vector<int>> M;

    IOManipulator* manipulatorWithScalarLast;
    IOManipulator* manipulatorWithVectorLast;
    IOManipulator* manipulatorWithMatrixLast;
    IOManipulator* manipulatorEmpty;

    void SetUp() {
        {
            IOFormatBuilder ioFormatBuilder;
            ioFormatBuilder.prepareForInputFormat();
            IOFormat ioFormat = ioFormatBuilder.build();

            manipulatorEmpty = new IOManipulator(ioFormat);
        }
        {
            IOFormatBuilder ioFormatBuilder;
            ioFormatBuilder.prepareForInputFormat();
            ioFormatBuilder.newLineIOSegment()
                    .addScalarVariable(Scalar::create(A, "A"));
            IOFormat ioFormat = ioFormatBuilder.build();

            manipulatorWithScalarLast = new IOManipulator(ioFormat);
        }
        {
            IOFormatBuilder ioFormatBuilder;
            ioFormatBuilder.prepareForInputFormat();
            ioFormatBuilder.newLineIOSegment()
                    .addScalarVariable(Scalar::create(A, "A"));
            ioFormatBuilder.newLinesIOSegment()
                    .addVectorVariable(Vector::create(V, "V"))
                    .setSize(new int(2));
            IOFormat ioFormat = ioFormatBuilder.build();

            manipulatorWithVectorLast = new IOManipulator(ioFormat);
        }
        {
            IOFormatBuilder ioFormatBuilder;
            ioFormatBuilder.prepareForInputFormat();
            ioFormatBuilder.newLineIOSegment()
                    .addScalarVariable(Scalar::create(A, "A"));
            ioFormatBuilder.newLinesIOSegment()
                    .addVectorVariable(Vector::create(V, "V"))
                    .setSize(new int(2));
            ioFormatBuilder.newGridIOSegment()
                    .addMatrixVariable(Matrix::create(M, "M"))
                    .setSize(new int(2), new int(2));
            IOFormat ioFormat = ioFormatBuilder.build();

            manipulatorWithMatrixLast = new IOManipulator(ioFormat);
        }
    }
};

TEST_F(IOManipulatorTests, Parsing_Successful) {
    istringstream in("123\n42\n7\n5 6\n7 8\n");
    manipulatorWithMatrixLast->parseInput(&in);
    EXPECT_THAT(A, Eq(123));
    EXPECT_THAT(V, Eq((vector<int>{42, 7})));
    EXPECT_THAT(M, Eq((vector<vector<int>>{{5, 6}, {7, 8}})));
}

TEST_F(IOManipulatorTests, Parsing_Failed_MissingEof_Empty) {
    istringstream in("123\n42\n7\n5 6\n7 8\nbogus");
    try {
        manipulatorEmpty->parseInput(&in);
        FAIL();
    } catch(runtime_error& e) {
        EXPECT_THAT(e.what(), StrEq("Expected: <EOF>"));
    }
}

TEST_F(IOManipulatorTests, Parsing_Failed_MissingEof_WithScalarLast) {
    istringstream in("123\n42\n7\n5 6\n7 8\nbogus");
    try {
        manipulatorWithScalarLast->parseInput(&in);
        FAIL();
    } catch(runtime_error& e) {
        EXPECT_THAT(e.what(), StrEq("Expected: <EOF> after 'A'"));
    }
}

TEST_F(IOManipulatorTests, Parsing_Failed_MissingEof_WithVectorLast) {
    istringstream in("123\n42\n7\n5 6\n7 8\nbogus");
    try {
        manipulatorWithVectorLast->parseInput(&in);
        FAIL();
    } catch(runtime_error& e) {
        EXPECT_THAT(e.what(), StrEq("Expected: <EOF> after 'V[1]'"));
    }
}

TEST_F(IOManipulatorTests, Parsing_Failed_MissingEof_WithMatrixLast) {
    istringstream in("123\n42\n7\n5 6\n7 8\nbogus");
    try {
        manipulatorWithMatrixLast->parseInput(&in);
        FAIL();
    } catch(runtime_error& e) {
        EXPECT_THAT(e.what(), StrEq("Expected: <EOF> after 'M[1][1]'"));
    }
}

TEST_F(IOManipulatorTests, Printing_Successful) {
    A = 123;
    V = {42, 7};
    M = {{5, 6}, {7, 8}};
    ostringstream out;
    manipulatorWithMatrixLast->printInput(&out);
    EXPECT_THAT(out.str(), Eq("123\n42\n7\n5 6\n7 8\n"));
}

}
