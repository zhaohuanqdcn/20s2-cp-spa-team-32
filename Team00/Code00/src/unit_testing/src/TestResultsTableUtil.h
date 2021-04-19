#pragma once
#include <vector>
#include <string>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include "catch.hpp"
#include "OptionalClause.h"
#include "RelationshipClause.h"
#include "PatternClause.h"
#include "WithClause.h"
#include "Declaration.h"

using namespace std;

class TestResultsTableUtil {
public:
	static void checkTable(vector<vector<string>> actualTable, vector<vector<string>> expectedTable);

	static void checkMap(unordered_map<string, int> actualMap, unordered_map<string, int> expectedMap);

	static void checkList(list<string> actualList, list<string> expectedList);

	static void checkSet(unordered_set<string> actualSet, unordered_set<string> expectedSet);

	static void checkClauseGroups(vector<vector<shared_ptr<OptionalClause>>> actualGroups, 
		vector<vector<shared_ptr<OptionalClause>>> expectedGroups);

	static void checkClauseList(vector<shared_ptr<OptionalClause>> actualList, vector<shared_ptr<OptionalClause>> expectedList);
	
	static void checkQueryInput(shared_ptr<QueryInput> actualQueryInput, shared_ptr<QueryInput> expectedQueryInput);
};