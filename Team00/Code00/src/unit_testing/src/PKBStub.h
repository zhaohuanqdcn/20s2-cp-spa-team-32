#pragma once
#include "PKBInterface.h"
#include <memory>
#include <unordered_set>
#include <vector>
#include <unordered_map>

using namespace std;

class PKBStub : public PKBInterface {
public:

	vector<unordered_map<string, unordered_set<string> > > mapResults;
	int mapResultsCount;

	vector<unordered_set<string>> setResults;
	int setResultsCount;

	vector<EntityType> getEntityTypes;
	int entityTypeCount;

	vector<bool> boolReturnValues;
	int boolCount;

	vector<string> stringValues;
	int stringCount;

	// original api being stubbed

	unordered_set<string> getEntities(const EntityType& type);

	bool getBooleanResultOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2);

	unordered_map<string, unordered_set<string> > getMapResultsOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2);

	unordered_set<string> getSetResultsOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2);

	unordered_map<string, unordered_set<string> > getMapResultsOfAssignPattern(
		shared_ptr<QueryInput> input, Expression& expression);

	unordered_set<string> getSetResultsOfAssignPattern(
		shared_ptr<QueryInput> input, Expression& expression);

	unordered_map<string, unordered_set<string> > getMapResultsOfContainerPattern(
		const EntityType& type, shared_ptr<QueryInput> input);

	unordered_set<string> getSetResultsOfContainerPattern(
		const EntityType& type, shared_ptr<QueryInput> input);

	unordered_map<string, unordered_set<string>> getDeclarationsMatchResults(shared_ptr<Declaration> leftDeclaration,
		shared_ptr<Declaration> rightDeclaration);

	unordered_map<string, unordered_set<string>> getDeclarationMatchAttributeResults(
		shared_ptr<Declaration> declaration, EntityType attributeEntityType);

	unordered_map<string, unordered_set<string>> getAttributesMatchResults(
		EntityType leftEntityType, EntityType rightEntityType);

	unordered_set<string> getAttributeMatchNameResults(EntityType attributeEntityType, shared_ptr<Ident> name);

	string getNameFromStmtNum(string stmtNum);

	// Methods to add the return values for each different test case
	void addBooleanResult(bool value);

	void addMapResult(unordered_map<string, unordered_set<string>> value);

	void addSetResult(unordered_set<string> value);

	void addEntityType(EntityType entityType);

	void addStringResult(string value);

	// Other methods
	PKBStub();
	void resetCounts();
	
};