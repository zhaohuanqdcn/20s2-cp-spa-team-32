#pragma once

#include "OptionalClause.h"
#include "RelationshipType.h"

class RelationshipClause : public OptionalClause {
private:
	RelationshipType aRelationshipType;

public:
	RelationshipClause(RelationshipType relationshipType, shared_ptr<QueryInput> leftInput, shared_ptr<QueryInput> rightInput);
	RelationshipType getRelationshipType();
	shared_ptr<QueryInput> getLeftInput();
	shared_ptr<QueryInput> getRightInput();
};