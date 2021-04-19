#include "OptionalClause.h"

ClauseType OptionalClause::getClauseType() {
	return this->clauseType;
}

shared_ptr<QueryInput> OptionalClause::getLeftInput() {
	return this->aLeftInput;
}
shared_ptr<QueryInput> OptionalClause::getRightInput() {
	return this->aRightInput;
}

ClauseResultType OptionalClause::getClauseResultType() {
	return this->clauseResultType;
}

void OptionalClause::addSetResult(unordered_set<string> results) {
	this->clauseResultType = ClauseResultType::SET;
	this->setResult = results;
	this->resultSize = results.size();
}

void OptionalClause::addMapResult(unordered_map<string, unordered_set<string>> results) {
	this->clauseResultType = ClauseResultType::MAP;
	this->mapResult = results;
	this->resultSize = 0;
	for (auto result : results) {
		this->resultSize += result.second.size();
	}
}

void OptionalClause::setBoolResult(bool result) {
	this->clauseResultType = ClauseResultType::BOOL;
	this->resultSize = 0;
	this->boolResult = result;
}

unordered_set<string> OptionalClause::getSetResult() {
	return this->setResult;
}

unordered_map<string, unordered_set<string>> OptionalClause::getMapResult() {
	return this->mapResult;
}

int OptionalClause::getResultSize() {
	return this->resultSize;
}

bool OptionalClause::getBoolResult() {
	return this->boolResult;
}

//bool OptionalClause::operator < (const OptionalClause& other) const {
//	return this->resultSize < other.resultSize;
//}

OptionalClause::~OptionalClause() {}