#pragma once
#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include <string>

class InputStream
{
private:
    const std::string input;  // The query string
    int index{ 0 };  // Index of current character in stream

public:

    InputStream(const std::string& givenInput);

    char next();  // Returns the current character and advances the index
    char peek();  // Returns the current character but does not advance the index
    bool eof();  // Check if the entire query string has been processed

};

#endif