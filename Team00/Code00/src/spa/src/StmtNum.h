#pragma once
#include <string>
#include "QueryInput.h"
using namespace std;

class StmtNum: public QueryInput{
public: 
	StmtNum(int value);
	StmtNum(string value);
};