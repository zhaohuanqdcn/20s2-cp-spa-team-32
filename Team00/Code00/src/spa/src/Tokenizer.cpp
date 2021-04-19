#include "Tokenizer.h"
#include "SyntacticException.h" // for throwing SyntacticException
#include <ctype.h>  // for std::isdigit, std::isalpha
#include <algorithm>  // for std::find
#include <iterator>  // for std::begin, std::end


std::set<std::string> Tokenizer::designEntities = { "stmt", "read", "print", "while", "if", "assign",
            "variable", "constant", "procedure", "prog_line", "call" };
std::set<std::string> Tokenizer::attrNames = { "procName", "varName", "value", "stmt#" };

/*
    This function determines if a Token type can be treated as an identifier.
    For example: Any one of the design entity tokens as well as attribute names with
    the exception of stmt# can be treated as an identifier and be consumed where
    an identifier is expected.

    This allows the Parser to be able to correctly parse identifiers in the event
    that synonym names are also the same as keywords instead of returning a syntax error.
*/
bool Tokenizer::canTreatAsIdent(TokenTypes type, std::string value) {
    // Keyword TokenTypes which can also be treated as identifiers when accepting tokens
    TokenTypes keywordTypes[] = { TokenTypes::Modifies, TokenTypes::Uses, TokenTypes::Parent,
                TokenTypes::Follows, TokenTypes::Select, TokenTypes::Such, TokenTypes::That, TokenTypes::Pattern,
                TokenTypes::Next, TokenTypes::Calls, TokenTypes::Affects, TokenTypes::AffectsBip, TokenTypes::NextBip, 
                TokenTypes::And, TokenTypes::With, TokenTypes::Boolean };
    if (std::find(std::begin(keywordTypes), std::end(keywordTypes), type) != std::end(keywordTypes))
        return true;
    // Possibly an identifier with same name as design entity token
    if (type == TokenTypes::DesignEntity && value != "prog_line")
        return true;
    if (type == TokenTypes::AttrName && value != "stmt#")
        return true;

    return false;
}

/*
    This function checks if a token type is one of the transitive relationship tokens.
*/
bool Tokenizer::hasSymbolInToken(TokenTypes type, std::string value) 
{
    TokenTypes typesWithSymbol[] = { TokenTypes::ParentT, TokenTypes::FollowsT,
                TokenTypes::NextT, TokenTypes::CallsT, TokenTypes::AffectsT,
                TokenTypes::AffectsBipT, TokenTypes::NextBipT };
    return std::find(std::begin(typesWithSymbol), std::end(typesWithSymbol), type) != std::end(typesWithSymbol);
}

/*
    This function converts a transitive relationship token into a pair of tokens, an Identifier and a TermSymbol token for the '*'
    This is required in case where we need to parse an identifier in an expression and we encounter a transitive relationship token.
    For example, an expression of "a + Modifies*c". We will first consume the identifier token after converting and add the
    TermSymbol token to the tokenBuffer to be consumed later.
*/
std::shared_ptr<Token> Tokenizer::convertTokenWithSymbolToIdentifierToken(TokenTypes type, std::shared_ptr<TokenizerInterface> tokenizer)
{
    std::shared_ptr<Token> token;
    switch (type)
    {
    case TokenTypes::ParentT:
        token = std::make_shared<Token>(Token{ TokenTypes::Identifier, "Parent" });
        break;
    case TokenTypes::FollowsT:
        token = std::make_shared<Token>(Token{ TokenTypes::Identifier, "Follows" });
        break;
    case TokenTypes::NextT:
        token = std::make_shared<Token>(Token{ TokenTypes::Identifier, "Next" });
        break;
    case TokenTypes::CallsT:
        token = std::make_shared<Token>(Token{ TokenTypes::Identifier, "Calls" });
        break;
    case TokenTypes::AffectsT:
        token = std::make_shared<Token>(Token{ TokenTypes::Identifier, "Affects" });
        break;
    case TokenTypes::AffectsBipT:
        token = std::make_shared<Token>(Token{ TokenTypes::Identifier, "AffectsBip" });
        break;
    case TokenTypes::NextBipT:
        token = std::make_shared<Token>(Token{ TokenTypes::Identifier, "NextBip" });
        break;
    default:
        break;
    }
    tokenizer->addToTokenBuffer(std::make_shared<Token>(Token{ TokenTypes::TermSymbol, "*" }));
    return token;
}

/*
    Tokenizer constructor that requires an inputStream to initialize.
*/
Tokenizer::Tokenizer(const std::string givenInput) : inputStream(givenInput)
{
    this->tokenBuffer = std::queue<std::shared_ptr<Token>>();
}

