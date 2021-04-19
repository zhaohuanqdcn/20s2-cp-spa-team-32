#ifndef __SIMPLE__TOKEN__H__
#define __SIMPLE__TOKEN__H__

#include <string>

#include "SIMPLEHelper.h"

/**
    Represent a lexical token, store information of where it actually is in the source code as well.
**/
class SIMPLEToken {
private:
    static SIMPLEHelper helper;

    string value;

    int row;

    int column;

    TokenType type;

public:
    SIMPLEToken(string value, int row, int column); /// type will be automatically deduced

    SIMPLEToken();

    string getValue();

    int getRow();

    int getColumn();

    TokenType getTokenType();

    bool operator == (const SIMPLEToken& oth) const;

    string toString() const;
};

#endif  ///__SIMPLE__TOKEN__H__