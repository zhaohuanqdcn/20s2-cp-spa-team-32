 #include "QueryEvaluator.h"
 #include "Any.h"
 #include "StmtNum.h"
 #include "Query.h"
 #include "TestResultsTableUtil.h"
 #include "PKBStub.h"
 #include "catch.hpp"
 #include <iostream>

 // Select a
 TEST_CASE("Evaluating query with select clause only, No optional clauses") {
 	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
 	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
 	query->addDeclarationToSelectClause(declaration);
 	pkb->addSetResult({ "1", "2", "3", "4" });
 	QueryEvaluator qe = QueryEvaluator(query, pkb);
 	shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 	unordered_map<string, int> indexMap = resultsTable->getSynonymIndexMap();
 	vector<vector<string>> table = resultsTable->getTableValues();
 	REQUIRE(indexMap.size() == 0);
 	REQUIRE(table.size() == 0);
 }

 TEST_CASE("Evaluating query with only one relationship clause") {
 	string stmtSynonym = "s";
 	string assignSynonym = "a";
 	string progLineSynonym1 = "n1";
 	string progLineSynonym2 = "n2";

 	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
 	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
 	query->addDeclarationToSelectClause(declaration);
	
 	//Select a such that Follows(s, a)
 	SECTION("relationship clause has 2 synonym input, evaluates to non empty results") {
 		shared_ptr<QueryInput> progLine1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROGLINE, progLineSynonym1));
 		shared_ptr<QueryInput> progLine2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROGLINE, progLineSynonym2));
 		query->addRelationshipClause(RelationshipType::NEXT_T, progLine1, progLine2);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbRsResult = { { "1", {"12", "13", "14"} }, { "2", {"22", "23", "24"} } };
 		pkb->addMapResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = { {progLineSynonym1, 0}, {progLineSynonym2, 1} };
 		vector<vector<string>> expectedTable = { {"1", "12"}, {"1", "13"}, {"1", "14"}, {"2", "22"}, {"2", "23"}, {"2", "24"} };
		
 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a such that Follows(s, _)
 	SECTION("relationship clause has 1 synonym and 1 non synonym input, evaluates to non empty results") {
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
 		query->addRelationshipClause(RelationshipType::FOLLOWS, stmt, wildcard);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbRsResult = { "11", "12", "13", "14", "22", "23", "24" };
 		pkb->addSetResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = { {stmtSynonym, 0} };
 		vector<vector<string>> expectedTable = { {"11"}, {"23"}, {"12"}, {"22"}, {"13"}, {"14"}, {"24"} };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a such that Follows(7, 8)
 	SECTION("relationship clause has no synonym input, evaluates to non empty results") {
 		shared_ptr<QueryInput> eight = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("8"));
 		shared_ptr<QueryInput> seven = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("7"));
 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, seven, eight);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> wrongResult = { "11", "12", "13", "14", "22", "23", "24" };
 		pkb->addSetResult(wrongResult);
 		pkb->addBooleanResult(true);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a such that Follows(s, a)
 	SECTION("relationship clause has 2 synonym input, evaluates to empty results") {
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		query->addRelationshipClause(RelationshipType::FOLLOWS, stmt, assign);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbRsResult = {};
 		pkb->addMapResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a such that Follows(s, _)
 	SECTION("relationship clause has 1 synonym and 1 non synonym input, evaluates to empty results") {
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
 		query->addRelationshipClause(RelationshipType::FOLLOWS, stmt, wildcard);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbRsResult = {};
 		pkb->addSetResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a such that Follows*(7, 8)
 	SECTION("relationship clause has no synonym input, evaluates to empty results") {
 		shared_ptr<QueryInput> eight = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("8"));
 		shared_ptr<QueryInput> seven = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("7"));
 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, seven, eight);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> wrongResult = {"11", "12", "13", "14", "22", "23", "24"};
 		pkb->addSetResult(wrongResult);
 		pkb->addBooleanResult(false);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
 }

 TEST_CASE("Evaluating query with only one pattern clause") {
 	string stmtSynonym = "s";
 	string assignSynonym = "a";
 	string varSynonym = "v";
 	string whileSynonym = "w";
 	string ifSynonym = "if";

	
 	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
 	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
 	query->addDeclarationToSelectClause(declaration);
 	shared_ptr<Declaration> assign = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 	shared_ptr<Declaration> synWhile = make_shared<Declaration>(EntityType::WHILE, whileSynonym);
 	shared_ptr<Declaration> synIf = make_shared<Declaration>(EntityType::IF, ifSynonym);

 	//Select a pattern a(v, _x_)
 	SECTION("Assign pattern clause has synonym input, evaluates to non empty results") {
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expression = make_shared<Expression>("x", ExpressionType::EXACT);
 		query->addAssignPatternClause(assign, var, expression);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbRsResult = { { "1", {"12", "13", "14"} }, { "2", {"22", "23", "24"} } };
 		pkb->addMapResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = { {assignSynonym, 0}, {varSynonym, 1} };
 		vector<vector<string>> expectedTable = { {"1", "12"}, {"1", "13"}, {"1", "14"}, {"2", "22"}, {"2", "23"}, {"2", "24"} };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a pattern a(_, _)
 	SECTION("Assign pattern clause has non synonym input, evaluates to non empty results") {
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
 		shared_ptr<Expression> expression = make_shared<Expression>("_", ExpressionType::EMPTY);
 		query->addAssignPatternClause(assign, wildcard, expression);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbRsResult = { "11", "12", "13", "14", "22", "23", "24" };
 		pkb->addSetResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = { {assignSynonym, 0} };
 		vector<vector<string>> expectedTable = { {"11"}, {"23"}, {"12"}, {"22"}, {"13"}, {"14"}, {"24"} };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a pattern a(v, _x_)
 	SECTION("Assign pattern clause has synonym input, evaluates to empty results") {
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expression = make_shared<Expression>("x", ExpressionType::EXACT);
 		query->addAssignPatternClause(assign, var, expression);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbRsResult = {};
 		pkb->addMapResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a pattern a(_, _)
 	SECTION("Assign pattern clause has non synonym input, evaluates to empty results") {
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
 		shared_ptr<Expression> expression = make_shared<Expression>("_", ExpressionType::EMPTY);
 		query->addAssignPatternClause(assign, wildcard, expression);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbRsResult = {};
 		pkb->addSetResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select w pattern w(v, _)
 	SECTION("While pattern clause has synonym input, evaluates to non empty results") {
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addContainerPatternClause(synWhile, var);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbRsResult = { { "1", {"12", "13", "14"} }, { "2", {"22", "23", "24"} } };
 		pkb->addMapResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = { {whileSynonym, 0}, {varSynonym, 1} };
 		vector<vector<string>> expectedTable = { {"1", "12"}, {"1", "13"}, {"1", "14"}, {"2", "22"}, {"2", "23"}, {"2", "24"} };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select w pattern w(_, _)
 	SECTION("While pattern clause has non synonym input, evaluates to non empty results") {
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
 		query->addContainerPatternClause(synWhile, wildcard);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbRsResult = { "11", "12", "13", "14", "22", "23", "24" };
 		pkb->addSetResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = { {whileSynonym, 0} };
 		vector<vector<string>> expectedTable = { {"11"}, {"23"}, {"12"}, {"22"}, {"13"}, {"14"}, {"24"} };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select w pattern w(v, _)
 	SECTION("While pattern clause has synonym input, evaluates to empty results") {
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addContainerPatternClause(synWhile, var);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbRsResult = {};
 		pkb->addMapResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select w pattern w(_, _)
 	SECTION("While pattern clause has non synonym input, evaluates to empty results") {
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
 		query->addContainerPatternClause(synWhile, wildcard);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbRsResult = {};
 		pkb->addSetResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select ifs pattern ifs(v, _, _)
 	SECTION("If pattern clause has synonym input, evaluates to non empty results") {
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addContainerPatternClause(synIf, var);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbRsResult = { { "1", {"12", "13", "14"} }, { "2", {"22", "23", "24"} } };
 		pkb->addMapResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = { {ifSynonym, 0}, {varSynonym, 1} };
 		vector<vector<string>> expectedTable = { {"1", "12"}, {"1", "13"}, {"1", "14"}, {"2", "22"}, {"2", "23"}, {"2", "24"} };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select ifs pattern ifs(_, _)
 	SECTION("If pattern clause has non synonym input, evaluates to non empty results") {
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
 		query->addContainerPatternClause(synIf, wildcard);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbRsResult = { "11", "12", "13", "14", "22", "23", "24" };
 		pkb->addSetResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = { {ifSynonym, 0} };
 		vector<vector<string>> expectedTable = { {"11"}, {"23"}, {"12"}, {"22"}, {"13"}, {"14"}, {"24"} };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select ifs pattern ifs(v, _)
 	SECTION("If pattern clause has synonym input, evaluates to empty results") {
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addContainerPatternClause(synIf, var);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbRsResult = {};
 		pkb->addMapResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select ifs pattern ifs(_, _)
 	SECTION("If pattern clause has non synonym input, evaluates to empty results") {
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>("_"));
 		query->addContainerPatternClause(synIf, wildcard);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbRsResult = {};
 		pkb->addSetResult(pkbRsResult);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
 }

 TEST_CASE("Evaluating query with only one with clause") {
 	string stmtSynonym = "s";
 	string assignSynonym = "a";
 	string progLineSynonym1 = "n1";
 	string progLineSynonym2 = "n2";
 	string printSynonym = "pn";
 	string callSynonym = "c";
 	string readSynonym = "rd";
 	string varSynonym = "v";
 	string procSynonym = "p";
 	string constSynonym = "con";
 	string whileSynonym = "w";

 	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
 	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
 	query->addDeclarationToSelectClause(declaration);

 	//Select a with p = v
 	SECTION("With clause has 2 declarations, procedure = variable, evaluates to non empty results") {
 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addWithClause(proc, var);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = { { "1", {"1"} }, { "2", {"2"} }, { "3", {"3"} }, { "4", {"4"} } };
 		pkb->addMapResult(pkbResult);
 		unordered_map<string, int> expectedMap = { { procSynonym, 0 }, { varSynonym, 1 } };
 		vector<vector<string>> expectedTable = { { "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with v = p
 	SECTION("With clause has 2 declarations, variable = procedure, evaluates to non empty results") {
 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addWithClause(var, proc);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = { { "1", {"1"} }, { "2", {"2"} }, { "3", {"3"} }, { "4", {"4"} } };
 		pkb->addMapResult(pkbResult);
 		unordered_map<string, int> expectedMap = { { varSynonym, 0 }, { procSynonym, 1 } };
 		vector<vector<string>> expectedTable = { { "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with constant = s 
 	SECTION("With clause has 2 declarations, constant = stmt, evaluates to non empty results") {
 		shared_ptr<QueryInput> constant = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		query->addWithClause(constant, stmt);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = { { "1", {"1"} }, { "2", {"2"} }, { "3", {"3"} }, { "4", {"4"} } };
 		pkb->addMapResult(pkbResult);
 		unordered_map<string, int> expectedMap = { { constSynonym, 0 }, { stmtSynonym, 1 } };
 		vector<vector<string>> expectedTable = { { "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
 	//Select a with s = constant 
 	SECTION("With clause has 2 declarations, stmt = constant, evaluates to non empty results") {
 		shared_ptr<QueryInput> constant = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		query->addWithClause(stmt, constant );

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = { { "1", {"1"} }, { "2", {"2"} }, { "3", {"3"} }, { "4", {"4"} } };
 		pkb->addMapResult(pkbResult);
 		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { constSynonym, 1 } };
 		vector<vector<string>> expectedTable = { { "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with p = v
 	SECTION("With clause has 2 declarations, procedure = variable, evaluates to empty results") {
 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addWithClause(proc, var);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = { };
 		pkb->addMapResult(pkbResult);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a with v = p
 	SECTION("With clause has 2 declarations, variable = procedure, evaluates to empty results") {
 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addWithClause(var, proc);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = { };
 		pkb->addMapResult(pkbResult);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a with constant = s 
 	SECTION("With clause has 2 declarations, constant = stmt, evaluates to empty results") {
 		shared_ptr<QueryInput> constant = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		query->addWithClause(constant, stmt);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = {};
 		pkb->addMapResult(pkbResult);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
 	//Select a with s = constant 
 	SECTION("With clause has 2 declarations, stmt = constant, evaluates to empty results") {
 		shared_ptr<QueryInput> constant = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		query->addWithClause(stmt, constant);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = {};
 		pkb->addMapResult(pkbResult);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a with s = a
 	SECTION("With clause has 2 declarations, stmt = <type of stmt>, evaluates to non empty results") {
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		query->addWithClause(stmt, assign);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbResult = { "1", "2", "3", "4" };
 		pkb->addSetResult(pkbResult);
 		pkb->addEntityType(EntityType::ASSIGN);
 		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { assignSynonym, 1 } };
 		vector<vector<string>> expectedTable = { { "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with w = s
 	SECTION("With clause has 2 declarations, <type of stmt> = s, evaluates to non empty results") {
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		query->addWithClause(wh, stmt);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbResult = { "1", "2", "3", "4" };
 		pkb->addSetResult(pkbResult);
 		pkb->addEntityType(EntityType::WHILE);
 		unordered_map<string, int> expectedMap = { { whileSynonym, 0 }, { stmtSynonym, 1 } };
 		vector<vector<string>> expectedTable = { { "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with v = v
 	SECTION("With clause has 2 declarations, both of same type, evaluates to non empty results") {
 		string var2Synonym = varSynonym + "2";

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, var2Synonym));
 		query->addWithClause(var, var2);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbResult = { "a", "b", "c", "d" };
 		pkb->addSetResult(pkbResult);
 		pkb->addEntityType(EntityType::VAR);
 		unordered_map<string, int> expectedMap = { { varSynonym, 0 }, { var2Synonym, 1 } };
 		vector<vector<string>> expectedTable = { { "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a with s = a
 	SECTION("With clause has 2 declarations, stmt = <type of stmt>, evaluates to empty results") {
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		query->addWithClause(stmt, assign);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbResult = {};
 		pkb->addSetResult(pkbResult);
 		pkb->addEntityType(EntityType::ASSIGN);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a with w = s
 	SECTION("With clause has 2 declarations, <type of stmt> = s, evaluates to empty results") {
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		query->addWithClause(wh, stmt);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbResult = { };
 		pkb->addSetResult(pkbResult);
 		pkb->addEntityType(EntityType::WHILE);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a with v = v
 	SECTION("With clause has 2 declarations, both of same type, evaluates to empty results") {
 		string var2Synonym = varSynonym + "2";

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, var2Synonym));
 		query->addWithClause(var, var2);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbResult = {};
 		pkb->addSetResult(pkbResult);
 		pkb->addEntityType(EntityType::VAR);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a with pn.varName = c.procName
 	SECTION("With clause has 2 attributes, evaluates to non empty results") {
 		shared_ptr<Declaration> print = make_shared<Declaration>(EntityType::PRINT, printSynonym);
 		print->setIsAttribute();
 		shared_ptr<Declaration> call = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		call->setIsAttribute();
 		query->addWithClause(dynamic_pointer_cast<QueryInput>(print), dynamic_pointer_cast<QueryInput>(call));

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = { { "1", {"10", "20"} },  { "30", {"5", "6"} } };
 		pkb->addMapResult(pkbResult);
 		unordered_map<string, int> expectedMap = { {printSynonym, 0}, {callSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "1", "10" }, { "1", "20" }, { "30", "5" }, { "30", "6" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with pn.varName = c.procName
 	SECTION("With clause has 2 attributes, evaluates to empty results") {
 		shared_ptr<Declaration> print = make_shared<Declaration>(EntityType::PRINT, printSynonym);
 		print->setIsAttribute();
 		shared_ptr<Declaration> call = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		call->setIsAttribute();
 		query->addWithClause(dynamic_pointer_cast<QueryInput>(print), dynamic_pointer_cast<QueryInput>(call));

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = {};
 		pkb->addMapResult(pkbResult);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select a with rd.varName = v 
 	SECTION("With clause has attribute on LHS and declaration on RHS, evaluates to non empty results") {
 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		read->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addWithClause(dynamic_pointer_cast<QueryInput>(read), var);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = { { "x", {"10", "20"} },  { "count", {"5", "6"} },  { "y", {"1"} } };
 		pkb->addMapResult(pkbResult);
 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {readSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "x", "10" }, { "x", "20" }, { "y", "1" }, { "count", "5" }, { "count", "6" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with v = rd.varName
 	SECTION("With clause has declaration on LHS and attribute on RHS, evaluates to non empty results") {
 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		read->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addWithClause(var, dynamic_pointer_cast<QueryInput>(read));

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = { { "x", {"10", "20"} },  { "count", {"5", "6"} },  { "y", {"1"} } };
 		pkb->addMapResult(pkbResult);
 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {readSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "x", "10" }, { "x", "20" }, { "y", "1" }, { "count", "5" }, { "count", "6" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with rd.varName = v 
 	SECTION("With clause has attribute on LHS and declaration on RHS, evaluates to empty results") {
 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		read->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addWithClause(dynamic_pointer_cast<QueryInput>(read), var);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = {};
 		pkb->addMapResult(pkbResult);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with v = rd.varName
 	SECTION("With clause has declaration on LHS and attribute on RHS, evaluates to empty results") {
 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		read->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addWithClause(var, dynamic_pointer_cast<QueryInput>(read));

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_map<string, unordered_set<string>> pkbResult = {};
 		pkb->addMapResult(pkbResult);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

	
 	//Select a with s = 10
 	SECTION("With clause has declaration on LHS and value on RHS, evaluates to non empty results") {
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> ten = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("10"));
 		query->addWithClause(stmt, ten);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
        unordered_set<string> PKBResults = { "10", "1", "2" };
        pkb->addSetResult(PKBResults);
        pkb->addEntityType(EntityType::STMT);
 		unordered_map<string, int> expectedMap = { {stmtSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "10" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with "x" = v
 	SECTION("With clause has value on LHS and declaration on RHS, evaluates to non empty results") {
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
 		query->addWithClause(ident, var);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
        unordered_set<string> PKBResults = { "x", "y", "z" };
        pkb->addSetResult(PKBResults);
        pkb->addEntityType(EntityType::VAR);
 		unordered_map<string, int> expectedMap = { {varSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "x" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with c.procName = "main"
 	SECTION("With clause has attribute on LHS and value on RHS, evaluates to non empty results") {
 		shared_ptr<Declaration> call = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		call->setIsAttribute();		
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));
 		query->addWithClause(call, ident);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbResult = {"1", "10", "15", "20"};
 		pkb->addSetResult(pkbResult);
 		unordered_map<string, int> expectedMap = { {callSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "10" }, { "1" }, { "15" }, { "20" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with "x" = pn.varName
 	SECTION("With clause has value on LHS and attribute on RHS, evaluates to non empty results") {
 		shared_ptr<Declaration> print = make_shared<Declaration>(EntityType::PRINT, printSynonym);
 		print->setIsAttribute();		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
 		query->addWithClause(ident, print);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbResult = { "1", "10", "15", "20" };
 		pkb->addSetResult(pkbResult);
 		unordered_map<string, int> expectedMap = { {printSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "10" }, { "1" }, { "15" }, { "20" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with c.procName = "main"
 	SECTION("With clause has attribute on LHS and value on RHS, evaluates to empty results") {
 		shared_ptr<Declaration> call = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		call->setIsAttribute();
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));
 		query->addWithClause(call, ident);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbResult = {};
 		pkb->addSetResult(pkbResult);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a with "x" = pn.varName
 	SECTION("With clause has value on LHS and attribute on RHS, evaluates to empty results") {
 		shared_ptr<Declaration> print = make_shared<Declaration>(EntityType::PRINT, printSynonym);
 		print->setIsAttribute();		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
 		query->addWithClause(ident, print);

 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		unordered_set<string> pkbResult = {};
 		pkb->addSetResult(pkbResult);
 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
 }

 TEST_CASE("Evaluating query of one With clause and other clauses") {
 	string stmtSynonym = "s";
 	string assignSynonym = "a";
 	string progLineSynonym1 = "n1";
 	string progLineSynonym2 = "n2";
 	string printSynonym = "pn";
 	string callSynonym = "c";
 	string readSynonym = "rd";
 	string varSynonym = "v";
 	string procSynonym = "p";
 	string constSynonym = "con";
 	string whileSynonym = "w";

 	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
 	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
 	query->addDeclarationToSelectClause(declaration);

 	// Tests for 2 Declarations ===========================================================================================================
	
 	//Select a such that Uses(p, v)  with p = v
 	SECTION("With clause has 2 declaration, proc = var, + 2 common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, proc, var);
 		pkb->addMapResult({ { "main", { "x", "y", "z" } }, { "x", { "x", "a" } }, { "count", { "x", "count" } } });

 		query->addWithClause(proc, var);
 		pkb->addMapResult({ { "x", { "x" } }, { "count", { "count" } } });

 		unordered_map<string, int> expectedMap = { {procSynonym, 0}, {varSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "x", "x" }, { "count", "count" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Uses(p, v)  with v = p
 	SECTION("With clause has 2 declaration, var = proc, + 2 common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, proc, var);
 		pkb->addMapResult({ { "main", { "x", "y", "z" } }, { "x", { "x", "a" } }, { "count", { "x", "count" } } });

 		query->addWithClause(var, proc);
 		pkb->addMapResult({ { "x", { "x" } }, { "count", { "count" } } });

 		unordered_map<string, int> expectedMap = { {procSynonym, 1}, {varSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "x", "x" }, { "count", "count" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Uses(p, v1)  with p = v
 	SECTION("With clause has 2 declaration, proc = var, + 1 common synonym with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		string var1Synonym = varSynonym + "1";

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> var1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, var1Synonym));

 		query->addRelationshipClause(RelationshipType::USES, proc, var1);
 		pkb->addMapResult({ { "main", { "x", "y", "z" } }, { "x", { "x", "a" } }, { "count", { "x", "count" } } });

 		query->addWithClause(proc, var);
 		pkb->addMapResult({ { "x", { "x" } }, { "count", { "count" } } });

 		unordered_map<string, int> expectedMap = { {procSynonym, 0}, {var1Synonym, 2}, {varSynonym, 1} };
 		vector<vector<string>> expectedTable = { 
 			{ "x", "x", "x" },
 			{ "x", "x", "a" },
 			{ "count", "count", "x" },
 			{ "count", "count", "count" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Uses(p, v1)  with v = p
 	SECTION("With clause has 2 declaration, var = proc, + 1 common synonym with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		string var1Synonym = varSynonym + "1";

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> var1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, var1Synonym));

 		query->addRelationshipClause(RelationshipType::USES, proc, var1);
 		pkb->addMapResult({ { "main", { "x", "y", "z" } }, { "x", { "x", "a" } }, { "count", { "x", "count" } } });

 		query->addWithClause(var, proc);
 		pkb->addMapResult({ { "x", { "x" } }, { "count", { "count" } } });

 		unordered_map<string, int> expectedMap = { {procSynonym, 1}, {var1Synonym, 2}, {varSynonym, 0} };
 		vector<vector<string>> expectedTable = {
 			{ "x", "x", "x" },
 			{ "x", "x", "a" },
 			{ "count", "count", "x" },
 			{ "count", "count", "count" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Parent(_, s)  with p = v
 	SECTION("With clause has 2 declaration, proc = var, + no common synonym with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());


 		query->addRelationshipClause(RelationshipType::PARENT, wildcard, s);
 		pkb->addSetResult({ "2", "3" });

 		query->addWithClause(proc, var);
 		pkb->addMapResult({ { "x", { "x" } }, { "count", { "count" } } });

 		unordered_map<string, int> expectedMap = { {stmtSynonym, 0}, {procSynonym, 1}, {varSynonym, 2} };
 		vector<vector<string>> expectedTable = {
 			{ "2", "x", "x" },
 			{ "2", "count", "count" },
 			{ "3", "x", "x" },
 			{ "3", "count", "count" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Parent(_, s)  with v = p
 	SECTION("With clause has 2 declaration, var = proc, + no common synonym with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());


 		query->addRelationshipClause(RelationshipType::PARENT, wildcard, s);
 		pkb->addSetResult({ "2", "3" });

 		query->addWithClause(var, proc);
 		pkb->addMapResult({ { "x", { "x" } }, { "count", { "count" } } });

 		unordered_map<string, int> expectedMap = { {stmtSynonym, 0}, {varSynonym, 1}, {procSynonym, 2} };
 		vector<vector<string>> expectedTable = {
 			{ "2", "x", "x" },
 			{ "2", "count", "count" },
 			{ "3", "x", "x" },
 			{ "3", "count", "count" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Uses(p, v)  with p = v
 	SECTION("With clause has 2 declaration, proc = var, + 2 common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, proc, var);
 		pkb->addMapResult({ { "main", { "x", "y", "z" } }, { "x", { "x1", "a" } }, { "count", { "x", "count1" } } });

 		query->addWithClause(proc, var);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select a such that Uses(p, v)  with v = p
 	SECTION("With clause has 2 declaration, var = proc, + 2 common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, proc, var);
 		pkb->addMapResult({ { "main", { "x", "y", "z" } }, { "x", { "x1", "a" } }, { "count", { "x", "count1" } } });

 		query->addWithClause(var, proc);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select a such that Uses(p, v1)  with p = v
 	SECTION("With clause has 2 declaration, proc = var, + 1 common synonym with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		string var1Synonym = varSynonym + "1";

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> var1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, var1Synonym));

 		query->addRelationshipClause(RelationshipType::USES, proc, var1);
 		pkb->addMapResult({ { "main", { "x", "y", "z" } }, { "x", { "x", "a" } }, { "count", { "x", "count" } } });

 		query->addWithClause(proc, var);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Uses(p, v1)  with v = p
 	SECTION("With clause has 2 declaration, var = proc, + 1 common synonym with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
 		string var1Synonym = varSynonym + "1";

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> var1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, var1Synonym));

 		query->addRelationshipClause(RelationshipType::USES, proc, var1);
 		pkb->addMapResult({ { "main", { "x", "y", "z" } }, { "x", { "x", "a" } }, { "count", { "x", "count" } } });

 		query->addWithClause(var, proc);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Parent(_, s)  with p = v
 	SECTION("With clause has 2 declaration, proc = var, + no common synonym with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());


 		query->addRelationshipClause(RelationshipType::PARENT, wildcard, s);
 		pkb->addSetResult({ "2", "3" });

 		query->addWithClause(proc, var);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Parent(_, s)  with v = p
 	SECTION("With clause has 2 declaration, var = proc, + no common synonym with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());


 		query->addRelationshipClause(RelationshipType::PARENT, wildcard, s);
 		pkb->addSetResult({ "2", "3" });

 		query->addWithClause(var, proc);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 	}

	
 	//Select a such that Uses(a, constant)  with constant = a
 	SECTION("With clause has 2 declaration, constant = <type of stmt>, + 2 common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> con = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		query->addRelationshipClause(RelationshipType::USES, assign, con);
 		pkb->addMapResult({ { "1", { "2", "1" } }, { "4", { "5", "6" } }, { "7", { "7", "8", "9" } } });

 		query->addWithClause(con, assign);
 		pkb->addMapResult({ { "1", { "1" } }, { "7", { "7" } } });

 		unordered_map<string, int> expectedMap = { {assignSynonym, 1}, {constSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "1", "1" }, { "7", "7" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(w, 20) and Parent(_, constant)  with w = constant
 	SECTION("With clause has 2 declaration, <type of stmt> = constant, + 2 common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		shared_ptr<QueryInput> con = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());
 		shared_ptr<QueryInput> twenty = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("20"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS, wh, twenty);
 		pkb->addSetResult({ "1", "5", "15" });

 		query->addRelationshipClause(RelationshipType::PARENT, wildcard, con);
 		pkb->addSetResult({ "15", "3", "4", "5" });

 		query->addWithClause(wh, con);
 		pkb->addMapResult({ { "5", { "5" } }, { "15", { "15" } } });

 		unordered_map<string, int> expectedMap = { {whileSynonym, 0}, {constSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "15", "15" }, { "5", "5" }  };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Uses(a, v)  with constant = a
 	SECTION("With clause has 2 declaration, constant = <type of stmt>, + 1 common synonym with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> con = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		query->addRelationshipClause(RelationshipType::USES, assign, var);
 		pkb->addMapResult({ { "1", { "a", "b" } }, { "4", { "c", "d" } }, { "7", { "e", "f", "g" } } });

 		query->addWithClause(con, assign);
 		pkb->addMapResult({ { "1", { "1" } }, { "7", { "7" } } });

 		unordered_map<string, int> expectedMap = { {assignSynonym, 1}, {varSynonym, 2} , {constSynonym, 0} };
 		vector<vector<string>> expectedTable = { 
 			{ "1", "1", "a" }, 
 			{ "1", "1", "b" },
 			{ "7", "7", "e" },
 			{ "7", "7", "f" },
 			{ "7", "7", "g" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(w, 20) with w = constant
 	SECTION("With clause has 2 declaration, <type of stmt> = constant, + 1 common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		shared_ptr<QueryInput> con = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		shared_ptr<QueryInput> twenty = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("20"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS, wh, twenty);
 		pkb->addSetResult({ "1", "5", "15" });

 		query->addWithClause(wh, con);
 		pkb->addMapResult({ { "5", { "5" } }, { "15", { "15" } } });

 		unordered_map<string, int> expectedMap = { {whileSynonym, 0}, {constSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "5", "5" }, { "15", "15" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Uses(p, v)  with constant = a
 	SECTION("With clause has 2 declaration, constant = <type of stmt>, + no common synonym with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> con = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		query->addRelationshipClause(RelationshipType::USES, proc, var);
 		pkb->addMapResult({ { "main", { "a", "b" } }, { "x", { "c" } } });

 		query->addWithClause(con, assign);
 		pkb->addMapResult({ { "1", { "1" } }, { "7", { "7" } } });

 		unordered_map<string, int> expectedMap = { {procSynonym, 2}, {varSynonym, 3} , {constSynonym, 0}, {assignSynonym, 1} };
 		vector<vector<string>> expectedTable = {
 			{ "1", "1", "main", "a"  },
 			{ "1", "1", "main", "b"  },
 			{ "1", "1", "x", "c" },
 			{ "7", "7", "main", "a" },
 			{ "7", "7", "main", "b" },
 			{ "7", "7", "x", "c" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(s, 3) with w = constant
 	SECTION("With clause has 2 declaration, <type of stmt> = constant, + no common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		shared_ptr<QueryInput> con = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		shared_ptr<QueryInput> three = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("3"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, s, three);
 		pkb->addSetResult({ "1", "2" });

 		query->addWithClause(wh, con);
 		pkb->addMapResult({ { "5", { "5" } }, { "15", { "15" } } });

 		unordered_map<string, int> expectedMap = { {stmtSynonym, 0}, {whileSynonym, 1}, {constSynonym, 2} };
 		vector<vector<string>> expectedTable = { 
 			{ "1", "5", "5" }, 
 			{ "1", "15", "15" },
 			{ "2", "5", "5" },
 			{ "2", "15", "15" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Uses(a, constant)  with constant = a
 	SECTION("With clause has 2 declaration, constant = <type of stmt>, + 2 common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> con = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		query->addRelationshipClause(RelationshipType::USES, assign, con);
 		pkb->addMapResult({ { "1", { "2", "1" } }, { "4", { "5", "6" } }, { "7", { "7", "8", "9" } } });

 		query->addWithClause(con, assign);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select a such that Follows*(w, 20) and Parent(_, constant)  with w = constant
 	SECTION("With clause has 2 declaration, <type of stmt> = constant, + 2 common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		shared_ptr<QueryInput> con = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());
 		shared_ptr<QueryInput> twenty = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("20"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS, wh, twenty);
 		pkb->addSetResult({ "1", "5", "15" });

 		query->addRelationshipClause(RelationshipType::PARENT, wildcard, con);
 		pkb->addSetResult({ "15", "3", "4", "5" });

 		query->addWithClause(wh, con);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Uses(a, v)  with constant = a
 	SECTION("With clause has 2 declaration, constant = <type of stmt>, + 1 common synonym with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> con = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		query->addRelationshipClause(RelationshipType::USES, assign, var);
 		pkb->addMapResult({ { "1", { "a", "b" } }, { "4", { "c", "d" } }, { "7", { "e", "f", "g" } } });

 		query->addWithClause(con, assign);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Follows*(w, 20) with w = constant
 	SECTION("With clause has 2 declaration, <type of stmt> = constant, + 1 common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		shared_ptr<QueryInput> con = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		shared_ptr<QueryInput> twenty = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("20"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS, wh, twenty);
 		pkb->addSetResult({ "1", "5", "15" });

 		query->addWithClause(wh, con);
 		pkb->addMapResult({});

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Uses(p, v)  with constant = a
 	SECTION("With clause has 2 declaration, constant = <type of stmt>, + no common synonym with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> con = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		query->addRelationshipClause(RelationshipType::USES, proc, var);
 		pkb->addMapResult({ { "main", { "a", "b" } }, { "x", { "c" } } });

 		query->addWithClause(con, assign);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Follows*(s, 3) with w = constant
 	SECTION("With clause has 2 declaration, <type of stmt> = constant, + no common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		shared_ptr<QueryInput> con = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CONST, constSynonym));
 		shared_ptr<QueryInput> three = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("3"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, s, three);
 		pkb->addSetResult({ "1", "2" });

 		query->addWithClause(wh, con);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select a such that Uses(p, _) and Modifies(p1, _) with p = p1
 	SECTION("With clause has 2 declaration, both of same type, + 2 common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		string proc1Synonym = procSynonym + "1";

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> proc1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, proc1Synonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());

 		query->addRelationshipClause(RelationshipType::USES, proc, wildcard);
 		pkb->addSetResult({ "a", "b", "c", "d" });

 		query->addRelationshipClause(RelationshipType::MODIFIES, proc1, wildcard);
 		pkb->addSetResult({ "b", "d" });

 		query->addWithClause(proc, proc1);
 		pkb->addSetResult({ "b", "d" });
 		pkb->addEntityType(EntityType::PROC);

 		unordered_map<string, int> expectedMap = { {procSynonym, 0}, {proc1Synonym, 1} };
 		vector<vector<string>> expectedTable = { { "b", "b" }, { "d", "d" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Uses(p, _) with p = p1
 	SECTION("With clause has 2 declaration, both of same type, + 1 common synonym with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		string proc1Synonym = procSynonym + "1";

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> proc1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, proc1Synonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());

 		query->addRelationshipClause(RelationshipType::USES, proc, wildcard);
 		pkb->addSetResult({ "a", "b", "c", "d" });

 		query->addWithClause(proc, proc1);
 		pkb->addSetResult({ "b", "d" });
 		pkb->addEntityType(EntityType::PROC);

 		unordered_map<string, int> expectedMap = { {procSynonym, 0}, {proc1Synonym, 1} };
 		vector<vector<string>> expectedTable = { { "b", "b" }, { "d", "d" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Uses(a, _) with p = p1
 	SECTION("With clause has 2 declaration, both of same type, + no common synonym with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		string proc1Synonym = procSynonym + "1";

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> proc1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, proc1Synonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());


 		query->addRelationshipClause(RelationshipType::USES, assign, wildcard);
 		pkb->addSetResult({ "1", "2", "3" });

 		query->addWithClause(proc, proc1);
 		pkb->addSetResult({ "b", "d" });
 		pkb->addEntityType(EntityType::PROC);

 		unordered_map<string, int> expectedMap = { {assignSynonym, 2}, {procSynonym, 0}, {proc1Synonym, 1} };
 		vector<vector<string>> expectedTable = { 
 			{ "b", "b", "1" },
 			{ "b", "b", "2" },
 			{ "b", "b", "3" },
 			{ "d", "d", "1" },
 			{ "d", "d", "2" },
 			{ "d", "d", "3" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Uses(p, _) and Modifies(p1, _) with p = p1
 	SECTION("With clause has 2 declaration, both of same type, + 2 common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		string proc1Synonym = procSynonym + "1";

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> proc1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, proc1Synonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());

 		query->addRelationshipClause(RelationshipType::USES, proc, wildcard);
 		pkb->addSetResult({ "a", "b", "c", "d" });

 		query->addRelationshipClause(RelationshipType::MODIFIES, proc1, wildcard);
 		pkb->addSetResult({ "b", "d" });

 		query->addWithClause(proc, proc1);
 		pkb->addSetResult({});
 		pkb->addEntityType(EntityType::PROC);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select a such that Uses(p, _) with p = p1
 	SECTION("With clause has 2 declaration, both of same type, + 1 common synonym with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		string proc1Synonym = procSynonym + "1";

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> proc1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, proc1Synonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());

 		query->addRelationshipClause(RelationshipType::USES, proc, wildcard);
 		pkb->addSetResult({ "a", "b", "c", "d" });

 		query->addWithClause(proc, proc1);
 		pkb->addSetResult({ });
 		pkb->addEntityType(EntityType::PROC);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Uses(a, _) with p = p1
 	SECTION("With clause has 2 declaration, both of same type, + no common synonym with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		string proc1Synonym = procSynonym + "1";

 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> proc1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, proc1Synonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());


 		query->addRelationshipClause(RelationshipType::USES, assign, wildcard);
 		pkb->addSetResult({ "1", "2", "3" });

 		query->addWithClause(proc, proc1);
 		pkb->addSetResult({ });
 		pkb->addEntityType(EntityType::PROC);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select a such that Follows*(1, a) and Parent(5, s) with a = s
 	SECTION("With clause has 2 declaration, <type of stmt> = stmt, + 2 common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));
 		shared_ptr<QueryInput> five = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("5"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, one, assign);
 		pkb->addSetResult({ "1",  "7" , "8" });

 		query->addRelationshipClause(RelationshipType::PARENT, five, s);
 		pkb->addSetResult({ "6",  "7" , "8" });

 		query->addWithClause(assign, s);
 		pkb->addSetResult({ "1", "7", "8" });
 		pkb->addEntityType(EntityType::ASSIGN);

 		unordered_map<string, int> expectedMap = { {assignSynonym, 0}, {stmtSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "7", "7" }, { "8", "8" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(1, a) and Parent(5, s) with s = a
 	SECTION("With clause has 2 declaration, stmt = <type of stmt> , + 2 common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));
 		shared_ptr<QueryInput> five = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("5"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, one, assign);
 		pkb->addSetResult({ "1",  "7" , "8" });

 		query->addRelationshipClause(RelationshipType::PARENT, five, s);
 		pkb->addSetResult({ "6",  "7" , "8" });

 		query->addWithClause(s, assign);
 		pkb->addSetResult({ "1", "7", "8" });
 		pkb->addEntityType(EntityType::ASSIGN);

 		unordered_map<string, int> expectedMap = { {assignSynonym, 1}, {stmtSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "7", "7" }, { "8", "8" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(1, a) with a = s
 	SECTION("With clause has 2 declaration, <type of stmt> = stmt, + 1 common synonym with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, one, assign);
 		pkb->addSetResult({ "1",  "7" , "8" });

 		query->addWithClause(assign, s);
 		pkb->addSetResult({ "1", "7", "8" });
 		pkb->addEntityType(EntityType::ASSIGN);

 		unordered_map<string, int> expectedMap = { {assignSynonym, 0}, {stmtSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "1", "1" } , { "7", "7" }, { "8", "8" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Parent(5, s) with s = a
 	SECTION("With clause has 2 declaration, stmt = <type of stmt> , + 1 common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> five = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("5"));

 		query->addRelationshipClause(RelationshipType::PARENT, five, s);
 		pkb->addSetResult({ "6",  "7" , "8" });

 		query->addWithClause(s, assign);
 		pkb->addSetResult({ "1", "7", "8" });
 		pkb->addEntityType(EntityType::ASSIGN);

 		unordered_map<string, int> expectedMap = { {stmtSynonym, 0}, {assignSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "7", "7" }, { "8", "8" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(1, w) with a = s
 	SECTION("With clause has 2 declaration, <type of stmt> = stmt, + no common synonym with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, one, wh);
 		pkb->addSetResult({ "2" });

 		query->addWithClause(assign, s);
 		pkb->addSetResult({ "1", "7", "8" });
 		pkb->addEntityType(EntityType::ASSIGN);

 		unordered_map<string, int> expectedMap = { {whileSynonym, 0} , {assignSynonym, 1}, {stmtSynonym, 2} };
 		vector<vector<string>> expectedTable = { 
 			{ "2", "1", "1" },
 			{ "2", "7", "7" }, 
 			{ "2", "8", "8" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Parent(5, w) with s = a
 	SECTION("With clause has 2 declaration, stmt = <type of stmt> , + no common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		shared_ptr<QueryInput> five = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("5"));

 		query->addRelationshipClause(RelationshipType::PARENT, five, wh);
 		pkb->addSetResult({ "2" });

 		query->addWithClause(s, assign);
 		pkb->addSetResult({ "1", "7", "8" });
 		pkb->addEntityType(EntityType::ASSIGN);

 		unordered_map<string, int> expectedMap = { {whileSynonym, 0} , {stmtSynonym, 1}, {assignSynonym, 2} };
 		vector<vector<string>> expectedTable = {
 			{ "2", "1", "1" },
 			{ "2", "7", "7" },
 			{ "2", "8", "8" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(1, a) and Parent(5, s) with a = s
 	SECTION("With clause has 2 declaration, <type of stmt> = stmt, + 2 common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));
 		shared_ptr<QueryInput> five = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("5"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, one, assign);
 		pkb->addSetResult({ "1",  "7" , "8" });

 		query->addRelationshipClause(RelationshipType::PARENT, five, s);
 		pkb->addSetResult({ "6",  "7" , "8" });

 		query->addWithClause(assign, s);
 		pkb->addSetResult({ });
 		pkb->addEntityType(EntityType::ASSIGN);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select a such that Follows*(1, a) and Parent(5, s) with s = a
 	SECTION("With clause has 2 declaration, stmt = <type of stmt> , + 2 common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));
 		shared_ptr<QueryInput> five = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("5"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, one, assign);
 		pkb->addSetResult({ "1",  "7" , "8" });

 		query->addRelationshipClause(RelationshipType::PARENT, five, s);
 		pkb->addSetResult({ "6",  "7" , "8" });

 		query->addWithClause(s, assign);
 		pkb->addSetResult({ });
 		pkb->addEntityType(EntityType::ASSIGN);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Follows*(1, a) with a = s
 	SECTION("With clause has 2 declaration, <type of stmt> = stmt, + 1 common synonym with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, one, assign);
 		pkb->addSetResult({ "1",  "7" , "8" });

 		query->addWithClause(assign, s);
 		pkb->addSetResult({  });
 		pkb->addEntityType(EntityType::ASSIGN);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Parent(5, s) with s = a
 	SECTION("With clause has 2 declaration, stmt = <type of stmt> , + 1 common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> five = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("5"));

 		query->addRelationshipClause(RelationshipType::PARENT, five, s);
 		pkb->addSetResult({ "6",  "7" , "8" });

 		query->addWithClause(s, assign);
 		pkb->addSetResult({  });
 		pkb->addEntityType(EntityType::ASSIGN);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Follows*(1, w) with a = s
 	SECTION("With clause has 2 declaration, <type of stmt> = stmt, + no common synonym with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, one, wh);
 		pkb->addSetResult({ "2" });

 		query->addWithClause(assign, s);
 		pkb->addSetResult({ });
 		pkb->addEntityType(EntityType::ASSIGN);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Parent(5, w) with s = a
 	SECTION("With clause has 2 declaration, stmt = <type of stmt> , + no common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> s = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		shared_ptr<QueryInput> five = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("5"));

 		query->addRelationshipClause(RelationshipType::PARENT, five, wh);
 		pkb->addSetResult({ "2" });

 		query->addWithClause(s, assign);
 		pkb->addSetResult({ });
 		pkb->addEntityType(EntityType::ASSIGN);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	// Tests for 2 attributes ===========================================================================================================
	
 	//Select a such that Follows(rd, pn) with rd.varName = pn.varName
 	SECTION("With clause has 2 attribute input + 2 common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> read = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::READ, readSynonym));
 		shared_ptr<QueryInput> print = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PRINT, printSynonym));
 		query->addRelationshipClause(RelationshipType::FOLLOWS, read, print);
 		pkb->addMapResult({ { "1", {"2"} }, { "3", {"4"} }, { "5", {"6"} } });

 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<Declaration> printAttr = make_shared<Declaration>(EntityType::PRINT, printSynonym);
 		printAttr->setIsAttribute();
 		query->addWithClause(dynamic_pointer_cast<QueryInput>(readAttr), dynamic_pointer_cast<QueryInput>(printAttr));
 		pkb->addMapResult({ { "3", {"4"} }, { "11", {"14"} }, { "1", {"16"} } });

 		unordered_map<string, int> expectedMap = { {readSynonym, 0}, {printSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "3", "4" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows(rd, pn) with rd.varName = pn.varName
 	SECTION("With clause has 2 attribute input + 2 common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> read = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::READ, readSynonym));
 		shared_ptr<QueryInput> print = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PRINT, printSynonym));
 		query->addRelationshipClause(RelationshipType::FOLLOWS, read, print);
 		pkb->addMapResult({ { "1", {"2"} }, { "3", {"4"} }, { "5", {"6"} } });

 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> printAttr = make_shared<Declaration>(EntityType::PRINT, printSynonym);
 		query->addWithClause(dynamic_pointer_cast<QueryInput>(readAttr), dynamic_pointer_cast<QueryInput>(printAttr));
 		pkb->addMapResult({ { "13", {"4"} }, { "11", {"14"} }, { "1", {"16"} } });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select a such that Uses(c, v) with c.procName = rd.varName
 	SECTION("With clause has 2 attributes input + 1 common synonym with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> call = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CALL, callSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, call, var);
 		pkb->addMapResult({ { "1", {"main"} }, { "3", {"megatron"} }, { "5", {"bumblebee"} } });

 		shared_ptr<Declaration> callAttr = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		callAttr->setIsAttribute();
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		query->addWithClause(dynamic_pointer_cast<QueryInput>(callAttr), dynamic_pointer_cast<QueryInput>(readAttr));
 		pkb->addMapResult({ { "1", { "11", "12" } }, { "2", { "7" } }, {"5", { "9" } } });

 		unordered_map<string, int> expectedMap = { {callSynonym, 0}, {varSynonym, 1}, {readSynonym, 2} };
 		vector<vector<string>> expectedTable = { 
 			{ "1", "main", "11" }, 
 			{ "1", "main", "12" },
 			{ "5", "bumblebee", "9" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Uses(c, v) with c.procName = rd.varName
 	SECTION("With clause has 2 attributes input + 1 common synonym with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> call = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::CALL, callSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, call, var);
 		pkb->addMapResult({ { "1", {"main"} }, { "3", {"megatron"} }, { "5", {"bumblebee"} } });

 		shared_ptr<Declaration> callAttr = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		callAttr->setIsAttribute();
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		query->addWithClause(dynamic_pointer_cast<QueryInput>(callAttr), dynamic_pointer_cast<QueryInput>(readAttr));
 		pkb->addMapResult({ { "10", { "11", "12" } }, { "2", { "7" } }, {"15", { "9" } } });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select a such that Modified(rd, v) with pn.varName = c.procName
 	SECTION("With clause has 2 attribute input + no common synonyms with results table, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> read = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::READ, readSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, read, var);
 		pkb->addMapResult({ { "11", {"x"} }, { "22", {"y"} }, { "33", {"z"} } });

 		shared_ptr<Declaration> printAttr = make_shared<Declaration>(EntityType::PRINT, printSynonym);
 		printAttr->setIsAttribute();
 		shared_ptr<Declaration> callAtrr = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		callAtrr->setIsAttribute();
 		query->addWithClause(dynamic_pointer_cast<QueryInput>(printAttr), dynamic_pointer_cast<QueryInput>(callAtrr));
 		pkb->addMapResult({ { "1", { "1" } }, { "2", { "2" } }, { "3", { "3" } } });

 		unordered_map<string, int> expectedMap = { {readSynonym, 2}, {varSynonym, 3} , {printSynonym, 0}, {callSynonym, 1} };
 		vector<vector<string>> expectedTable = {
 			{ "1", "1", "11", "x" },
 			{ "1", "1", "22", "y" },
 			{ "1", "1", "33", "z" },
 			{ "2", "2", "11", "x" },
 			{ "2", "2", "22", "y" },
 			{ "2", "2", "33", "z" },
 			{ "3", "3", "11", "x" },
 			{ "3", "3",  "22", "y" },
 			{ "3", "3", "33", "z" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Modified(rd, v) with pn.varName = c.procName
 	SECTION("With clause has 2 attributes input + no common synonyms with results table, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> read = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::READ, readSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, read, var);
 		pkb->addMapResult({ { "11", {"x"} }, { "22", {"y"} }, { "33", {"z"} } });

 		shared_ptr<Declaration> printAttr = make_shared<Declaration>(EntityType::PRINT, printSynonym);
 		printAttr->setIsAttribute();
 		shared_ptr<Declaration> callAtrr = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		callAtrr->setIsAttribute();
 		query->addWithClause(dynamic_pointer_cast<QueryInput>(printAttr), dynamic_pointer_cast<QueryInput>(callAtrr));
 		pkb->addMapResult({});

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}


 	// Tests for attribute + declaration ===========================================================================================================

 	//Select a Modifies(rd, v) with rd.varName = v 
 	SECTION("With clause has attribute on LHS and declaration on RHS + 2 common synonyms, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));

 		query->addRelationshipClause(RelationshipType::MODIFIES, read, var);
 		pkb->addMapResult({ { "1",  { "x" } }, { "2",  { "y" } }, { "3",  { "z" } } });


 		query->addWithClause(dynamic_pointer_cast<QueryInput>(readAttr), var);
 		pkb->addMapResult({ { "x",  { "1" } }, { "y",  { "2" } }, { "z",  { "3" } } });


 		unordered_map<string, int> expectedMap = { {readSynonym, 0}, {varSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "1", "x" }, { "2", "y" }, { "3", "z" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a Modifies(rd, _) with rd.varName = v 
 	SECTION("With clause has attribute on LHS and declaration on RHS + 1 common synonym, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());

 		query->addRelationshipClause(RelationshipType::MODIFIES, read, wildcard);
 		pkb->addSetResult({ "1", "2", "3" });

 		query->addWithClause(dynamic_pointer_cast<QueryInput>(readAttr), var);
 		pkb->addMapResult({ { "x",  { "1" } }, { "y",  { "2" } }, { "z",  { "3" } } });


 		unordered_map<string, int> expectedMap = { {readSynonym, 0}, {varSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "1", "x" }, { "2", "y" }, { "3", "z" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a pattern a(_, _) with rd.varName = v 
 	SECTION("With clause has attribute on LHS and declaration on RHS + no common synonyms, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> assign = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());
 		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);

 		query->addAssignPatternClause(assign, wildcard, expr);
 		pkb->addSetResult({ "3", "4" });

 		query->addWithClause(dynamic_pointer_cast<QueryInput>(readAttr), var);
 		pkb->addMapResult({ { "x",  { "1" } }, { "y",  { "2" } } });


 		unordered_map<string, int> expectedMap = { {assignSynonym, 0}, {varSynonym, 1}, {readSynonym, 2} };
 		vector<vector<string>> expectedTable = { 
 			{ "3", "x", "1" },
 			{ "3", "y", "2" },
 			{ "4", "x", "1" },
 			{ "4", "y", "2" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
		
 	}
	
 	//Select a Modifies(rd, v) with rd.varName = v 
 	SECTION("With clause has attribute on LHS and declaration on RHS + 2 common synonyms, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));

 		query->addRelationshipClause(RelationshipType::MODIFIES, read, var);
 		pkb->addMapResult({ { "1",  { "x" } }, { "2",  { "y" } }, { "3",  { "z" } } });


 		query->addWithClause(dynamic_pointer_cast<QueryInput>(readAttr), var);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a Modifies(rd, _) with rd.varName = v 
 	SECTION("With clause has attribute on LHS and declaration on RHS + 1 common synonym, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());

 		query->addRelationshipClause(RelationshipType::MODIFIES, read, wildcard);
 		pkb->addSetResult({ "1", "2", "3" });

 		query->addWithClause(dynamic_pointer_cast<QueryInput>(readAttr), var);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a pattern a(_, _) with rd.varName = v 
 	SECTION("With clause has attribute on LHS and declaration on RHS + no common synonyms, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> assign = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());
 		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);

 		query->addAssignPatternClause(assign, wildcard, expr);
 		pkb->addSetResult({ "3", "4" });

 		query->addWithClause(dynamic_pointer_cast<QueryInput>(readAttr), var);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());

 	}
	
 	//Select a Modifies(rd, v) with v = rd.varName 
 	SECTION("With clause has declaration on LHS and attribute on RHS + 2 common synonyms, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));

 		query->addRelationshipClause(RelationshipType::MODIFIES, read, var);
 		pkb->addMapResult({ { "1",  { "x" } }, { "2",  { "y" } }, { "3",  { "z" } } });


 		query->addWithClause(var, dynamic_pointer_cast<QueryInput>(readAttr));
 		pkb->addMapResult({ { "x",  { "1" } }, { "y",  { "2" } }, { "z",  { "3" } } });


 		unordered_map<string, int> expectedMap = { {readSynonym, 0}, {varSynonym, 1} };
 		vector<vector<string>> expectedTable = { { "1", "x" }, { "2", "y" }, { "3", "z" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a Modifies(rd, _) with v = rd.varName
 	SECTION("With clause has declaration on LHS and attribute on RHS + 1 common synonym, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());

 		query->addRelationshipClause(RelationshipType::MODIFIES, read, wildcard);
 		pkb->addSetResult({ "1", "2", "3" });

 		query->addWithClause(var, dynamic_pointer_cast<QueryInput>(readAttr));
 		pkb->addMapResult({ { "x",  { "1" } }, { "y",  { "2" } }, { "z",  { "3" } } });


 		unordered_map<string, int> expectedMap = { {readSynonym, 1}, {varSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "x", "1" }, { "y", "2" }, { "z", "3" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a pattern a(_, _) with v = rd.varName
 	SECTION("With clause has declaration on LHS and attribute on RHS + no common synonyms, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> assign = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());
 		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);

 		query->addAssignPatternClause(assign, wildcard, expr);
 		pkb->addSetResult({ "3", "4" });

 		query->addWithClause(var, dynamic_pointer_cast<QueryInput>(readAttr));
 		pkb->addMapResult({ { "x",  { "1" } }, { "y",  { "2" } } });


 		unordered_map<string, int> expectedMap = { {assignSynonym, 0}, {varSynonym, 1}, {readSynonym, 2} };
 		vector<vector<string>> expectedTable = {
 			{ "3", "x", "1" },
 			{ "3", "y", "2" },
 			{ "4", "x", "1" },
 			{ "4", "y", "2" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);

 	}
	
 	//Select a Modifies(rd, v) with v = rd.varName 
 	SECTION("With clause has declaration on LHS and attribute on RHS + 2 common synonyms, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));

 		query->addRelationshipClause(RelationshipType::MODIFIES, read, var);
 		pkb->addMapResult({ { "1",  { "x" } }, { "2",  { "y" } }, { "3",  { "z" } } });


 		query->addWithClause(dynamic_pointer_cast<QueryInput>(readAttr), var);
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a Modifies(rd, _) with v = rd.varName
 	SECTION("With clause has declaration on LHS and attribute on RHS + 1 common synonym, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());

 		query->addRelationshipClause(RelationshipType::MODIFIES, read, wildcard);
 		pkb->addSetResult({ "1", "2", "3" });

 		query->addWithClause(var, dynamic_pointer_cast<QueryInput>(readAttr));
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a pattern a(_, _) with v = rd.varName 
 	SECTION("With clause has declaration on LHS and attribute on RHS + no common synonyms, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> read = make_shared<Declaration>(EntityType::READ, readSynonym);
 		shared_ptr<Declaration> readAttr = make_shared<Declaration>(EntityType::READ, readSynonym);
 		readAttr->setIsAttribute();
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> assign = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());
 		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);

 		query->addAssignPatternClause(assign, wildcard, expr);
 		pkb->addSetResult({ "3", "4" });

 		query->addWithClause(var, dynamic_pointer_cast<QueryInput>(readAttr));
 		pkb->addMapResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());

 	}


 	// Tests for attribute/declaration + value ===========================================================================================================

 	//Select a such that Follows*(s, 6) with s = 5
 	SECTION("With clause has declaration on LHS and value on RHS + common synonym, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> five = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("5"));
 		shared_ptr<QueryInput> six = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("6"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, stmt, six);
 		pkb->addSetResult({ "1", "2", "3", "4", "5" });

 		query->addWithClause(stmt, five);
        pkb->addSetResult({ "10", "1", "2", "3", "4", "5" });
        pkb->addEntityType(EntityType::STMT);

 		unordered_map<string, int> expectedMap = { {stmtSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "5" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(a, 3) with s = 5
 	SECTION("With clause has declaration on LHS and value on RHS + no common synonym, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> five = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("5"));
 		shared_ptr<QueryInput> three = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("3"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, assign, three);
 		pkb->addSetResult({ "1", "2" });

 		query->addWithClause(stmt, five);
        pkb->addSetResult({ "10", "1", "2", "3", "4", "5" });
        pkb->addEntityType(EntityType::STMT);

 		unordered_map<string, int> expectedMap = { {assignSynonym, 1}, {stmtSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "5", "1" }, { "5", "2" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(s, 1) with s = 5
 	SECTION("With clause has declaration on LHS and value on RHS + common synonym, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> five = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("5"));
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, stmt, one);
 		pkb->addSetResult({ });

 		query->addWithClause(stmt, five);
        pkb->addSetResult({ "10", "1", "2", "3", "4", "5" });
        pkb->addEntityType(EntityType::STMT);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Follows*(a, 3) with s = 5
 	SECTION("With clause has declaration on LHS and value on RHS + no common synonym, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> five = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("5"));
 		shared_ptr<QueryInput> three = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("3"));

 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, assign, three);
 		pkb->addSetResult({ });

 		query->addWithClause(stmt, five);
        pkb->addSetResult({ "10", "1", "2", "3", "4", "5" });
        pkb->addEntityType(EntityType::STMT);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

	
 	//Select a such that Calls(p, _) with "x" = p
 	SECTION("With clause has value on LHS and declaration on RHS + common synonym, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());
 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
 		query->addRelationshipClause(RelationshipType::CALLS, proc, wildcard);
 		pkb->addSetResult({ "main", "x", "y" });

 		query->addWithClause(ident, proc);
        pkb->addSetResult({ "e", "x", "a", "b", "v", "d" });
        pkb->addEntityType(EntityType::PROC);

 		unordered_map<string, int> expectedMap = { {procSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "x" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Parent(_, s) with "x" = p
 	SECTION("With clause has value on LHS and declaration on RHS + no common synonym, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
 		query->addRelationshipClause(RelationshipType::PARENT, wildcard, stmt);
 		pkb->addSetResult({ "1", "2", "3" });

 		query->addWithClause(ident, proc);
        pkb->addSetResult({ "e", "x", "a", "b", "v", "d" });
        pkb->addEntityType(EntityType::PROC);

 		unordered_map<string, int> expectedMap = { {stmtSynonym, 1}, {procSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "x", "1" }, { "x", "2" }, { "x", "3" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Calls(p, _) with "x" = p
 	SECTION("With clause has value on LHS and declaration on RHS + common synonym, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());
 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
 		query->addRelationshipClause(RelationshipType::CALLS, proc, wildcard);
 		pkb->addSetResult({ "main", "x1", "y" });

 		query->addWithClause(ident, proc);
        pkb->addSetResult({ "e", "x", "a", "b", "v", "d" });
        pkb->addEntityType(EntityType::PROC);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Parent(_, s) with "x" = p
 	SECTION("With clause has value on LHS and declaration on RHS + no common synonym, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> wildcard = dynamic_pointer_cast<QueryInput>(make_shared<Any>());
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> proc = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, procSynonym));
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));
 		query->addRelationshipClause(RelationshipType::PARENT, wildcard, stmt);
 		pkb->addSetResult({ });

 		query->addWithClause(ident, proc);
        pkb->addSetResult({ "e", "x", "a", "b", "v", "d" });
        pkb->addEntityType(EntityType::PROC);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select a such that Follows*(1, c) with c.procName = "main"
 	SECTION("With clause has attribute on LHS and value on RHS + common synonym, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> call = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));
 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, one, call);
 		pkb->addSetResult({ "1", "10", "11", "15", "20" });

 		shared_ptr<Declaration> callAttr = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		callAttr->setIsAttribute();
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));
 		query->addWithClause(callAttr, ident);
 		pkb->addSetResult({ "1", "10", "15", "20" });

 		unordered_map<string, int> expectedMap = { {callSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "10" }, { "1" }, { "20" }, { "15" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(s, 5) with c.procName = "main"
 	SECTION("With clause has attribute on LHS and value on RHS + no common synonym, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> stmt = make_shared<Declaration>(EntityType::STMT, stmtSynonym);
 		shared_ptr<QueryInput> three = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("3"));
 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, stmt, three);
 		pkb->addSetResult({ "1", "2" });

 		shared_ptr<Declaration> callAttr = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		callAttr->setIsAttribute();
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));
 		query->addWithClause(callAttr, ident);
 		pkb->addSetResult({ "5", "6" });

 		unordered_map<string, int> expectedMap = { {stmtSynonym, 1}, {callSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "5", "1" }, { "5", "2" }, { "6", "1" }, { "6", "2" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(1, c) with c.procName = "main"
 	SECTION("With clause has attribute on LHS and value on RHS + common synonym, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> call = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));
 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, one, call);
 		pkb->addSetResult({ "1", "10", "11", "15", "20" });

 		shared_ptr<Declaration> callAttr = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		callAttr->setIsAttribute();
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));
 		query->addWithClause(callAttr, ident);
 		pkb->addSetResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Follows*(s, 5) with c.procName = "main"
 	SECTION("With clause has attribute on LHS and value on RHS + no common synonym, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> stmt = make_shared<Declaration>(EntityType::STMT, stmtSynonym);
 		shared_ptr<QueryInput> three = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("3"));
 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, stmt, three);
 		pkb->addSetResult({ "1", "2" });

 		shared_ptr<Declaration> callAttr = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		callAttr->setIsAttribute();
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));
 		query->addWithClause(callAttr, ident);
 		pkb->addSetResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select a such that Follows*(1, c) with "main" = c.procName
 	SECTION("With clause has value on LHS and attribute on RHS + common synonym, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> call = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));
 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, one, call);
 		pkb->addSetResult({ "1", "10", "11", "15", "20" });

 		shared_ptr<Declaration> callAttr = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		callAttr->setIsAttribute();
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));
 		query->addWithClause(ident, callAttr);
 		pkb->addSetResult({ "1", "10", "15", "20" });

 		unordered_map<string, int> expectedMap = { {callSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "10" }, { "1" }, { "20" }, { "15" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(s, 5) with "main" = c.procName
 	SECTION("With clause has value on LHS and attribute on RHS + no common synonym, evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> stmt = make_shared<Declaration>(EntityType::STMT, stmtSynonym);
 		shared_ptr<QueryInput> three = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("3"));
 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, stmt, three);
 		pkb->addSetResult({ "1", "2" });

 		shared_ptr<Declaration> callAttr = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		callAttr->setIsAttribute();
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));
 		query->addWithClause(ident, callAttr);
 		pkb->addSetResult({ "5", "6" });

 		unordered_map<string, int> expectedMap = { {stmtSynonym, 1}, {callSynonym, 0} };
 		vector<vector<string>> expectedTable = { { "5", "1" }, { "5", "2" }, { "6", "1" }, { "6", "2" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Follows*(1, c) with "main" = c.procName
 	SECTION("With clause has value on LHS and attribute on RHS + common synonym, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> call = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));
 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, one, call);
 		pkb->addSetResult({ "1", "10", "11", "15", "20" });

 		shared_ptr<Declaration> callAttr = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		callAttr->setIsAttribute();
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));
 		query->addWithClause(ident, callAttr);
 		pkb->addSetResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Follows*(s, 5) with "main" = c.procName
 	SECTION("With clause has value on LHS and attribute on RHS + no common synonym, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<Declaration> stmt = make_shared<Declaration>(EntityType::STMT, stmtSynonym);
 		shared_ptr<QueryInput> three = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("3"));
 		query->addRelationshipClause(RelationshipType::FOLLOWS_T, stmt, three);
 		pkb->addSetResult({ "1", "2" });

 		shared_ptr<Declaration> callAttr = make_shared<Declaration>(EntityType::CALL, callSynonym);
 		callAttr->setIsAttribute();
 		shared_ptr<QueryInput> ident = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));
 		query->addWithClause(ident, callAttr);
 		pkb->addSetResult({ });

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
 }


 TEST_CASE("Evaluating query with both such that and pattern clause") {
 	string stmtSynonym = "s";
 	string assignSynonym = "a";
 	string varSynonym = "v";
 	string whileSynonym = "w";
 	string ifSynonym = "if";

 	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
 	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
 	query->addDeclarationToSelectClause(declaration);
 	shared_ptr<Declaration> assign = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);

 	//Select a such that Modifies(a, v) pattern a(v, _x_)
 	SECTION("such that and assign pattern clauses have 2 common synonyms, all evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expr = make_shared<Expression>("x", ExpressionType::EXACT);
 		shared_ptr<Declaration> assign2 = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		query->addAssignPatternClause(assign2, var2, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = { { assignSynonym, 0 }, { varSynonym, 1 } };
 		vector<vector<string>> expectedTable = { { "1", "u" }, { "3", "count" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a such that Uses(s, v) pattern a(v, _)
 	SECTION("such that and assign pattern clauses have 1 common synonym, all evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, stmt, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
 		shared_ptr<Declaration> assign = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		query->addAssignPatternClause(assign, var2, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { varSynonym, 1 }, {assignSynonym, 2} };
 		vector<vector<string>> expectedTable = { { "1", "u", "1" }, { "3", "count", "3" }, {"1", "x", "51"}, { "5", "x", "51" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a such that Parent*(s, w) pattern a(v, _)
 	SECTION("such that and assign pattern clauses have no common synonym, all evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		query->addRelationshipClause(RelationshipType::PARENT_T, stmt, wh);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"12", "13"} }, { "24", {"25","26"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
 		shared_ptr<Declaration> assign = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		query->addAssignPatternClause(assign, var, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count", "p"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { whileSynonym, 1 }, {assignSynonym, 2}, 
 			{varSynonym, 3} };
 		vector<vector<string>> expectedTable = { 
 			{ "1", "12", "1", "x1" },
 			{ "1", "12", "1", "u" }, 
 			{ "1", "12", "3", "count" },
 			{ "1", "12", "3", "p" },

 			{ "1", "13", "1", "x1" },
 			{ "1", "13", "1", "u" },
 			{ "1", "13", "3", "count" },
 			{ "1", "13", "3", "p" },

 			{ "24", "25", "1", "x1" },
 			{ "24", "25", "1", "u" }, 
 			{ "24", "25", "3", "count" },
 			{ "24", "25", "3", "p" },

 			{ "24", "26", "1", "x1" },
 			{ "24", "26", "1", "u" },
 			{ "24", "26", "3", "count" },
 			{ "24", "26", "3", "p" } 
 		};
		
 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a such that Modifies(a, v) pattern a(v, _)
 	SECTION("such that and assign pattern clauses have 2 common synonyms, rs clause evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = {};
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> assign2 = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
 		query->addAssignPatternClause(assign2, var2, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a such that Modifies(a, v) pattern a(v, _)
 	SECTION("such that and assign pattern clauses have 2 common synonyms, pattern clause evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> assign2 = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
 		query->addAssignPatternClause(assign2, var2, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = {};
 		pkb->addMapResult(patternClauseResult);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Uses(s, v) pattern a(v, _)
 	SECTION("such that and assign pattern clauses have 1 common synonym, rs clause evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, stmt, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = {};
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
 		shared_ptr<Declaration> assign = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		query->addAssignPatternClause(assign, var2, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a such that Uses(s, v) pattern a(v, _)
 	SECTION("such that and assign pattern clauses have 1 common synonym, pattern clause evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, stmt, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
 		shared_ptr<Declaration> assign = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		query->addAssignPatternClause(assign, var2, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = {};
 		pkb->addMapResult(patternClauseResult);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select w such that Modifies(w, v) pattern w(v, _)
 	SECTION("such that and while pattern clauses have 2 common synonyms, all evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, synWhile, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synWhile2 = make_shared<Declaration>(EntityType::WHILE, whileSynonym);
 		query->addContainerPatternClause(synWhile2, var2);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = { { whileSynonym, 0 }, { varSynonym, 1 } };
 		vector<vector<string>> expectedTable = { { "1", "u" }, { "3", "count" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select w such that Uses(s, v) pattern w(v, _)
 	SECTION("such that and while pattern clauses have 1 common synonym, all evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, stmt, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synWhile = make_shared<Declaration>(EntityType::WHILE, whileSynonym);
 		query->addContainerPatternClause(synWhile, var2);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { varSynonym, 1 }, {whileSynonym, 2} };
 		vector<vector<string>> expectedTable = { { "1", "u", "1" }, { "3", "count", "3" }, {"1", "x", "51"}, { "5", "x", "51" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select w such that Parent*(ifs, s) pattern w(v, _)
 	SECTION("such that and while pattern clauses have no common synonym, all evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		query->addRelationshipClause(RelationshipType::PARENT_T, synIf, stmt);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"12", "13"} }, { "24", {"25","26"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synWhile = make_shared<Declaration>(EntityType::WHILE, whileSynonym);
 		query->addContainerPatternClause(synWhile, var);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count", "p"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = { { ifSynonym, 2 }, { stmtSynonym, 3 }, {whileSynonym, 0},
 			{varSynonym, 1} };
 		vector<vector<string>> expectedTable = {
 			{ "1", "x1", "1", "12" },
 			{ "1", "x1", "1", "13" },
 			{ "1", "x1", "24", "25" },
 			{ "1", "x1", "24", "26" },

 			{ "1", "u", "1", "12" },
 			{ "1", "u", "1", "13" },
 			{ "1", "u", "24", "25" },
 			{ "1", "u", "24", "26" },


 			{ "3", "count", "1", "12" },
 			{ "3", "count", "1", "13" },
 			{ "3", "count", "24", "25" },
 			{ "3", "count", "24", "26" },

 			{ "3", "p", "1", "12" },
 			{ "3", "p", "1", "13" },
 			{ "3", "p", "24", "25" },
 			{ "3", "p", "24", "26" }
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select w such that Modifies(w, v) pattern w(v, _)
 	SECTION("such that and while pattern clauses have 2 common synonyms, rs clause evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, synWhile, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = {};
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synWhile2 = make_shared<Declaration>(EntityType::WHILE, whileSynonym);
 		query->addContainerPatternClause(synWhile2, var2);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select w such that Modifies(w, v) pattern w(v, _)
 	SECTION("such that and while pattern clauses have 2 common synonyms, pattern clause evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, synWhile, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synWhile2 = make_shared<Declaration>(EntityType::WHILE, whileSynonym);
 		query->addContainerPatternClause(synWhile2, var2);
 		unordered_map<string, unordered_set<string>> patternClauseResult = {};
 		pkb->addMapResult(patternClauseResult);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select w such that Uses(s, v) pattern w(v, _)
 	SECTION("such that and while pattern clauses have 1 common synonym, rs clause evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, stmt, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = {};
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synWhile = make_shared<Declaration>(EntityType::WHILE, whileSynonym);
 		query->addContainerPatternClause(synWhile, var2);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select w such that Uses(s, v) pattern w(v, _)
 	SECTION("such that and while pattern clauses have 1 common synonym, pattern clause evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, stmt, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synWhile = make_shared<Declaration>(EntityType::WHILE, whileSynonym);
 		query->addContainerPatternClause(synWhile, var2);
 		unordered_map<string, unordered_set<string>> patternClauseResult = {};
 		pkb->addMapResult(patternClauseResult);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
	
 	//Select ifs such that Modifies(ifs, v) pattern ifs(v, _, _)
 	SECTION("such that and if pattern clauses have 2 common synonyms, all evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, synIf, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synIf2 = make_shared<Declaration>(EntityType::IF, ifSynonym);
 		query->addContainerPatternClause(synIf2, var2);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = { { ifSynonym, 0 }, { varSynonym, 1 } };
 		vector<vector<string>> expectedTable = { { "1", "u" }, { "3", "count" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select ifs such that Uses(s, v) pattern ifs(v, _)
 	SECTION("such that and if pattern clauses have 1 common synonym, all evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, stmt, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synIf = make_shared<Declaration>(EntityType::IF, ifSynonym);
 		query->addContainerPatternClause(synIf, var2);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = { { stmtSynonym, 0 }, { varSynonym, 1 }, {ifSynonym, 2} };
 		vector<vector<string>> expectedTable = { { "1", "u", "1" }, { "3", "count", "3" }, {"1", "x", "51"}, { "5", "x", "51" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select ifs such that Parent*(w, s) pattern ifs(v, _)
 	SECTION("such that and if pattern clauses have no common synonym, all evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> synWhile = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		query->addRelationshipClause(RelationshipType::PARENT_T, synWhile, stmt);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"12", "13"} }, { "24", {"25","26"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synIf = make_shared<Declaration>(EntityType::IF, ifSynonym);
 		query->addContainerPatternClause(synIf, var);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count", "p"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = { { whileSynonym, 2 }, { stmtSynonym, 3 }, {ifSynonym, 0},
 			{varSynonym, 1} };
 		vector<vector<string>> expectedTable = {
 			{ "1", "x1", "1", "12" },
 			{ "1", "x1", "1", "13" },
 			{ "1", "x1", "24", "25" },
 			{ "1", "x1", "24", "26" },

 			{ "1", "u", "1", "12" },
 			{ "1", "u", "1", "13" },
 			{ "1", "u", "24", "25" },
 			{ "1", "u", "24", "26" },

 			{ "3", "count", "1", "12" },
 			{ "3", "count", "1", "13" },
 			{ "3", "count", "24", "25" },
 			{ "3", "count", "24", "26" },

 			{ "3", "p", "1", "12" },
 			{ "3", "p", "1", "13" },
 			{ "3", "p", "24", "25" },
 			{ "3", "p", "24", "26" }
		
 		};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select ifs such that Modifies(ifs, v) pattern ifs(v, _)
 	SECTION("such that and if pattern clauses have 2 common synonyms, rs clause evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, synIf, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = {};
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synIf2 = make_shared<Declaration>(EntityType::IF, ifSynonym);
 		query->addContainerPatternClause(synIf2, var2);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select ifs such that Modifies(ifs, v) pattern ifs(v, _)
 	SECTION("such that and if pattern clauses have 2 common synonyms, pattern clause evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<QueryInput> synIf = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, ifSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, synIf, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synIf2 = make_shared<Declaration>(EntityType::IF, ifSynonym);
 		query->addContainerPatternClause(synIf2, var2);
 		unordered_map<string, unordered_set<string>> patternClauseResult = {};
 		pkb->addMapResult(patternClauseResult);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select ifs such that Uses(s, v) pattern ifs(v, _)
 	SECTION("such that and if pattern clauses have 1 common synonym, rs clause evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, stmt, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = {};
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synIf = make_shared<Declaration>(EntityType::IF, ifSynonym);
 		query->addContainerPatternClause(synIf, var2);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select ifs such that Uses(s, v) pattern ifs(v, _)
 	SECTION("such that and if pattern clauses have 1 common synonym, pattern clause evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::USES, stmt, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synIf = make_shared<Declaration>(EntityType::IF, ifSynonym);
 		query->addContainerPatternClause(synIf, var2);
 		unordered_map<string, unordered_set<string>> patternClauseResult = {};
 		pkb->addMapResult(patternClauseResult);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}
 }

 TEST_CASE("Evaluating query with multiple such that and pattern clauses") {
 	string stmtSynonym = "s";
 	string assignSynonym = "a";
 	string varSynonym = "v";
 	string whileSynonym = "w";
 	string ifSynonym = "if";
 	string progLine1Synonym = "n1";
 	string progLine2Synonym = "n2";

 	shared_ptr<QueryInterface> query = dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
 	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, "a");
 	query->addDeclarationToSelectClause(declaration);
 	shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
	
 	//Select a such that Modifies(a, v) such that Parent(s, a) pattern a(v, _x_)
 	SECTION("2 such that and 1 assign pattern clauses, all evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} }, { "8", {"y"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> assign1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		query->addRelationshipClause(RelationshipType::PARENT, stmt, assign1);
 		unordered_map<string, unordered_set<string>> rsClauseResult2 = { { "2", {"3"} }, { "4", {"5","6"} }, { "7", {"8","10"} } };
 		pkb->addMapResult(rsClauseResult2);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expr = make_shared<Expression>("x", ExpressionType::EXACT);
 		shared_ptr<Declaration> assign2 = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		query->addAssignPatternClause(assign2, var2, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} }, { "8", {"x", "y"} } };
 		pkb->addMapResult(patternClauseResult);

 		unordered_map<string, int> expectedMap = { { assignSynonym, 1 }, { varSynonym, 2 }, { stmtSynonym, 0 } };
 		vector<vector<string>> expectedTable = { { "2", "3", "count"  }, {  "7", "8", "y" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 	//Select a such that Modifies(a, v) pattern a(v, _x_) pattern w(v, _)
 	SECTION("1 such that and 2 pattern clauses, all evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expr = make_shared<Expression>("x", ExpressionType::EXACT);
 		shared_ptr<Declaration> assign2 = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		query->addAssignPatternClause(assign2, var2, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		shared_ptr<QueryInput> var3 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synWhile = make_shared<Declaration>(EntityType::WHILE, whileSynonym);
 		query->addContainerPatternClause(synWhile, var3);
 		unordered_map<string, unordered_set<string>> patternClauseResult2 = { { "7", {"u1", "u", "x"} }, { "20", {"i"} }, { "10", {"count"} } };
 		pkb->addMapResult(patternClauseResult2);

 		unordered_map<string, int> expectedMap = { { assignSynonym, 2 }, { varSynonym, 1 }, { whileSynonym, 0 } };
 		vector<vector<string>> expectedTable = { { "7", "u", "1" }, { "10", "count", "3" } };

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a such that Next(1, 2) such that Parent*(s, w) pattern a(v, _) pattern ifs(v, _, _)
 	SECTION("2 such that and 2 pattern clauses, all evaluates to non empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmtNum1 = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));
 		shared_ptr<QueryInput> stmtNum2 = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("2"));
 		query->addRelationshipClause(RelationshipType::NEXT, stmtNum1, stmtNum2);
 		pkb->addBooleanResult(true);

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		query->addRelationshipClause(RelationshipType::PARENT_T, stmt, wh);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"12", "13"} }, { "24", {"25","26"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
 		shared_ptr<Declaration> assign = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		query->addAssignPatternClause(assign, var, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count", "p"} } };
 		pkb->addMapResult(patternClauseResult);

 		shared_ptr<QueryInput> var1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synIf = make_shared<Declaration>(EntityType::IF, ifSynonym);
 		query->addContainerPatternClause(synIf, var1);
 		unordered_map<string, unordered_set<string>> patternClauseResult2 = { { "30", {"count", "y"} }, { "38", {"x1", "u"} }, { "40", {"x3", "u3"} } };
 		pkb->addMapResult(patternClauseResult2);

 		unordered_map<string, int> expectedMap = { { stmtSynonym, 3 }, { whileSynonym, 4 }, {assignSynonym, 2},
 			{varSynonym, 1}, { ifSynonym, 0} };
 		vector<vector<string>> expectedTable = {
 			{ "38", "x1", "1", "1", "12" },
 			{ "38", "x1", "1", "1", "13" },
 			{ "38", "x1", "1", "24", "25" },
 			{ "38", "x1", "1", "24", "26" },

 			{ "38", "u", "1", "1", "12" },
 			{ "38", "u", "1", "1", "13" },
 			{ "38", "u", "1",  "24", "25" },
 			{ "38", "u", "1", "24", "26" },

 			{ "30", "count", "3", "1", "12", },
 			{ "30", "count", "3", "1", "13" },
 			{ "30", "count", "3", "24", "25" },
 			{ "30", "count", "3", "24", "26" }
 		};

		
 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(!resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}

 	//Select a such that Modifies(a, v) such that Parent(s, a) pattern a(v, _x_)
 	SECTION("2 such that and 1 assign pattern clauses, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} }, { "8", {"y"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> assign1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		query->addRelationshipClause(RelationshipType::PARENT, stmt, assign1);
 		unordered_map<string, unordered_set<string>> rsClauseResult2 = {};
 		pkb->addMapResult(rsClauseResult2);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expr = make_shared<Expression>("x", ExpressionType::EXACT);
 		shared_ptr<Declaration> assign2 = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		query->addAssignPatternClause(assign2, var2, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} }, { "8", {"x", "y"} } };
 		pkb->addMapResult(patternClauseResult);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Modifies(a, v) pattern a(v, _x_) pattern w(v, _)
 	SECTION("1 such that and 2 pattern clauses, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, assignSynonym));
 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		query->addRelationshipClause(RelationshipType::MODIFIES, assign, var);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"x", "u"} }, { "3", {"count","p"} }, { "5", {"x"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var2 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expr = make_shared<Expression>("x", ExpressionType::EXACT);
 		shared_ptr<Declaration> assign2 = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		query->addAssignPatternClause(assign2, var2, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count"} }, { "51", {"x"} } };
 		pkb->addMapResult(patternClauseResult);

 		shared_ptr<QueryInput> var3 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synWhile = make_shared<Declaration>(EntityType::WHILE, whileSynonym);
 		query->addContainerPatternClause(synWhile, var3);
 		unordered_map<string, unordered_set<string>> patternClauseResult2 = {};
 		pkb->addMapResult(patternClauseResult2);

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 	}

 	//Select a such that Next(1, 2) such that Parent*(s, w) pattern a(v, _) pattern ifs(v, _, _)
 	SECTION("2 such that and 2 pattern clauses, evaluates to empty results") {
 		shared_ptr<PKBStub> pkb = make_shared<PKBStub>();

 		shared_ptr<QueryInput> stmtNum1 = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));
 		shared_ptr<QueryInput> stmtNum2 = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("2"));
 		query->addRelationshipClause(RelationshipType::NEXT, stmtNum1, stmtNum2);
 		pkb->addBooleanResult(false);

 		shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, stmtSynonym));
 		shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, whileSynonym));
 		query->addRelationshipClause(RelationshipType::PARENT_T, stmt, wh);
 		unordered_map<string, unordered_set<string>> rsClauseResult = { { "1", {"12", "13"} }, { "24", {"25","26"} } };
 		pkb->addMapResult(rsClauseResult);

 		shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);
 		shared_ptr<Declaration> assign = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
 		query->addAssignPatternClause(assign, var, expr);
 		unordered_map<string, unordered_set<string>> patternClauseResult = { { "1", {"x1", "u"} }, { "3", {"count", "p"} } };
 		pkb->addMapResult(patternClauseResult);

 		shared_ptr<QueryInput> var1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, varSynonym));
 		shared_ptr<Declaration> synIf = make_shared<Declaration>(EntityType::IF, ifSynonym);
 		query->addContainerPatternClause(synIf, var1);
 		unordered_map<string, unordered_set<string>> patternClauseResult2 = { { "30", {"count", "y"} }, { "38", {"x1", "u"} }, { "40", {"x3", "u3"} } };
 		pkb->addMapResult(patternClauseResult2);

 		unordered_map<string, int> expectedMap = {};
 		vector<vector<string>> expectedTable = {};

 		QueryEvaluator qe = QueryEvaluator(query, pkb);
 		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
 		REQUIRE(resultsTable->isNoResult());
 		unordered_map<string, int> actualMap = resultsTable->getSynonymIndexMap();
 		vector<vector<string>> actualTable = resultsTable->getTableValues();
 		TestResultsTableUtil::checkMap(actualMap, expectedMap);
 		TestResultsTableUtil::checkTable(actualTable, expectedTable);
 	}
	
 }
