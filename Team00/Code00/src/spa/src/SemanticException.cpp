#include "SemanticException.h"

SemanticException::SemanticException(const std::string& what_arg) : std::runtime_error(what_arg)
{

}