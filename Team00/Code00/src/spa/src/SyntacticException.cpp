#include "SyntacticException.h"

SyntacticException::SyntacticException(const std::string& what_arg) : std::runtime_error(what_arg)
{

}