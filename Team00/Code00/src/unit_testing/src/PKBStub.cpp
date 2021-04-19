#include "PKBStub.h"

#include <set>
#include <vector>
#include <unordered_map>

void PKBStub::resetCounts() {
	this->setResultsCount = 0;
	this->mapResultsCount = 0;
	this->boolCount = 0;;
	this->entityTypeCount = 0;
	this->stringCount = 0;
}

PKBStub::PKBStub() {
	this->resetCounts();
}

unordered_set<string> PKBStub::getEntities(const EntityType& type) {
	if (this->setResults.size() <= this->setResultsCount || this->getEntityTypes.size() <= this->entityTypeCount) {
		return {};
	}

	if (type == this->getEntityTypes.at(this->entityTypeCount)) {
		this->entityTypeCount++;
		return this->setResults.at(this->setResultsCount++);
	}
	else {
		return {};
	}
	
}

unordered_map<string, unordered_set<string> > PKBStub::getMapResultsOfRS(
	const RelationshipType& type, shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	if (this->mapResults.size() <= this->mapResultsCount) {
		return {};
	}
	return this->mapResults.at(this->mapResultsCount++);
}

unordered_set<string> PKBStub::getSetResultsOfRS(
	const RelationshipType& type, shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	if (this->setResults.size() <= this->setResultsCount) {
		return {};
	}
	return this->setResults.at(this->setResultsCount++);
}

unordered_map<string, unordered_set<string> > PKBStub::getMapResultsOfAssignPattern(
	shared_ptr<QueryInput> input, Expression& expression) {
	if (this->mapResults.size() <= this->mapResultsCount) {
		return {};
	}
	return this->mapResults.at(this->mapResultsCount++);
}

unordered_set<string> PKBStub::getSetResultsOfAssignPattern(
	shared_ptr<QueryInput> input, Expression& expression) {
	if (this->setResults.size() <= this->setResultsCount) {
		return {};
	}
	return this->setResults.at(this->setResultsCount++);
}

unordered_map<string, unordered_set<string> > PKBStub::getMapResultsOfContainerPattern(
	const EntityType& type, shared_ptr<QueryInput> input) {
	if (this->mapResults.size() <= this->mapResultsCount) {
		return {};
	}
	return this->mapResults.at(this->mapResultsCount++);
}

unordered_set<string> PKBStub::getSetResultsOfContainerPattern(
	const EntityType& type, shared_ptr<QueryInput> input) {
	if (this->setResults.size() <= this->setResultsCount) {
		return {};
	}
	return this->setResults.at(this->setResultsCount++);
}

bool PKBStub::getBooleanResultOfRS(const RelationshipType& type,
	shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	if (this->boolReturnValues.size() <= this->boolCount) {
		return {};
	}
	return this->boolReturnValues.at(this->boolCount++);
}

unordered_map<string, unordered_set<string>> PKBStub::getDeclarationsMatchResults(shared_ptr<Declaration> leftDeclaration,
	shared_ptr<Declaration> rightDeclaration) {
	if (this->mapResults.size() <= this->mapResultsCount) {
		return {};
	}
	return this->mapResults.at(this->mapResultsCount++);
}

unordered_map<string, unordered_set<string>> PKBStub::getDeclarationMatchAttributeResults(
	shared_ptr<Declaration> declaration, EntityType attributeEntityType) {
	if (this->mapResults.size() <= this->mapResultsCount) {
		return {};
	}
	return this->mapResults.at(this->mapResultsCount++);
}

unordered_map<string, unordered_set<string>> PKBStub::getAttributesMatchResults(
	EntityType leftEntityType, EntityType rightEntityType) {
	if (this->mapResults.size() <= this->mapResultsCount) {
		return {};
	}
	return this->mapResults.at(this->mapResultsCount++);
}

unordered_set<string> PKBStub::getAttributeMatchNameResults(EntityType attributeEntityType, shared_ptr<Ident> name) {
	if (this->setResults.size() <= this->setResultsCount) {
		return {};
	}
	return this->setResults.at(this->setResultsCount++);
}

string PKBStub::getNameFromStmtNum(string stmtNum) {
	if (this->stringValues.size() <= this->stringCount) {
		return {};
	}
	return this->stringValues.at(this->stringCount++);
}

// setter methods to set the return values for each different test case
void PKBStub::addBooleanResult(bool value) {
	this->boolReturnValues.push_back(value);
}

void PKBStub::addMapResult(unordered_map<string, unordered_set<string> > value) {
	this->mapResults.push_back(value);
}

void PKBStub::addSetResult(unordered_set<string> value) {
	this->setResults.push_back(value);
}

void PKBStub::addEntityType(EntityType entityType) {
	this->getEntityTypes.push_back(entityType);
}

void PKBStub::addStringResult(string value) {
	this->stringValues.push_back(value);
}