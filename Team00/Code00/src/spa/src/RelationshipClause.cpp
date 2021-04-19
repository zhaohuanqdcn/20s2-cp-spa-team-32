#include "RelationshipClause.h"

RelationshipClause::RelationshipClause(RelationshipType relationshipType, shared_ptr<QueryInput> leftInput, shared_ptr<QueryInput> rightInput) {
	this->aRelationshipType = relationshipType;
	this->aLeftInput = leftInput;
	this->aRightInput = rightInput;
	this->clauseType = ClauseType::RELATIONSHIP;
}

RelationshipType RelationshipClause::getRelationshipType() {
	return this->aRelationshipType;
}

shared_ptr<QueryInput> RelationshipClause::getLeftInput() {
	return this->aLeftInput;
}

shared_ptr<QueryInput> RelationshipClause::getRightInput() {
	return this->aRightInput;
}