#pragma once
#include "ClauseType.h"
#include "ClauseResultType.h"
#include "QueryInput.h"
#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>

class OptionalClause {
protected:
	ClauseType clauseType;
	ClauseResultType clauseResultType;
	shared_ptr<QueryInput> aLeftInput;
	shared_ptr<QueryInput> aRightInput;

	unordered_set<string> setResult;
	unordered_map<string, unordered_set<string>> mapResult;
	bool boolResult;
	int resultSize;

public:
	ClauseType getClauseType();
	virtual shared_ptr<QueryInput> getLeftInput();
	virtual shared_ptr<QueryInput> getRightInput();

	ClauseResultType getClauseResultType();
	void addSetResult(unordered_set<string> results);
	void addMapResult(unordered_map<string, unordered_set<string>> results);
	void setBoolResult(bool result);
	unordered_set<string> getSetResult();
	unordered_map<string, unordered_set<string>> getMapResult();
	bool getBoolResult();
	int getResultSize();

	virtual ~OptionalClause();
};