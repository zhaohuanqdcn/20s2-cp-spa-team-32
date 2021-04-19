#pragma once
#include <string>
#include "QueryInputType.h"

using namespace std;

class QueryInput {
protected:
	string aValue;
	QueryInputType aQueryInputType;

public:
	QueryInputType getQueryInputType();
	string getValue();
	virtual ~QueryInput();
};