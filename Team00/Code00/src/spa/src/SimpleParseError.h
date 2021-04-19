#ifndef __SIMPLE__PARSE_ERROR__H__
#define __SIMPLE__PARSE_ERROR__H__

#include "SIMPLEToken.h"

using namespace std;

class ParseError {
public:
    /* Constructs parse error with an error*/
    ParseError(SIMPLEToken token, string errorMessage);

    /* Constructs parse error without a token -- semantic error*/
    ParseError(string errorMessage);

    /* Constructs an empty error*/
    ParseError();

    /* Returns true if this is an error*/
    bool hasError() const;

    /* Merging two error togethers*/
    ParseError combineWith(const ParseError& other);

    string getErrorMessage() const;

    string getToken() const;
private:
    SIMPLEToken token;
    string errorMessage;
    bool hasErrorValue;
};

#endif ///__SIMPLE__PARSE_ERROR__H__
