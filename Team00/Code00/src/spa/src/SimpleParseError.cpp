#include "SimpleParseError.h"

using namespace std;

ParseError::ParseError(
    const SIMPLEToken token,
    string errorMessage) : token(token), errorMessage(errorMessage) {
    hasErrorValue = true;
}

ParseError::ParseError(string errorMessage) {
    this->errorMessage = errorMessage;
    hasErrorValue = true;
}

ParseError::ParseError() {
    hasErrorValue = false;
}

bool ParseError::hasError() const {
    return this->hasErrorValue;
}

ParseError ParseError::combineWith(const ParseError& other) {
    if (this->hasError()) {
        return *this;
    }
    else return other;
}

string ParseError::getErrorMessage() const {
    return this->errorMessage;
}

string ParseError::getToken() const {
    return token.toString();
}