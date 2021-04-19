#ifndef __SIMPLE__HELPER__H__
#define __SIMPLE__HELPER__H__

#include <vector>
#include <string>
#include <map>

using namespace std;

enum class TokenType {
    KEYWORD = 10, ///deprecated since keyword can also be identifier
    REL_SYMBOL = 3,
    NOT_SYMBOL = 2,
    COND_SYMBOL = 1,
    EXPR_SYMBOL = 4,
    TERM_SYMBOL = 5,
    NAME = 7,
    INTEGER = 8,
    BRACE = 6,
    SEMICOLON = 9,
    OTHER = 11 /// unable to detect type 
};

class SIMPLEHelper {
private:
    const vector<string> relSymbols = { ">", ">=", "<", "<=", "==", "!=" };

    const vector<string> condSymbols = { "&&", "||" };

    const vector<string> notSymbols = { "!" };

    /*
        Take note of the priority and possible expression
    */
    const vector<string> exprSymbols = { "+", "-" };

    const vector<string> termSymbols = { "*", "/", "%" };

    const vector<string> braces = { "{", "}", "(", ")" };

    const vector<string> semicolon = { ";" };

    const vector<char> separators = { ' ', '\t', '\v' };

    const vector<string> keywords = {
        "procedure",
        "read",
        "print",
        "call",
        "while",
        "if",
        "assign",
    };

    map<string, TokenType> strToTokenType;

    void buildStrToTokenType();

public:
    SIMPLEHelper();

    bool isBrace(char ch);

    bool isSemicolon(char ch);

    TokenType getTokenType(string token);

    /** There are library implementation but system dependence **/
    bool isDigit(char ch);

    bool isAlpha(char ch);

    bool isAlphaNumeric(char ch);

    bool isInteger(string token);

    bool isName(string token);

    bool isKeyword(string token);

    bool isSeparator(char ch);

    string getTokenTypeName(TokenType type);
};

#endif  ///__SIMPLE__HELPER__H__