#pragma once

#include "OptionalClause.h"

class WithClause : public OptionalClause {

public:
	WithClause(shared_ptr<QueryInput> leftInput, shared_ptr<QueryInput> rightInput);
	shared_ptr<QueryInput> getLeftInput();
	shared_ptr<QueryInput> getRightInput();
};