/*
    Read an integer from the input.
*/
std::shared_ptr<Token> Tokenizer::readInteger()
{
    std::string integer = readWhile(std::isdigit);
    return std::make_shared<Token>(Token{ TokenTypes::Integer, integer });
}

/*
    Determine if the string is a pure identifier; every character is alphanumeric.
    We do not need to check if the first character is only alphabetical and not numeric
    because the readNext function already filters out this possibility.
*/
bool Tokenizer::isPureIdentifier(std::string s)
{
    bool isValidIdentifier = true;
    for (char const& c : s) {
        if (!std::isalnum(c)) {
            isValidIdentifier = false;
            break;
        }
    }
    return isValidIdentifier;
}

/*
    Attempt to read an identifier from the input.
    We might encounter 3 special cases: "stmt#", "prog_line" and "Uses*" 
    and will have to handle these cases if detected.
*/
std::shared_ptr<Token> Tokenizer::readIdentifier()
{
    std::string identifier = std::string(1, inputStream.next());
    identifier += readWhile(std::isalnum);

    // Peek at the next character to handle special cases
    char c = ' ';
    if (!inputStream.eof()) 
        c = inputStream.peek();
    
    if (c == '#') {
        identifier += inputStream.next();
        if (identifier == "stmt#")
            return std::make_shared<Token>(Token{ TokenTypes::AttrName, identifier });
    }
    else if (c == '_') {
        identifier += inputStream.next();
        identifier += readWhile(std::isalpha);
        if (identifier == "prog_line" && (inputStream.eof() || std::isspace(inputStream.peek())))
            return std::make_shared<Token>(Token{ TokenTypes::DesignEntity, identifier });
    }
    else if (c == '*') {
        identifier += inputStream.peek();  // Do not consume this character yet
        std::shared_ptr<Token> token = std::shared_ptr<Token>();
        if (identifier == "Parent*") {
            token = std::make_shared<Token>(Token{ TokenTypes::ParentT, identifier });
        }
        else if (identifier == "Follows*") {
            token = std::make_shared<Token>(Token{ TokenTypes::FollowsT, identifier });
        }
        else if (identifier == "Calls*") {
            token = std::make_shared<Token>(Token{ TokenTypes::CallsT, identifier });
        }
        else if (identifier == "Next*") {
            token = std::make_shared<Token>(Token{ TokenTypes::NextT, identifier });
        }
        else if (identifier == "NextBip*") {
            token = std::make_shared<Token>(Token{ TokenTypes::NextBipT, identifier });
        }
        else if (identifier == "Affects*") {
            token = std::make_shared<Token>(Token{ TokenTypes::AffectsT, identifier });
        }
        else if (identifier == "AffectsBip*") {
            token = std::make_shared<Token>(Token{ TokenTypes::AffectsBipT, identifier });
        }

        // Consume the '*' character if a match was found
        if (token) {
            inputStream.next();
            return token;
        }
        else {
            // Remove the '*' from the last character of identifier since it produced no match
            // Attempt to match other identifiers
            identifier.erase(identifier.size() - 1);
        }
    }

    // Match other keywords
    std::shared_ptr<Token> token;
    if (identifier == "Select") {
        token = std::make_shared<Token>(Token{ TokenTypes::Select, identifier });
    }
    else if (identifier == "such") {
        token = std::make_shared<Token>(Token{ TokenTypes::Such, identifier });
    }
    else if (identifier == "that") {
        token = std::make_shared<Token>(Token{ TokenTypes::That, identifier });
    }
    else if (identifier == "pattern") {
        token = std::make_shared<Token>(Token{ TokenTypes::Pattern, identifier });
    }
    else if (identifier == "and") {
        token = std::make_shared<Token>(Token{ TokenTypes::And, identifier });
    }
    else if (identifier == "Modifies") {
        token = std::make_shared<Token>(Token{ TokenTypes::Modifies, identifier });
    }
    else if (identifier == "Uses") {
        token = std::make_shared<Token>(Token{ TokenTypes::Uses, identifier });
    }
    else if (identifier == "Parent") {
        token = std::make_shared<Token>(Token{ TokenTypes::Parent, identifier });
    }
    else if (identifier == "Follows") {
        token = std::make_shared<Token>(Token{ TokenTypes::Follows, identifier });
    }
    else if (identifier == "Calls") {
        token = std::make_shared<Token>(Token{ TokenTypes::Calls, identifier });
    }
    else if (identifier == "Next") {
        token = std::make_shared<Token>(Token{ TokenTypes::Next, identifier });
    }
    else if (identifier == "NextBip") {
        token = std::make_shared<Token>(Token{ TokenTypes::NextBip, identifier });
    }
    else if (identifier == "Affects") {
        token = std::make_shared<Token>(Token{ TokenTypes::Affects, identifier });
    }
    else if (identifier == "AffectsBip") {
        token = std::make_shared<Token>(Token{ TokenTypes::AffectsBip, identifier });
    }
    else if (identifier == "with") {
        token = std::make_shared<Token>(Token{ TokenTypes::With, identifier });
    }
    else if (identifier == "BOOLEAN") {
        token = std::make_shared<Token>(Token{ TokenTypes::Boolean, identifier });
    }
    else if (std::find(std::begin(Tokenizer::designEntities), std::end(Tokenizer::designEntities), identifier) != std::end(Tokenizer::designEntities)) {
        token = std::make_shared<Token>(Token{ TokenTypes::DesignEntity, identifier });
    }
    else if (std::find(std::begin(Tokenizer::attrNames), std::end(Tokenizer::attrNames), identifier) != std::end(Tokenizer::attrNames)) {
        token = std::make_shared<Token>(Token{ TokenTypes::AttrName, identifier });
    }
    else {
        // Valid identifier must not contain any special characters, throw syntax error if such characters found
        if (!isPureIdentifier(identifier))
            throw SyntacticException("Invalid identifier encountered: " + identifier);
        token = std::make_shared<Token>(Token{ TokenTypes::Identifier, identifier });
    }
    return token;
}

