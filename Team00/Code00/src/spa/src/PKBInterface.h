#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "RelationshipType.h"
#include "EntityType.h"
#include "QueryInput.h"
#include "Expression.h"
#include "Declaration.h"
#include "Ident.h"

using namespace std;

class PKBInterface {

public:

	virtual unordered_set<string> getEntities(const EntityType& type) = 0;

	virtual bool getBooleanResultOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) = 0;

	virtual unordered_map<string, unordered_set<string>> getMapResultsOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) = 0;

	virtual unordered_set<string> getSetResultsOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) = 0;

	virtual unordered_map<string, unordered_set<string>> getMapResultsOfAssignPattern(
		shared_ptr<QueryInput> input, Expression& expression) = 0;

	virtual unordered_set<string> getSetResultsOfAssignPattern(
		shared_ptr<QueryInput> input, Expression& expression) = 0;

	virtual unordered_map<string, unordered_set<string>> getMapResultsOfContainerPattern(
		const EntityType& type, shared_ptr<QueryInput> input) = 0;

	virtual unordered_set<string> getSetResultsOfContainerPattern(
		const EntityType& type, shared_ptr<QueryInput> input) = 0;

	virtual unordered_map<string, unordered_set<string>> getDeclarationsMatchResults(shared_ptr<Declaration> leftDeclaration,
		shared_ptr<Declaration> rightDeclaration) = 0;

	virtual unordered_map<string, unordered_set<string>> getDeclarationMatchAttributeResults(
		shared_ptr<Declaration> declaration, EntityType attributeEntityType) = 0;

	virtual unordered_map<string, unordered_set<string>> getAttributesMatchResults(
		EntityType leftEntityType, EntityType rightEntityType) = 0;

	virtual unordered_set<string> getAttributeMatchNameResults(EntityType attributeEntityType, shared_ptr<Ident> name) = 0;

	virtual string getNameFromStmtNum(string stmtNum) = 0;
};