#include <sstream>

#include "SIMPLEToken.h"

SIMPLEHelper SIMPLEToken::helper = SIMPLEHelper();

SIMPLEToken::SIMPLEToken(string value, int row, int column) {
    this->value = value;
    this->row = row;
    this->column = column;
    this->type = helper.getTokenType(value);
}

SIMPLEToken::SIMPLEToken() {
    this->value = "";
    this->row = 0;
    this->column = 0;
    this->type = TokenType::OTHER;
}

string SIMPLEToken::getValue() {
    return value;
}

int SIMPLEToken::getRow() {
    return row;
}

int SIMPLEToken::getColumn() {
    return column;
}

bool SIMPLEToken::operator == (const SIMPLEToken& oth) const {
    return (value == oth.value
        && row == oth.row
        && column == oth.column
        );
}

string SIMPLEToken::toString() const {
    std::ostringstream stream;
    stream << helper.getTokenTypeName(type) << " token at " << row << ":" << column << " " << value;
    return stream.str();
}

TokenType SIMPLEToken::getTokenType() {
    return this->type;
}