#include "SelectClause.h"

// remove after merge with QPP
SelectClause::SelectClause(shared_ptr<Declaration> declaration) {
	this->aDeclaration = declaration;
}
// remove after merge with QPP
shared_ptr<Declaration> SelectClause::getDeclaration() {
	return this->aDeclaration;
}

SelectClause::SelectClause() {
	this->aDeclarations = vector<shared_ptr<Declaration>>();;
}

vector<shared_ptr<Declaration>> SelectClause::getDeclarations() {
	return this->aDeclarations;
}

vector<string> SelectClause::getSynonyms() {
	vector<string> synonyms;
	for (vector<shared_ptr<Declaration>>::iterator it = this->aDeclarations.begin(); it != this->aDeclarations.end(); it++) {
		shared_ptr<Declaration> declaration = *it;
		string synonym = declaration->getValue();
		synonyms.push_back(synonym);
	}
	return synonyms;
}

void SelectClause::addDeclaration(shared_ptr<Declaration> declaration) {
	this->aDeclarations.push_back(declaration);
}