/*
    Reads from the input while characters satisfy a given predicate.
*/
std::string Tokenizer::readWhile(std::function<bool(char)> predicate)
{
    std::string str = "";
    while (!inputStream.eof() && predicate(inputStream.peek())) {
        str += inputStream.next();
    }
    return str;
}

/*
    Add a token to the Tokenizer's buffer.
*/
void Tokenizer::addToTokenBuffer(std::shared_ptr<Token> token)
{
    tokenBuffer.push(token);
}

/*
    This function is exposed to the QueryParser to call.
    It returns the next lexed token from the input on demand. (Tokenizer does Lazy Evaluation)
*/
std::shared_ptr<Token> Tokenizer::readNext()
{
    // Attempt to retrieve token from the tokenBuffer
    if (!tokenBuffer.empty()) {
        std::shared_ptr<Token> token = tokenBuffer.front();
        tokenBuffer.pop();
        return token;
    }

    readWhile(std::isspace);
    if (inputStream.eof()) return std::shared_ptr<Token>();
    char ch = inputStream.peek();
    switch (ch)
    {
    case '(':
        return std::make_shared<Token>(Token{ TokenTypes::LeftParen, std::string(1, inputStream.next()) });
        break;
    case ')':
        return std::make_shared<Token>(Token{ TokenTypes::RightParen, std::string(1, inputStream.next()) });
        break;
    case '"':
        return std::make_shared<Token>(Token{ TokenTypes::DoubleQuote, std::string(1, inputStream.next()) });
        break;
    case '_':
        return std::make_shared<Token>(Token{ TokenTypes::Underscore, std::string(1, inputStream.next()) });
        break;
    case ';':
        return std::make_shared<Token>(Token{ TokenTypes::Semicolon, std::string(1, inputStream.next()) });
        break;
    case ',':
        return std::make_shared<Token>(Token{ TokenTypes::Comma, std::string(1, inputStream.next()) });
        break;
    case '+':
    case '-':
        return std::make_shared<Token>(Token{ TokenTypes::ExprSymbol, std::string(1, inputStream.next()) });
        break;
    case '/':
    case '%':
    case '*':
        return std::make_shared<Token>(Token{ TokenTypes::TermSymbol, std::string(1, inputStream.next()) });
        break;
    case '<':
        return std::make_shared<Token>(Token{ TokenTypes::LeftAngleBracket, std::string(1, inputStream.next()) });
        break;
    case '>':
        return std::make_shared<Token>(Token{ TokenTypes::RightAngleBracket, std::string(1, inputStream.next()) });
        break;
    case '=':
        return std::make_shared<Token>(Token{ TokenTypes::Equals, std::string(1, inputStream.next()) });
        break;
    case '.':
        return std::make_shared<Token>(Token{ TokenTypes::Dot, std::string(1, inputStream.next()) });
        break;
    default:
        break;
    }
    if (std::isdigit(ch)) return readInteger();
    if (std::isalpha(ch)) return readIdentifier();
    throw SyntacticException("Cannot handle invalid character: " + ch);
}
