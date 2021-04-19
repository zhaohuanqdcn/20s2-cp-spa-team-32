#pragma once

#include "OptionalClause.h"
#include "Declaration.h"
#include "Expression.h"

class PatternClause : public OptionalClause {
private:
	shared_ptr<Expression> aExpression;

public:
	PatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput, shared_ptr<Expression> expression);
	PatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput);
	shared_ptr<Declaration> getSynonym();
	shared_ptr<QueryInput> getQueryInput();
	shared_ptr<Expression> getExpression();

	shared_ptr<QueryInput> getLeftInput();
	shared_ptr<QueryInput> getRightInput();
};