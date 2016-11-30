#pragma once

#include <iostream>
#include <stdexcept>

#include "tcframe/logger.hpp"
#include "tcframe/spec/io.hpp"
#include "tcframe/spec/variable.hpp"
#include "tcframe/util.hpp"

using std::endl;
using std::istream;
using std::ostream;
using std::runtime_error;

namespace tcframe {

class LinesIOSegmentManipulator {
public:
    static string parse(LinesIOSegment* segment, istream* in) {
        for (Variable* variable : segment->variables()) {
            if (variable->type() == VariableType::VECTOR) {
                ((Vector*) variable)->clear();
            } else {
                ((Matrix*) variable)->clear();
            }
        }

        string lastVariableName;

        for (int j = 0; j < *segment->size(); j++) {
            bool isFirstColumn = true;
            for (Variable* variable : segment->variables()) {
                if (variable->type() == VariableType::VECTOR) {
                    if (!isFirstColumn) {
                        WhitespaceManipulator::parseSpace(in, lastVariableName);
                    }
                    ((Vector*) variable)->parseAndAddElementFrom(in);
                    lastVariableName = TokenFormatter::formatVectorElement(variable->name(), j);
                } else {
                    if (!isFirstColumn && !WhitespaceManipulator::canParseNewline(in)) {
                        WhitespaceManipulator::parseSpace(in, lastVariableName);
                    }
                    Matrix* matrixVariable = (Matrix*) variable;
                    matrixVariable->parseAndAddRowFrom(in, j);
                    lastVariableName = TokenFormatter::formatMatrixElement(variable->name(),
                                                                           j,
                                                                           matrixVariable->columns(j) - 1);
                }
                isFirstColumn = false;
            }
            WhitespaceManipulator::parseNewline(in, lastVariableName);
        }

        return lastVariableName;
    }

    static void print(LinesIOSegment* segment, ostream* out) {
        checkVectorSizes(segment);

        for (int j = 0; j < *segment->size(); j++) {
            for (int i = 0; i < segment->variables().size(); i++) {
                Variable *variable = segment->variables()[i];
                if (variable->type() == VariableType::VECTOR) {
                    if (i > 0) {
                        *out << ' ';
                    }
                    ((Vector*) variable)->printElementTo(j, out);
                } else {
                    if (((Matrix*) variable)->columns(j) > 0) {
                        *out << ' ';
                    }
                    ((Matrix*) variable)->printRowTo(j, out);
                }
            }
            *out << endl;
        }
    }

private:
    static void checkVectorSizes(LinesIOSegment* segment) {
        for (Variable* variable : segment->variables()) {
            int size;
            string type;
            if (variable->type() == VariableType::VECTOR) {
                size = ((Vector*) variable)->size();
                type = "vector";
            } else {
                size = ((Matrix*) variable)->rows();
                type = "jagged vector";
            }
            if (size != *segment->size()) {
                throw runtime_error(
                        "Number of elements of " + type + " " + TokenFormatter::formatVariable(variable->name())
                      + " unsatisfied. Expected: " + StringUtils::toString(*segment->size())
                      + ", actual: " + StringUtils::toString(size));
            }
        }
    }
};

}
