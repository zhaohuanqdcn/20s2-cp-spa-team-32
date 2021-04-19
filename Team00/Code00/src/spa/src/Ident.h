#pragma once
#include <string>
#include "QueryInput.h"

using namespace std;

class Ident : public QueryInput {
public:
	Ident(string value);
};