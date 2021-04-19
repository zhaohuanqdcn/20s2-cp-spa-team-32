#include <fstream>
#include <cassert>

#include "SIMPLETokenStream.h"

using namespace std;

void SIMPLETokenStream::insertToken(string token, int row, int column) {
    if (token.size() == 0) {
        /// do nothing with empty token
        return;
    }
    try {
        tokenQ.push_back(SIMPLEToken(token, row, column));
    }
    catch (...) {
        usable = false;
        printf(nonregconizedTokenErr, row, column, token.c_str());
    }
}
SIMPLETokenStream::SIMPLETokenStream(vector<string> lineVector) {
    usable = true;

    int curRow = 1, curCol = 1;

    SIMPLEHelper helper;
    for (const auto& line : lineVector) {

        string curToken = "";
        for (const auto& ch : line) {
            if (helper.isSeparator(ch)) {
                /// push current token
                insertToken(curToken, curRow, curCol - curToken.size());
                curToken.clear();
            }
            else if (helper.isBrace(ch) || helper.isSemicolon(ch)) {
                /// encounter a brace or semicolon, must flush currentToken before proceed.
                insertToken(curToken, curRow, curCol - curToken.size());
                curToken.clear();

                insertToken(string(1, ch), curRow, curCol);
            }
            else {
                /// token is non-empty, check if new char is appendable to current token
                if (curToken.size() != 0) {
                    bool isAlphaNumericalToken = helper.isInteger(curToken) || helper.isName(curToken);
                    if (helper.isAlphaNumeric(ch) != isAlphaNumericalToken) {
                        /// different type, must clear current token first
                        insertToken(curToken, curRow, curCol - curToken.size());
                        curToken.clear();
                    }
                }
                curToken.push_back(ch);
            }
            curCol++;
        }

        /// add current token before goto next line
        insertToken(curToken, curRow, curCol - curToken.size());



        curRow++;
        curCol = 0;
    }

    if (!usable) {
        tokenQ.clear();
    }
}


bool SIMPLETokenStream::isEmpty() {
    return tokenQ.empty();
}

bool SIMPLETokenStream::isUsable() {
    return usable;
}

SIMPLEToken SIMPLETokenStream::getToken() {
    if (tokenQ.size() == 0) {
        throw "Empty stream";
    }
    auto token = tokenQ.front();
    lastConsumed = token;
    tokenQ.pop_front();
    return token;
}

vector<SIMPLEToken> SIMPLETokenStream::lookAhead(int k) {
    if (k > tokenQ.size()) {
        return {};
    }
    vector<SIMPLEToken> answer;
    for (int i = 0; i < k; i++) answer.push_back(tokenQ[i]);
    return answer;
}


SIMPLEToken SIMPLETokenStream::lookAheadSingle() {
    if (this->isEmpty()) {
        throw "Empty stream";
    }
    return tokenQ.front();
}


SIMPLEToken SIMPLETokenStream::getLastConsumedToken() const {
    return this->lastConsumed;
}