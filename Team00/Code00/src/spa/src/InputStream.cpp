#include "InputStream.h"

InputStream::InputStream(const std::string& givenInput) : input(givenInput)
{

}

char InputStream::next()
{
    char ch = input.at(index);
    index++;
    return ch;
}

char InputStream::peek()
{
    return input.at(index);
}

bool InputStream::eof()
{
    return (size_t)index >= input.size();
}