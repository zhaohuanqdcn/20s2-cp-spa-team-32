#pragma once
#include <string>
#include <vector>
#include <memory>
#include "QueryInterface.h"
#include "Declaration.h"
#include "SelectClause.h"
#include "RelationshipClause.h"
#include "PatternClause.h"

using namespace std;

class Query : public QueryInterface {
private:
	shared_ptr<SelectClause> aSelectClause;
	vector<shared_ptr<OptionalClause>> aOptionalClauses;
    bool isBooleanQuery;
	
public:
	Query();
	void addDeclarationToSelectClause(shared_ptr<Declaration> declaration);
	void addRelationshipClause(RelationshipType relationshipType,
		shared_ptr<QueryInput> leftQueryInput, shared_ptr<QueryInput> rightQueryInput);
	void addAssignPatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput, shared_ptr<Expression> expression);
	void addContainerPatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput);
	void addWithClause(shared_ptr<QueryInput> leftQueryInput, shared_ptr<QueryInput> rightQueryInput);

	shared_ptr<SelectClause> getSelectClause();
	vector<shared_ptr<OptionalClause>> getOptionalClauses();
    void setIsBooleanQuery();
    bool getIsBooleanQuery();

};