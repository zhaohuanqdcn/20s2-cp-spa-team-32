 #include "ResultUtil.h"
 #include "TestResultsTableUtil.h"
 #include "catch.hpp"
 #include <iostream>

 TEST_CASE("Getting common synonyms between a vector and a list") {
 	string stmtSynonym = "s";
 	string assignSynonym = "a";
 	string varSynonym = "v";
 	string whileSynonym = "w";
    string ifSynonym = "if";
	

 	SECTION("Getting one common synonym from size 2 vector and non empty list") {
 		vector<string> PKBResultSynonyms = { whileSynonym, stmtSynonym };
 		unordered_set<string> currentResultSynonyms = { whileSynonym, ifSynonym, varSynonym, assignSynonym };

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
 		REQUIRE(commonSynonyms.size() == 1);
 		REQUIRE(commonSynonyms.find(whileSynonym) != commonSynonyms.end());
 	}

 	SECTION("Getting two common synonym from size 2 vector and non empty list") {
 		vector<string> PKBResultSynonyms = { whileSynonym, assignSynonym };
 		unordered_set<string> currentResultSynonyms = { whileSynonym, ifSynonym, varSynonym, assignSynonym };

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
 		REQUIRE(commonSynonyms.size() == 2);
 		REQUIRE(commonSynonyms.find(whileSynonym) != commonSynonyms.end());
 		REQUIRE(commonSynonyms.find(assignSynonym) != commonSynonyms.end());
 	}

 	SECTION("Getting one common synonyms from size 1 vector and non empty list") {
 		vector<string> PKBResultSynonyms = { stmtSynonym };
 		unordered_set<string> currentResultSynonyms = { whileSynonym, ifSynonym, stmtSynonym, assignSynonym };

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
 		REQUIRE(commonSynonyms.size() == 1);
 		REQUIRE(commonSynonyms.find(stmtSynonym) != commonSynonyms.end());
 	}

 	SECTION("Getting no common synonyms from size 1 vector and non empty list") {
 		vector<string> PKBResultSynonyms = { varSynonym };
 		unordered_set<string> currentResultSynonyms = { whileSynonym, ifSynonym, stmtSynonym, assignSynonym };

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
 		REQUIRE(commonSynonyms.size() == 0);
 	}

 	SECTION("Getting no common synonyms from size 2 vector and non empty list") {
 		vector<string> PKBResultSynonyms = { varSynonym, whileSynonym };
 		unordered_set<string> currentResultSynonyms = { ifSynonym, stmtSynonym, assignSynonym };

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
 		REQUIRE(commonSynonyms.size() == 0);
 	}

 	SECTION("Getting no common synonyms from empty vector and list") {
 		vector<string> PKBResultSynonyms = {};
 		unordered_set<string> currentResultSynonyms = {};

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
 		REQUIRE(commonSynonyms.size() == 0);
 	}

 	SECTION("Getting no common synonyms from non empty vector and empty list") {
 		vector<string> PKBResultSynonyms = { varSynonym, whileSynonym };
		unordered_set<string> currentResultSynonyms = {};

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
		REQUIRE(commonSynonyms.size() == 0);
	}

 	SECTION("Getting no common synonyms from empty vector and non empty list") {
 		vector<string> PKBResultSynonyms = {};
 		unordered_set<string> currentResultSynonyms = { ifSynonym, stmtSynonym, assignSynonym };

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
 		REQUIRE(commonSynonyms.size() == 0);
 	}
 }

 TEST_CASE("Getting cartesian product from two results") {
 	string stmtSynonym = "s";
 	string assignSynonym = "a";
 	string varSynonym = "v";
 	string whileSynonym = "w";

 	SECTION("Getting cartesian product from PKB Map Result with 2 synonym values") {
 		unordered_map<string, unordered_set<string>> pkbResult = { {"1", {"2", "3"}}, {"4", {"5"}} };
 		vector<vector<string>> table = { {"a", "c"}, {"b", "d"} };
		unordered_map<string, int> indexMap = { {varSynonym, 0}, {whileSynonym, 1} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
 		resultsTable->setTable(indexMap, table);

 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getCartesianProductFromMap(pkbResult, { stmtSynonym, assignSynonym }, resultsTable);
		
		
 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

 		vector<vector<string>> expectedTable = { {"a", "c", "1", "2"}, {"b", "d", "1", "2"}, {"a", "c", "1", "3"},
 			{"b", "d", "1", "3"},{"a", "c", "4", "5"}, {"b", "d", "4", "5"} };
 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {whileSynonym, 1},
 			{stmtSynonym, 2}, {assignSynonym, 3} };

 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
 	}

 	SECTION("Getting cartesian product from PKB Set results with one synonym value") {
		unordered_set<string> pkbResult = { "1", "2", "3", "4" };
 		vector<vector<string>> table = { {"a", "c"}, {"b", "d"} };
		unordered_map<string, int> indexMap = { {varSynonym, 0}, {whileSynonym, 1} };
 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getCartesianProductFromSet(pkbResult, { stmtSynonym }, resultsTable);


 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

 		vector<vector<string>> expectedTable = { {"a", "c", "1"}, {"a", "c", "2"}, {"a", "c", "3"}, {"a", "c", "4"},
 			{"b", "d", "1"}, {"b", "d", "2"}, {"b", "d", "3"}, {"b", "d", "4"} };
		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {whileSynonym, 1},
 			{stmtSynonym, 2} };

 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
 	}

 	SECTION("Getting cartesian product from PKB Set results with two synonym values") {
 		unordered_set<string> pkbResult = { "1", "2", "3", "4" };
 		vector<vector<string>> table = { {"a", "c"}, {"b", "d"} };
 		unordered_map<string, int> indexMap = { {varSynonym, 0}, {whileSynonym, 1} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
 		resultsTable->setTable(indexMap, table);

 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getCartesianProductFromSet(pkbResult, { stmtSynonym, assignSynonym }, resultsTable);


 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

 		vector<vector<string>> expectedTable = { 
 			{"a", "c", "1", "1"}, 
 			{"a", "c", "2", "2"}, 
 			{"a", "c", "3", "3"},
 			{"a", "c", "4", "4"},
 			{"b", "d", "1", "1"},
 			{"b", "d", "2", "2"},
 			{"b", "d", "3", "3"},
 			{"b", "d", "4", "4"} };
 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {whileSynonym, 1},
			{stmtSynonym, 2}, {assignSynonym, 3} };

 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
 	}
 }

 TEST_CASE("Getting natural join from two results") {
 	string stmtSynonym = "s";
 	string assignSynonym = "a";
 	string varSynonym = "v";
 	string whileSynonym = "w";
 	string readSynonym = "r";

 	SECTION("Getting natural join from PKB Set result with 1 synonym") {
 		unordered_set<string> pkbResult = { "1", "2", "3", "4" };
 		vector<vector<string>> table = { {"a", "2", "c"}, {"b", "10", "d"}, {"e", "2", "f"}, {"b", "4", "d"} };
 		unordered_map<string, int> indexMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2} };
 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
 		resultsTable->setTable(indexMap, table);

 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getNaturalJoinFromSet(pkbResult, { stmtSynonym }, resultsTable, { stmtSynonym });


 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

		vector<vector<string>> expectedTable = { {"a", "2", "c"}, {"e", "2", "f"}, {"b", "4", "d"} };
 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2} };

 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
 	}

 	SECTION("Getting natural join from PKB Set result with 2 common synonyms") {
 		unordered_set<string> pkbResult = { "1", "4", "10" };
 		vector<vector<string>> table = { {"a", "1", "c", "10"}, {"b", "10", "d", "10"}, {"e", "4", "f", "4"}, {"b", "4", "d", "12"},
			{"b", "1", "a", "222"}, {"ee", "1", "ff", "1"} };
 		unordered_map<string, int> indexMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2}, {assignSynonym, 3 } };
 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
 		resultsTable->setTable(indexMap, table);

 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getNaturalJoinFromSet(pkbResult, { stmtSynonym, assignSynonym }, resultsTable,
 			{ stmtSynonym, assignSynonym });


 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

 		vector<vector<string>> expectedTable = { {"ee", "1", "ff", "1"}, {"b", "10", "d", "10"}, {"e", "4", "f", "4"} };
 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2}, {assignSynonym, 3 } };

 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
 	}

 	SECTION("Getting natural join from PKB set result with common left synonym and uncommon right synonym") {
 		unordered_set<string> pkbResult = { "1", "4", "10" };
 		vector<vector<string>> table = { {"a", "1", "c", "10"}, {"b", "10", "d", "2"}, {"e", "14", "f", "2"}, {"b", "41", "d", "12"},
 			{"b", "10", "a", "222"}, {"ee", "1", "ff", "2"} };
		unordered_map<string, int> indexMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2}, {assignSynonym, 3 } };
 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
 		resultsTable->setTable(indexMap, table);

 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getNaturalJoinFromSet(pkbResult, { stmtSynonym, readSynonym }, resultsTable,
 			{ stmtSynonym });


 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

 		vector<vector<string>> expectedTable = { {"a", "1", "c", "10", "1"}, {"ee", "1", "ff", "2", "1"} , {"b", "10", "d", "2", "10"},
 			{"b", "10", "a", "222", "10"} };
		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2}, {assignSynonym, 3 },
 			{readSynonym, 4} };

 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
 	}

 	SECTION("Getting natural join from PKB set result with uncommon left synonym and common right synonym") {
 		unordered_set<string> pkbResult = { "1", "4", "10" };
 		vector<vector<string>> table = { {"a", "1", "c", "10"}, {"b", "10", "d", "2"}, {"e", "14", "f", "4"}, {"b", "41", "d", "1"},
 			{"b", "10", "a", "4"}, {"ee", "1", "ff", "2"} };
 		unordered_map<string, int> indexMap = { {varSynonym, 0}, {assignSynonym, 1}, {whileSynonym, 2}, {readSynonym, 3 } };
 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
 		resultsTable->setTable(indexMap, table);

 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getNaturalJoinFromSet(pkbResult, { stmtSynonym, readSynonym }, resultsTable,
 			{ readSynonym });


		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

 		vector<vector<string>> expectedTable = { {"b", "41", "d", "1", "1"}, {"a", "1", "c", "10", "10"} , {"e", "14", "f", "4", "4"}, 
 			{"b", "10", "a", "4", "4"} };
 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {assignSynonym, 1}, {whileSynonym, 2}, {readSynonym, 3 },
 			{stmtSynonym, 4} };

 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
 	}

 	SECTION("Getting natural join from PKB Map result with 2 common synonyms") {
 		unordered_map<string, unordered_set<string>> pkbResult = { {"1", {"2", "3", "12", "10"}}, {"4", {"5", "2", "23", "12"}} };
 		vector<vector<string>> table = { {"a", "1", "c", "10"}, {"b", "10", "d", "2"}, {"e", "4", "f", "2"}, {"b", "4", "d", "12"},
 			{"b", "10", "a", "222"}, {"ee", "1", "ff", "2"} };
 		unordered_map<string, int> indexMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2}, {assignSynonym, 3 } };
 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
 		resultsTable->setTable(indexMap, table);

 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getNaturalJoinFromMap(pkbResult, { stmtSynonym, assignSynonym }, resultsTable,
 			{ stmtSynonym, assignSynonym });


 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

 		vector<vector<string>> expectedTable = { {"ee", "1", "ff", "2"}, {"a", "1", "c", "10"}, {"e", "4", "f", "2"} , 
 			{"b", "4", "d", "12"} };
 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2}, {assignSynonym, 3 } };
		
 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
 	}

 	SECTION("Getting natural join from PKB Map result with common left synonym and uncommon right synonym") {
 		unordered_map<string, unordered_set<string>> pkbResult = { {"1", {"2", "10"}}, {"4", {"5", "2", "23"}}, {"10", {"5", "2", "23"}} };
 		vector<vector<string>> table = { {"a", "1", "c", "10"}, {"b", "10", "d", "2"}, {"e", "14", "f", "2"}, {"b", "41", "d", "12"},
			{"b", "10", "a", "222"}, {"ee", "1", "ff", "2"} };
		unordered_map<string, int> indexMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2}, {assignSynonym, 3 } };
 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
 		resultsTable->setTable(indexMap, table);

 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getNaturalJoinFromMap(pkbResult, { stmtSynonym, readSynonym }, resultsTable,
			{ stmtSynonym });


	vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

 		vector<vector<string>> expectedTable = { 
 			{"b", "10", "d", "2", "5"},
 			{"b", "10", "d", "2", "2"}, 
			{"b", "10", "d", "2", "23"}, 
			
 			{"b", "10", "a", "222", "5"},
 			{"b", "10", "a", "222", "2"}, 
 			{"b", "10", "a", "222", "23"}, 
		
 			{"a", "1", "c", "10", "2"},
 			{"a", "1", "c", "10", "10"}, 

 			{"ee", "1", "ff", "2", "2"},
 			{"ee", "1", "ff", "2", "10"} 
 		};
 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2}, {assignSynonym, 3 }, 
 			{readSynonym, 4} };
		
 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
 	}
	
 	SECTION("Getting natural join from PKB Map result with uncommon left synonym and common right synonym") {
 		unordered_map<string, unordered_set<string>> pkbResult = { {"1", {"2", "10"}}, {"4", {"5", "21", "23"}}, {"10", {"15", "222", "2"}} };
 		vector<vector<string>> table = { {"a", "1", "c", "10"}, {"b", "10", "d", "2"}, {"e", "14", "f", "2"}, {"b", "41", "d", "5"},
 			{"b", "10", "a", "222"}, {"ee", "1", "ff", "2"} };
 		unordered_map<string, int> indexMap = { {varSynonym, 0}, {assignSynonym, 1}, {whileSynonym, 2}, {readSynonym, 3 } };
 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
 		resultsTable->setTable(indexMap, table);

 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getNaturalJoinFromMap(pkbResult, { stmtSynonym, readSynonym }, resultsTable,
 			{ readSynonym });


 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

		vector<vector<string>> expectedTable = { 
 			{"b", "10", "a", "222", "10"},
 			{"b", "10", "d", "2", "10"}, 
 			{"e", "14", "f", "2", "10"}, 
 			{"ee", "1", "ff", "2", "10"},
			 
 			{"b", "10", "d", "2", "1"},
 			{"e", "14", "f", "2", "1"},
 			{"ee", "1", "ff", "2", "1"},
 			{"a", "1", "c", "10", "1"}, 
			{"b", "41", "d", "5", "4"} 
 		};
 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {assignSynonym, 1}, {whileSynonym, 2}, {readSynonym, 3 },
 			{stmtSynonym, 4} };
		
 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
 	}
 }

 TEST_CASE("Getting cartesian product of Tables") {
 	string stmtSynonym = "s";
 	string assignSynonym = "a";
 	string varSynonym = "v";
 	string whileSynonym = "w";
 	string readSynonym = "r";
 	string printSynonym = "pn";


 	SECTION("Non empty results") {
 		unordered_map<string, int> groupResultIndexMap = {
 			{stmtSynonym, 0},
 			{assignSynonym, 1},
 			{varSynonym, 2},
 			{whileSynonym, 3}
 		};
 		vector<vector<string>> groupResultTable = {
 			{ "1", "2", "3", "4" },
 			{ "11", "12", "13", "14" },
 			{ "21", "22", "23", "24" }
 		};
 		shared_ptr<ResultsTable> groupResult = make_shared<ResultsTable>();
 		groupResult->setTable(groupResultIndexMap, groupResultTable);

 		unordered_map<string, int> currentResultIndexMap = {
 			{readSynonym, 0},
 			{printSynonym, 1}
 		};
 		vector<vector<string>> currentResultTable = {
 			{ "3", "4" },
			{ "13", "14" }
 		};
 		shared_ptr<ResultsTable> currentResult = make_shared<ResultsTable>();
 		currentResult->setTable(currentResultIndexMap, currentResultTable);

 		unordered_map<string, int> expectedIndexMap = {
 			{readSynonym, 0},
 			{printSynonym, 1},
 			{stmtSynonym, 2},
 			{assignSynonym, 3},
 			{varSynonym, 4},
 			{whileSynonym, 5}
 		};
 		vector<vector<string>> expectedTable = {
 			{ "3", "4", "1", "2", "3", "4" },
 			{ "3", "4", "11", "12", "13", "14" },
 			{ "3", "4", "21", "22", "23", "24" },
 			{ "13", "14", "1", "2", "3", "4"  },
 			{ "13", "14", "11", "12", "13", "14" },
 			{ "13", "14", "21", "22", "23", "24" }
 		};

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(groupResult->getSynonyms(), currentResult->getSynonyms());
 		unordered_set<string> expectedCommonSynonyms = {};
 		TestResultsTableUtil::checkSet(commonSynonyms, expectedCommonSynonyms);

 		shared_ptr<ResultsTable> mergedResultTable = ResultUtil::getCartesianProductOfTables(groupResult, currentResult);
 		unordered_map<string, int> actualIndexMap = mergedResultTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = mergedResultTable->getTableValues();
		TestResultsTableUtil::checkMap(actualIndexMap, expectedIndexMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
 }

 TEST_CASE("Getting natural join of Tables") {
 	string stmtSynonym = "s"; 
    string assignSynonym = "a";
 	string varSynonym = "v";
 	string whileSynonym = "w";
 	string readSynonym = "r";
 	string printSynonym = "pn";

 	SECTION("All Common Synonyms, non empty results") {
 		unordered_map<string, int> groupResultIndexMap = {
 			{stmtSynonym, 0},
 			{assignSynonym, 1},
 			{varSynonym, 2},
			{whileSynonym, 3}
 		};
 		vector<vector<string>> groupResultTable = {
 			{ "1", "2", "3", "4" }, 
 			{ "11", "12", "13", "14" },
 			{ "21", "22", "23", "24" },
			{ "31", "32", "33", "34" },
 		};
 		shared_ptr<ResultsTable> groupResult = make_shared<ResultsTable>();
 		groupResult->setTable(groupResultIndexMap, groupResultTable);

 		unordered_map<string, int> currentResultIndexMap = {
			{varSynonym, 0},
 			{assignSynonym, 1},
 			{whileSynonym, 2},
 			{stmtSynonym, 3}
 		};
 		vector<vector<string>> currentResultTable = {
 			{ "10", "20", "30", "40" },
 			{ "13", "12", "14", "11" },
 			{ "23", "22", "24", "21" },
 			{ "41", "42", "43", "44" }, 
 		};
 		shared_ptr<ResultsTable> currentResult = make_shared<ResultsTable>();
 		currentResult->setTable(currentResultIndexMap, currentResultTable);

 		unordered_map<string, int> expectedIndexMap = { 
 			{varSynonym, 0},
 			{assignSynonym, 1},
 			{whileSynonym, 2},
 			{stmtSynonym, 3} 
 		};
 		vector<vector<string>> expectedTable = { 
 			{ "13", "12", "14", "11" },
 			{ "23", "22", "24", "21" }
 		};

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(groupResult->getSynonyms(), currentResult->getSynonyms());
 		unordered_set<string> expectedCommonSynonyms = { stmtSynonym, assignSynonym, varSynonym, whileSynonym };
 		TestResultsTableUtil::checkSet(commonSynonyms, expectedCommonSynonyms);

 		shared_ptr<ResultsTable> mergedResultTable = ResultUtil::getNaturalJoinOfTables(groupResult, currentResult, commonSynonyms);
 		unordered_map<string, int> actualIndexMap = mergedResultTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = mergedResultTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualIndexMap, expectedIndexMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	SECTION("All Common Synonyms, empty results") {
 		unordered_map<string, int> groupResultIndexMap = {
 			{stmtSynonym, 0},
 			{assignSynonym, 1},
 			{varSynonym, 2},
 			{whileSynonym, 3}
 		};
 		vector<vector<string>> groupResultTable = {
			{ "1", "2", "3", "4" },
 			{ "11", "12", "13", "14" },
 			{ "21", "22", "23", "24" },
 			{ "31", "32", "33", "34" },
 		};
 		shared_ptr<ResultsTable> groupResult = make_shared<ResultsTable>();
 		groupResult->setTable(groupResultIndexMap, groupResultTable);

 		unordered_map<string, int> currentResultIndexMap = {
 			{varSynonym, 0},
 			{assignSynonym, 1},
 			{whileSynonym, 2},
 			{stmtSynonym, 3}
 		};
 		vector<vector<string>> currentResultTable = {
 			{ "10", "20", "30", "40" },
 			{ "53", "12", "14", "11" },
 			{ "23", "22", "74", "21" },
 			{ "41", "42", "43", "44" },
 		};
 		shared_ptr<ResultsTable> currentResult = make_shared<ResultsTable>();
 		currentResult->setTable(currentResultIndexMap, currentResultTable);

 		unordered_map<string, int> expectedIndexMap = {
 			{varSynonym, 0},
 			{assignSynonym, 1},
 			{whileSynonym, 2},
 			{stmtSynonym, 3}
 		};
 		vector<vector<string>> expectedTable = {};

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(groupResult->getSynonyms(), currentResult->getSynonyms());
		unordered_set<string> expectedCommonSynonyms = { stmtSynonym, assignSynonym, varSynonym, whileSynonym };
 		TestResultsTableUtil::checkSet(commonSynonyms, expectedCommonSynonyms);

 		shared_ptr<ResultsTable> mergedResultTable = ResultUtil::getNaturalJoinOfTables(groupResult, currentResult, commonSynonyms);
 		unordered_map<string, int> actualIndexMap = mergedResultTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = mergedResultTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualIndexMap, expectedIndexMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	SECTION("Some Uncommon Synonyms, non empty results") {
 		unordered_map<string, int> groupResultIndexMap = {
 			{stmtSynonym, 0},
 			{assignSynonym, 1},
 			{varSynonym, 2},
 			{whileSynonym, 3}
 		};
 		vector<vector<string>> groupResultTable = {
 			{ "1", "2", "3", "4" },
			{ "11", "12", "13", "14" },
 			{ "21", "22", "23", "24" },
 			{ "31", "32", "33", "34" },
 		};
 		shared_ptr<ResultsTable> groupResult = make_shared<ResultsTable>();
 		groupResult->setTable(groupResultIndexMap, groupResultTable);

 		unordered_map<string, int> currentResultIndexMap = {
 			{varSynonym, 0},
 			{readSynonym, 1},
 			{printSynonym, 2},
 			{stmtSynonym, 3}
 		};
 		vector<vector<string>> currentResultTable = {
 			{ "10", "20", "30", "40" },
 			{ "13", "2", "4", "11" },
 			{ "23", "222", "244", "21" },
 			{ "41", "42", "43", "44" },
 		};
 		shared_ptr<ResultsTable> currentResult = make_shared<ResultsTable>();
 		currentResult->setTable(currentResultIndexMap, currentResultTable);

 		unordered_map<string, int> expectedIndexMap = {
 			{varSynonym, 0},
 			{readSynonym, 1},
			{printSynonym, 2},
 			{stmtSynonym, 3},
 			{assignSynonym, 4},
 			{whileSynonym, 5}
 		};
 		vector<vector<string>> expectedTable = {
 			{ "13", "2", "4", "11", "12", "14" },
			{ "23", "222", "244", "21", "22", "24" }
 		};

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(groupResult->getSynonyms(), currentResult->getSynonyms());
 		unordered_set<string> expectedCommonSynonyms = { stmtSynonym , varSynonym };
 		TestResultsTableUtil::checkSet(commonSynonyms, expectedCommonSynonyms);

 		shared_ptr<ResultsTable> mergedResultTable = ResultUtil::getNaturalJoinOfTables(groupResult, currentResult, commonSynonyms);
 		unordered_map<string, int> actualIndexMap = mergedResultTable->getSynonymIndexMap();
		vector<vector<string>> actualTable = mergedResultTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualIndexMap, expectedIndexMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	SECTION("Some Uncommon Synonyms, empty results") {
 		unordered_map<string, int> groupResultIndexMap = {
 			{stmtSynonym, 0},
 			{assignSynonym, 1},
 			{varSynonym, 2},
 			{whileSynonym, 3}
 		};
 		vector<vector<string>> groupResultTable = {
 			{ "1", "2", "3", "4" },
 			{ "11", "12", "13", "14" },
 			{ "21", "22", "23", "24" },
 			{ "31", "32", "33", "34" },
 		};
 		shared_ptr<ResultsTable> groupResult = make_shared<ResultsTable>();
 		groupResult->setTable(groupResultIndexMap, groupResultTable);

 		unordered_map<string, int> currentResultIndexMap = {
 			{varSynonym, 0},
 			{readSynonym, 1},
 			{printSynonym, 2},
 			{stmtSynonym, 3}
 		};
 		vector<vector<string>> currentResultTable = {
 			{ "10", "20", "30", "40" },
 			{ "131", "2", "4", "11" },
			{ "23", "222", "244", "212" },
 			{ "41", "42", "43", "44" },
 		};
 		shared_ptr<ResultsTable> currentResult = make_shared<ResultsTable>();
 		currentResult->setTable(currentResultIndexMap, currentResultTable);

 		unordered_map<string, int> expectedIndexMap = {
 			{varSynonym, 0},
 			{readSynonym, 1},
 			{printSynonym, 2},
 			{stmtSynonym, 3},
 			{assignSynonym, 4},
 			{whileSynonym, 5}
 		};
 		vector<vector<string>> expectedTable = {};

 		unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(groupResult->getSynonyms(), currentResult->getSynonyms());
 		unordered_set<string> expectedCommonSynonyms = { stmtSynonym , varSynonym };
 		TestResultsTableUtil::checkSet(commonSynonyms, expectedCommonSynonyms);

 		shared_ptr<ResultsTable> mergedResultTable = ResultUtil::getNaturalJoinOfTables(groupResult, currentResult, commonSynonyms);
 		unordered_map<string, int> actualIndexMap = mergedResultTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = mergedResultTable->getTableValues();
		TestResultsTableUtil::checkMap(actualIndexMap, expectedIndexMap);
		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
 }
