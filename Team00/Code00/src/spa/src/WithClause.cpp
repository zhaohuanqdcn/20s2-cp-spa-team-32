#include "WithClause.h"

WithClause::WithClause(shared_ptr<QueryInput> leftInput, shared_ptr<QueryInput> rightInput) {
	this->aLeftInput = leftInput;
	this->aRightInput = rightInput;
	this->clauseType = ClauseType::WITH;
}

shared_ptr<QueryInput> WithClause::getLeftInput() {
	return this->aLeftInput;
}

shared_ptr<QueryInput> WithClause::getRightInput() {
	return this->aRightInput;
}