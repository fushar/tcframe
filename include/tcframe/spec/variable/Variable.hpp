#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>

#include "WhitespaceManipulator.hpp"

using std::char_traits;
using std::enable_if;
using std::is_arithmetic;
using std::is_reference;
using std::is_same;
using std::istream;
using std::ostream;
using std::runtime_error;
using std::string;
using std::tie;

namespace tcframe {

template<typename T>
using ScalarCompatible = typename enable_if<!is_reference<T>::value && (is_arithmetic<T>::value || is_same<string, T>::value)>::type;

template<typename T>
using NotScalarCompatible = typename enable_if<is_reference<T>::value || (!is_arithmetic<T>::value && !is_same<string, T>::value)>::type;

enum class VariableType {
    SCALAR,
    VECTOR,
    MATRIX
};

class Variable {
private:
    string name_;
    VariableType type_;

public:
    virtual ~Variable() {}

    virtual const string& name() const {
        return name_;
    }

    virtual VariableType type() const {
        return type_;
    }

    bool equals(Variable* o) const {
        return tie(name_, type_) == tie(o->name_, o->type_);
    }

protected:
    Variable(const string& name, VariableType type)
            : name_(name)
            , type_(type) {}

    template<typename T>
    static void parseValue(istream* in, T& val, const string& context) {
        WhitespaceManipulator::ensureNoEof(in, context);
        WhitespaceManipulator::ensureNoWhitespace(in, context);

        long long currentPos = in->tellg();
        *in >> val;

        if (in->fail()) {
            in->clear();
            in->seekg(currentPos);
            in->width(20);
            string found;
            *in >> found;

            throw runtime_error("Cannot parse for " + context + ". Found: '" + found + "'");
        }
    }

    static void parseRawLine(istream* in, string& val) {
        getline(*in, val);
        if (!in->eof()) {
            in->unget();
        }
    }
};

}
