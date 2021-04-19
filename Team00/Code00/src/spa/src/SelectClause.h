#pragma once

#include "Declaration.h"
#include <memory>
#include <vector>

using namespace std;

class SelectClause {
private:
	vector<shared_ptr<Declaration>> aDeclarations;
	shared_ptr<Declaration> aDeclaration; // remove after merge with QPP

public:
	SelectClause();
	void addDeclaration(shared_ptr<Declaration> declaration);
	vector<shared_ptr<Declaration>> getDeclarations();
	vector<string> getSynonyms();

	// remove after merge with QPP
	SelectClause(shared_ptr<Declaration> declaration);
	shared_ptr<Declaration> getDeclaration();
};