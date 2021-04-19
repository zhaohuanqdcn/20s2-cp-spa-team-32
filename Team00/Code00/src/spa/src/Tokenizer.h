#pragma once
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "InputStream.h"
#include "TokenizerInterface.h"
#include <functional>
#include <set>
#include <queue>

class Tokenizer : public TokenizerInterface
{
private:

    static std::set<char> specialCharactersAmongIdentifiers;
    static std::set<std::string> designEntities;
    static std::set<std::string> attrNames;
    InputStream inputStream;
    std::queue<std::shared_ptr<Token>> tokenBuffer;

    bool isPureIdentifier(std::string s);
    std::shared_ptr<Token> readInteger();
    std::shared_ptr<Token> readIdentifier();
    std::string readWhile(std::function<bool(char)> predicate);

public:

    static bool canTreatAsIdent(TokenTypes type, std::string value);
    static bool hasSymbolInToken(TokenTypes type, std::string value);
    static std::shared_ptr<Token> convertTokenWithSymbolToIdentifierToken(TokenTypes type, std::shared_ptr<TokenizerInterface> tokenizer);
    Tokenizer(const std::string givenInput);
    std::shared_ptr<Token> readNext();
    void addToTokenBuffer(std::shared_ptr<Token> token);

};

#endif
