#pragma once

#include <stdexcept>

class SyntacticException : public std::runtime_error {
public:
    SyntacticException(const std::string& what_arg);
};
