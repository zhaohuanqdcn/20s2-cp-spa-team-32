#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "OptionalClause.h"
#include "ClauseList.h"

using namespace std;

class DisjointClausesSet {
private:
	vector<shared_ptr<OptionalClause>> allClauses;
	unordered_map<string, string> parent;
	unordered_map<string, shared_ptr<ClauseList>> disjointClauses;
	vector<shared_ptr<OptionalClause>> noDeclarationClauses;

	unordered_set<string> extractSynonyms();
	void init(unordered_set<string> syonyms);
	void addClause(shared_ptr<OptionalClause> clause);
	string findRoot(string synonym);

public:
	DisjointClausesSet(vector<shared_ptr<OptionalClause>>);
	vector<vector<shared_ptr<OptionalClause>>> getClauses();
};