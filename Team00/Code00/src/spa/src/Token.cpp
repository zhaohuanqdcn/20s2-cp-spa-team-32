#include "Token.h"
#include <unordered_map>

std::string Token::TokenTypeToString(TokenTypes type)
{
    switch (type)
    {
    case TokenTypes::And:
        return "Token Type of And";
        break;
    case TokenTypes::Comma:
        return "Token Type of Comma";
        break;
    case TokenTypes::DesignEntity:
        return "Token Type of DesignEntity";
        break;
    case TokenTypes::DoubleQuote:
        return "Token Type of DoubleQuote";
        break;
    case TokenTypes::Follows:
        return "Token Type of Follows";
        break;
    case TokenTypes::Identifier:
        return "Token Type of Identifier";
        break;
    case TokenTypes::Integer:
        return "Token Type of Integer";
        break;
    case TokenTypes::LeftParen:
        return "Token Type of LeftParen";
        break;
    case TokenTypes::Modifies:
        return "Token Type of Modifies";
        break;
    case TokenTypes::Uses:
        return "Token Type of Uses";
        break;
    case TokenTypes::Parent:
        return "Token Type of Parent";
        break;
    case TokenTypes::ParentT:
        return "Token Type of ParentT";
        break;
    case TokenTypes::Calls:
        return "Token Type of Calls";
        break;
    case TokenTypes::CallsT:
        return "Token Type of CallsT";
        break;
    case TokenTypes::Next:
        return "Token Type of Next";
        break;
    case TokenTypes::NextT:
        return "Token Type of NextT";
        break;
    case TokenTypes::NextBip:
        return "Token Type of NextBip";
        break;
    case TokenTypes::NextBipT:
        return "Token Type of NextBipT";
        break;
    case TokenTypes::Affects:
        return "Token Type of Affects";
        break;
    case TokenTypes::AffectsT:
        return "Token Type of AffectsT";
        break;
    case TokenTypes::AffectsBip:
        return "Token Type of AffectsBip";
        break;
    case TokenTypes::AffectsBipT:
        return "Token Type of AffectsBipT";
        break;
    case TokenTypes::Pattern:
        return "Token Type of Pattern";
        break;
    case TokenTypes::RightParen:
        return "Token Type of RightParen";
        break;
    case TokenTypes::Select:
        return "Token Type of Select";
        break;
    case TokenTypes::Semicolon:
        return "Token Type of Semicolon";
        break;
    case TokenTypes::Such:
        return "Token Type of Such";
        break;
    case TokenTypes::That:
        return "Token Type of That";
        break;
    case TokenTypes::Underscore:
        return "Token Type of Underscore";
        break;
    case TokenTypes::ExprSymbol:
        return "Token Type of ExprSymbol";
        break;
    case TokenTypes::TermSymbol:
        return "Token Type of TermSymbol";
        break;
    case TokenTypes::LeftAngleBracket:
        return "Token Type of LeftAngleBracket";
        break;
    case TokenTypes::RightAngleBracket:
        return "Token Type of RightAngleBracket";
        break;
    case TokenTypes::AttrName:
        return "Token Type of AttrName";
        break;
    case TokenTypes::Equals:
        return "Token Type of Equals";
        break;
    case TokenTypes::With:
        return "Token Type of With";
        break;
    case TokenTypes::Dot:
        return "Token Type of Dot";
        break;
    default:  // case TokenTypes::Boolean
        return "Token Type of Boolean";
        break;
    }
}

std::string Token::EntityTypeToString(EntityType type)
{
    switch (type)
    {
    case EntityType::ASSIGN:
        return "Entity Type of Assign";
        break;
    case EntityType::CALL:
        return "Entity Type of Call";
        break;
    case EntityType::CONST:
        return "Entity Type of Constant";
        break;
    case EntityType::IF:
        return "Entity Type of If";
        break;
    case EntityType::PRINT:
        return "Entity Type of Print";
        break;
    case EntityType::PROC:
        return "Entity Type of Procedure";
        break;
    case EntityType::READ:
        return "Entity Type of Read";
        break;
    case EntityType::STMT:
        return "Entity Type of Stmt";
        break;
    case EntityType::VAR:
        return "Entity Type of Variable";
        break;
    case EntityType::PROGLINE:
        return "Entity Type of Program line";
        break;
    case EntityType::WHILE:
        return "Entity Type of While";
        break;
    default:  // case EntityType::Null
        return "Entity Type of Null";
        break;
    }
}

Token::Token(TokenTypes tokenType, std::string tokenValue)
{
    type = tokenType;
    value = tokenValue;
}

TokenTypes Token::getType()
{
    return type;
}

std::string Token::getValue()
{
    return value;
}

std::string Token::toString()
{
    return Token::TokenTypeToString(type) + " with value: " + value;
}

EntityType Token::getEntityType() {
    if (type != TokenTypes::DesignEntity) return EntityType::NONETYPE;
    std::unordered_map<std::string, EntityType> stringToCase = { {"stmt", EntityType::STMT}, {"read", EntityType::READ},
        {"print", EntityType::PRINT}, {"while", EntityType::WHILE}, {"if", EntityType::IF}, {"assign", EntityType::ASSIGN},
        {"variable", EntityType::VAR}, {"constant", EntityType::CONST}, {"procedure", EntityType::PROC}, 
        {"prog_line", EntityType::PROGLINE}, {"call", EntityType::CALL} };
    return stringToCase[value];
}