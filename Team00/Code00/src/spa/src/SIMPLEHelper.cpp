#include "SIMPLEHelper.h"

#include <algorithm>
#include <cassert>

using namespace std;

void SIMPLEHelper::buildStrToTokenType() {
    for (auto s : relSymbols) strToTokenType[s] = TokenType::REL_SYMBOL;
    for (auto s : condSymbols) strToTokenType[s] = TokenType::COND_SYMBOL;
    for (auto s : exprSymbols) strToTokenType[s] = TokenType::EXPR_SYMBOL;
    for (auto s : termSymbols) strToTokenType[s] = TokenType::TERM_SYMBOL;
    for (auto s : braces) strToTokenType[s] = TokenType::BRACE;
    for (auto s : semicolon) strToTokenType[s] = TokenType::SEMICOLON;
    for (auto s : notSymbols) strToTokenType[s] = TokenType::NOT_SYMBOL;
}

SIMPLEHelper::SIMPLEHelper() {
    buildStrToTokenType();
}

bool SIMPLEHelper::isBrace(char ch) {
    string chAsStr = string(1, ch);
    return getTokenType(chAsStr) == TokenType::BRACE;
}

bool SIMPLEHelper::isSemicolon(char ch) {
    string chAsStr = string(1, ch);
    return getTokenType(chAsStr) == TokenType::SEMICOLON;
}

bool SIMPLEHelper::isDigit(char ch) {
    return '0' <= ch && ch <= '9';
}

bool SIMPLEHelper::isInteger(string token) {
    assert(token.size() > 0);
    for (auto& ch : token) {
        if (!isDigit(ch)) return false;
    }

    /// can not have leading 0
    if (token.size() > 1 && token[0] == '0') {
        return false;
    }

    return true;
}

bool SIMPLEHelper::isName(string token) {
    assert(token.size() > 0);
    if (!isAlpha(token[0])) return false;
    for (auto& ch : token) {
        if (!isAlphaNumeric(ch)) return false;
    }
    return true;
}

bool SIMPLEHelper::isKeyword(string token) {
    return find(keywords.begin(), keywords.end(), token) != keywords.end();
}

bool SIMPLEHelper::isAlpha(char ch) {
    if ('a' <= ch && ch <= 'z') return true;
    if ('A' <= ch && ch <= 'Z') return true;
    return false;
}

bool SIMPLEHelper::isAlphaNumeric(char ch) {
    return isDigit(ch) || isAlpha(ch);
}

TokenType SIMPLEHelper::getTokenType(string token) {
    /// safe-guard
    if (token.empty()) {
        return TokenType::OTHER;
    }

    auto ite = strToTokenType.find(token);
    if (ite != strToTokenType.end()) {
        ///pre-defined symbol
        return ite->second;
    }

    if (isInteger(token)) {
        return TokenType::INTEGER;
    }

    if (isName(token)) {
        return TokenType::NAME;
    }

    return TokenType::OTHER;
}

string SIMPLEHelper::getTokenTypeName(TokenType type) {
    switch (type) {
    case TokenType::NOT_SYMBOL:
        return "!";
    case TokenType::KEYWORD:
        return "keyword";
    case TokenType::REL_SYMBOL:
        return "relSymbol";
    case TokenType::COND_SYMBOL:
        return "condSymbol";
    case TokenType::EXPR_SYMBOL:
        return "exprSymbol";
    case TokenType::TERM_SYMBOL:
        return "termSymbol";
    case TokenType::NAME:
        return "name";
    case TokenType::INTEGER:
        return "integer";
    case TokenType::BRACE:
        return "brace";
    case TokenType::SEMICOLON:
        return "semicolon";
    case TokenType::OTHER:
        return "other";
    };
}

bool SIMPLEHelper::isSeparator(char ch) {
    for (auto i : separators) {
        if (i == ch) return true;
    }
    return false;
}
