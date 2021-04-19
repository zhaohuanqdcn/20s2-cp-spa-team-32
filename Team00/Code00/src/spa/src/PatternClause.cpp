#include "PatternClause.h"

PatternClause::PatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput, shared_ptr<Expression> expression) {
	this->aLeftInput = dynamic_pointer_cast<QueryInput>(synonym);
	this->aRightInput = queryInput;
	this->aExpression = expression;
	this->clauseType = ClauseType::PATTERN;
}

PatternClause::PatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput) {
	this->aLeftInput = dynamic_pointer_cast<QueryInput>(synonym);
	this->aRightInput = queryInput;
	this->clauseType = ClauseType::PATTERN;
}

shared_ptr<Declaration> PatternClause::getSynonym() {
	return dynamic_pointer_cast<Declaration>(this->aLeftInput);
}

shared_ptr<QueryInput> PatternClause::getQueryInput() {
	return this->aRightInput;
}

shared_ptr<Expression> PatternClause::getExpression() {
	return this->aExpression;
}

shared_ptr<QueryInput> PatternClause::getLeftInput() {
	return this->aLeftInput;
}

shared_ptr<QueryInput> PatternClause::getRightInput() {
	return this->aRightInput;
}