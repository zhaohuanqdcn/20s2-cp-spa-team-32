#include "ResultsTable.h"
#include "TestResultsTableUtil.h"
#include "catch.hpp"

TEST_CASE("Populating a results table with PKB results") {
string stmtSynonym = "s";
string assignSynonym = "a";

 	SECTION("Populating a results table with PKB map results of two synonyms") {
 		unordered_map<string, unordered_set<string>> PKBResult = { {"1", {"a", "b", "c"}}, {"2", {"e", "f", "g"}} };
 		vector<string> synonyms = { stmtSynonym, assignSynonym };

		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { assignSynonym, 1 } };
 		vector<vector<string>> expectedTable = { {"1", "a"}, {"1", "b"}, {"1", "c"},
 			{"2", "e"}, {"2","f"}, {"2", "g"} };

 		ResultsTable* resultsTable = new ResultsTable();
 		resultsTable->populateWithMap(PKBResult, synonyms);
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();

 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	SECTION("Populating a results table with PKB set results of one synonym") {
 		unordered_set<string> PKBResult = { "a", "b", "c", "e", "f", "g" };

 		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 } };
 		vector<vector<string>> expectedTable = { {"a"}, {"b"}, {"c"}, {"e"}, {"f"}, {"g"} };

 		ResultsTable* resultsTable = new ResultsTable();
 		resultsTable->populateWithSet(PKBResult, { stmtSynonym });
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();

 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
	}

 	SECTION("Populating a results table with PKB set results of two synonym") {
 		unordered_set<string> PKBResult = { "a", "b", "c", "e", "f", "g" };

 		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { assignSynonym, 1 } };
 		vector<vector<string>> expectedTable = { {"a", "a"}, {"b", "b"}, {"c", "c"}, {"e", "e"}, {"f", "f"}, {"g", "g"} };

 		ResultsTable* resultsTable = new ResultsTable();
 		resultsTable->populateWithSet(PKBResult, { stmtSynonym, assignSynonym });
		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = resultsTable->getTableValues();

 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
 }

