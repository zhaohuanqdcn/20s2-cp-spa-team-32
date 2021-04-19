#pragma once
#ifndef TOKEN_H
#define TOKEN_H

#include "TokenTypes.h"
#include "EntityType.h"
#include <string>

class Token
{
private:
    TokenTypes type;
    std::string value;

public:

    static std::string TokenTypeToString(TokenTypes type);

    static std::string EntityTypeToString(EntityType type);

    Token(TokenTypes tokenType, std::string tokenValue);

    TokenTypes getType();

    std::string getValue();

    std::string toString();

    EntityType getEntityType();

};

#endif
