#pragma once

#include <stdexcept>

class SemanticException : public std::runtime_error {
public:
    SemanticException(const std::string& what_arg);
};
