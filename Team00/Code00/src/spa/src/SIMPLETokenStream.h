#ifndef __SIMPLE__TOKEN__STREAM__H__
#define __SIMPLE__TOKEN__STREAM__H__

#include <string>
#include <deque>

#include "SIMPLEToken.h"
#include "SIMPLEHelper.h"

using namespace std;

class SIMPLETokenStream {
    /* Taken in a vector, each element represent one line
    *  Therefore, each element should not contain newl character
    */

private:
    static constexpr const char* nonregconizedTokenErr = "%d:%d Error: Non-recognized token: %s";


    bool usable;
    deque<SIMPLEToken> tokenQ;
    SIMPLEToken lastConsumed;

    /*
    * This function will ignore empty token.
    */
    void insertToken(string token, int row, int column);


public:
    SIMPLETokenStream(vector<string> lineVector);

    bool isEmpty();

    bool isUsable();

    /**
        Consume 1 token.
    **/
    SIMPLEToken getToken();

    /**
        Return k token from the head of the stream (empty of not enough).
    **/
    vector<SIMPLEToken> lookAhead(int k);

    SIMPLEToken lookAheadSingle();

    SIMPLEToken getLastConsumedToken() const;
};
#endif /// __SIMPLE__TOKEN__STREAM__H__