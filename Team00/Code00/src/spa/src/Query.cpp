#include "Query.h"

Query::Query() {
	this->aOptionalClauses = vector<shared_ptr<OptionalClause>>();
	this->aSelectClause = make_shared<SelectClause>();
    this->isBooleanQuery = false;
}

void Query::addDeclarationToSelectClause(shared_ptr<Declaration> declaration) {
	this->aSelectClause->addDeclaration(declaration);
}

void Query::addRelationshipClause(RelationshipType relationshipType, shared_ptr<QueryInput> leftQueryInput, shared_ptr<QueryInput> rightQueryInput) {
	shared_ptr<RelationshipClause> relationshipClause = make_shared<RelationshipClause>(relationshipType, leftQueryInput, rightQueryInput);
	this->aOptionalClauses.push_back(dynamic_pointer_cast<OptionalClause>(relationshipClause));
}

void Query::addAssignPatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput, shared_ptr<Expression> expression) {
	shared_ptr<PatternClause> patternClause = make_shared<PatternClause>(synonym, queryInput, expression);
	this->aOptionalClauses.push_back(dynamic_pointer_cast<OptionalClause>(patternClause));
}

void Query::addContainerPatternClause(shared_ptr<Declaration> synonym, shared_ptr<QueryInput> queryInput) {
	shared_ptr<PatternClause> patternClause = make_shared<PatternClause>(synonym, queryInput);
	this->aOptionalClauses.push_back(dynamic_pointer_cast<OptionalClause>(patternClause));
}

void Query::addWithClause(shared_ptr<QueryInput> leftQueryInput, shared_ptr<QueryInput> rightQueryInput) {
	shared_ptr<WithClause> withClause = make_shared<WithClause>(leftQueryInput, rightQueryInput);
	this->aOptionalClauses.push_back(dynamic_pointer_cast<OptionalClause>(withClause));
}


vector<shared_ptr<OptionalClause>>  Query::getOptionalClauses() {
	return this->aOptionalClauses;
}

shared_ptr<SelectClause> Query::getSelectClause() {
	return this->aSelectClause;
}

void Query::setIsBooleanQuery()
{
    isBooleanQuery = true;
}

bool Query::getIsBooleanQuery()
{
    return isBooleanQuery;
}

