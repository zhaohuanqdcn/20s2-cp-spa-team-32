#pragma once
#include "ResultsProjector.h"
#include "TestResultsTableUtil.h"
#include "PKBStub.h"
#include "catch.hpp"

TEST_CASE("Projecting list of final results from select clause with BOOLEAN") {
	string stmtSynonym = "s";
	string assignSynonym = "a";
	string whileSynonym = "w";
	const string TRUE = "TRUE";
	const string FALSE = "FALSE";

	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<SelectClause> selectClause = make_shared<SelectClause>();

	SECTION("Select BOOLEAN true, all clauses fulfilled") {
		unordered_map<string, int> indexMap = { {stmtSynonym, 0}, {assignSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);
		list<string> expectedList = { TRUE };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	SECTION("Select BOOLEAN false, some clauses not fulfilled") {
		unordered_map<string, int> indexMap = { {stmtSynonym, 0}, {assignSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);
		resultsTable->setIsNoResult();
		list<string> expectedList = { FALSE };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

}

TEST_CASE("Projecting empty list of final results from select clause with synonym/tuple given empty resultsTable from QE") {
	string stmtSynonym = "s";
	string assignSynonym = "a";
	string whileSynonym = "w";

	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<SelectClause> selectClause = make_shared<SelectClause>();
	shared_ptr<Declaration> selectedDeclaration = make_shared<Declaration>(EntityType::STMT, stmtSynonym);
	selectClause->addDeclaration(selectedDeclaration);

	unordered_map<string, int> indexMap = { {stmtSynonym, 0}, {assignSynonym, 1} };
	vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
	shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
	resultsTable->setTable(indexMap, table);
	resultsTable->setIsNoResult();
	list<string> expectedList = { };
	list<string> actualList;
	ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
	TestResultsTableUtil::checkList(actualList, expectedList);
}

TEST_CASE("Projecting list of final results from select clause with single declaration") {
	string stmtSynonym = "s";
	string assignSynonym = "a";
	string whileSynonym = "w";

	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);
	shared_ptr<SelectClause> selectClause = make_shared<SelectClause>();
	selectClause->addDeclaration(declaration);

	// select a
	SECTION("Select declaration in PKB results") {
		unordered_map<string, int> indexMap = { {stmtSynonym, 0}, {assignSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);
		list<string> expectedList = { "22", "33", "44" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select a
	SECTION("Select declaration not in PKB results, declaration has non empty pkb values") {
		unordered_map<string, int> indexMap = { {whileSynonym, 0}, {stmtSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ "1", "2", "3", "4" });
		pkb->addEntityType(EntityType::ASSIGN);

		list<string> expectedList = { "1", "2", "3", "4" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select a
	SECTION("Select declaration not in PKB results, declaration has empty pkb values") {
		unordered_map<string, int> indexMap = { {stmtSynonym, 0}, {stmtSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ });
		pkb->addEntityType(EntityType::ASSIGN);

		list<string> expectedList = {};
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
}

TEST_CASE("Projecting list of final results from select clause with single attribute") {
	string stmtSynonym = "s";
	string assignSynonym = "a";
	string whileSynonym = "w";
	string printSynonym = "pn";

	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<Declaration> declaration = make_shared<Declaration>(EntityType::PRINT, printSynonym);
	declaration->setIsAttribute();
	shared_ptr<SelectClause> selectClause = make_shared<SelectClause>();
	selectClause->addDeclaration(declaration);

	// select pn.varName
	SECTION("Select attribute in PKB results") {
		unordered_map<string, int> indexMap = { {stmtSynonym, 0}, {printSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addStringResult("x");
		pkb->addStringResult("y");
		pkb->addStringResult("z");

		list<string> expectedList = { "x", "y", "z" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select pn.varName
	SECTION("Select attribute not in PKB results, attribute has non empty pkb values") {
		unordered_map<string, int> indexMap = { {whileSynonym, 0}, {stmtSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ "1", "2", "3" });
		pkb->addEntityType(EntityType::PRINT);

		pkb->addStringResult("x");
		pkb->addStringResult("y");
		pkb->addStringResult("z");

		list<string> expectedList = { "x", "y", "z" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select pn.varName
	SECTION("Select attribute not in PKB results, attribute has empty pkb values") {
		unordered_map<string, int> indexMap = { {stmtSynonym, 0}, {stmtSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ });
		pkb->addEntityType(EntityType::PRINT);

		list<string> expectedList = {};
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
}

TEST_CASE("Projecting list of final results from select clause with tuple without attribute") {

	string stmtSynonym = "s";
	string assignSynonym = "a";
	string varSynonym = "v";
	string whileSynonym = "w";
	string printSynonym = "pn";

	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<Declaration> varDeclaration = make_shared<Declaration>(EntityType::VAR, varSynonym);
	shared_ptr<Declaration> stmtDeclaration = make_shared<Declaration>(EntityType::STMT, stmtSynonym);
	shared_ptr<Declaration> assignDeclaration = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);

	shared_ptr<SelectClause> selectClause = make_shared<SelectClause>();
	selectClause->addDeclaration(stmtDeclaration);
	selectClause->addDeclaration(varDeclaration);
	selectClause->addDeclaration(assignDeclaration);

	// select <s, v, a>
	SECTION("Select tuple in PKB results, all clauses fulfilled") {
		unordered_map<string, int> indexMap = { {assignSynonym, 0}, {stmtSynonym, 1}, {varSynonym, 2} };
		vector<vector<string>> table = { {"1", "11", "22"}, {"2", "11", "33"}, {"3", "11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);
		list<string> expectedList = { "11 22 1", "11 33 2", "11 44 3" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select <s, v, a, v>
	SECTION("Select tuple in PKB results, all clauses fulfilled, repeated selected synonym") {
		selectClause->addDeclaration(varDeclaration);
		unordered_map<string, int> indexMap = { {assignSynonym, 0}, {stmtSynonym, 1}, {varSynonym, 2} };
		vector<vector<string>> table = { {"1", "11", "22"}, {"2", "11", "33"}, {"3", "11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);
		list<string> expectedList = { "11 22 1 22", "11 33 2 33", "11 44 3 44" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select <s, v, a>
	SECTION("Select tuple not in PKB results, all clauses fulfilled") {
		unordered_map<string, int> indexMap = { {printSynonym, 0}, {whileSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ "1", "2" });
		pkb->addEntityType(EntityType::STMT);
		pkb->addSetResult({ "a", "b"});
		pkb->addEntityType(EntityType::VAR);
		pkb->addSetResult({ "3", "4" });
		pkb->addEntityType(EntityType::ASSIGN);

		list<string> expectedList = { "1 a 3", "1 a 4", "1 b 3", "1 b 4", "2 a 3", "2 a 4", "2 b 3", "2 b 4" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select <s, v, a, v>
	SECTION("Select tuple not in PKB results, all clauses fulfilled, repeated synonym") {
		selectClause->addDeclaration(varDeclaration);
		unordered_map<string, int> indexMap = { {printSynonym, 0}, {whileSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ "1", "2" });
		pkb->addEntityType(EntityType::STMT);
		pkb->addSetResult({ "a", "b" });
		pkb->addEntityType(EntityType::VAR);
		pkb->addSetResult({ "3", "4" });
		pkb->addEntityType(EntityType::ASSIGN);

		list<string> expectedList = { "1 a 3 a", "1 a 4 a", "1 b 3 b", "1 b 4 b", "2 a 3 a", "2 a 4 a", "2 b 3 b", "2 b 4 b" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select <s, v, a>
	SECTION("Select tuple not in PKB results, all clauses fulfilled, selected synonym has empty pkb results") {
		unordered_map<string, int> indexMap = { {printSynonym, 0}, {whileSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ "1", "2" });
		pkb->addEntityType(EntityType::STMT);
		pkb->addSetResult({ });
		pkb->addEntityType(EntityType::VAR);
		pkb->addSetResult({ "3", "4" });
		pkb->addEntityType(EntityType::ASSIGN);

		list<string> expectedList = { };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select <s, v, a, v>
	SECTION("Select tuple not in PKB results, all clauses fulfilled, repeated synonym, selected synonym has empty pkb results") {
		selectClause->addDeclaration(varDeclaration);
		unordered_map<string, int> indexMap = { {printSynonym, 0}, {whileSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ "1", "2" });
		pkb->addEntityType(EntityType::STMT);
		pkb->addSetResult({ "a", "b" });
		pkb->addEntityType(EntityType::VAR);
		pkb->addSetResult({});
		pkb->addEntityType(EntityType::ASSIGN);

		list<string> expectedList = {  };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

}

TEST_CASE("Projecting list of final results from select clause with tuple with attribute") {

	string stmtSynonym = "s";
	string assignSynonym = "a";
	string varSynonym = "v";
	string whileSynonym = "w";
	string printSynonym = "pn";

	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<Declaration> varDeclaration = make_shared<Declaration>(EntityType::VAR, varSynonym);

	shared_ptr<Declaration> printDeclaration = make_shared<Declaration>(EntityType::PRINT, printSynonym);
	printDeclaration->setIsAttribute();
	shared_ptr<Declaration> stmtDeclaration = make_shared<Declaration>(EntityType::STMT, stmtSynonym);
	shared_ptr<Declaration> assignDeclaration = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);

	shared_ptr<SelectClause> selectClause = make_shared<SelectClause>();
	selectClause->addDeclaration(stmtDeclaration);
	selectClause->addDeclaration(printDeclaration);
	selectClause->addDeclaration(assignDeclaration);

	// select <s, pn.varName, a>
	SECTION("Select tuple in PKB results, all clauses fulfilled") {
		unordered_map<string, int> indexMap = { {assignSynonym, 0}, {stmtSynonym, 1}, {printSynonym, 2} };
		vector<vector<string>> table = { {"1", "11", "22"}, {"2", "11", "33"}, {"3", "11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addStringResult("x");
		pkb->addStringResult("y");
		pkb->addStringResult("z");

		list<string> expectedList = { "11 x 1", "11 y 2", "11 z 3" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
	
	// select <s, pn.varName, a, pn>
	SECTION("Select tuple in PKB results, all clauses fulfilled, repeated selected synonym") {
		shared_ptr<Declaration> printDeclaration1 = make_shared<Declaration>(EntityType::PRINT, printSynonym);

		selectClause->addDeclaration(printDeclaration1);
		unordered_map<string, int> indexMap = { {assignSynonym, 0}, {stmtSynonym, 1}, {printSynonym, 2} };
		vector<vector<string>> table = { {"1", "11", "22"}, {"2", "11", "33"}, {"3", "11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addStringResult("x");
		pkb->addStringResult("y");
		pkb->addStringResult("z");

		list<string> expectedList = { "11 x 1 22", "11 y 2 33", "11 z 3 44" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
	
	// select <s, pn.varName, a>
	SECTION("Select tuple not in PKB results, all clauses fulfilled") {
		unordered_map<string, int> indexMap = { {varSynonym, 0}, {whileSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ "1", "2" });
		pkb->addEntityType(EntityType::STMT);
		pkb->addSetResult({ "3", "4" });
		pkb->addEntityType(EntityType::PRINT);
		pkb->addSetResult({ "5", "6" });
		pkb->addEntityType(EntityType::ASSIGN);

		pkb->addStringResult("a");
		pkb->addStringResult("a");
		pkb->addStringResult("b");
		pkb->addStringResult("b");
		pkb->addStringResult("a");
		pkb->addStringResult("a");
		pkb->addStringResult("b");
		pkb->addStringResult("b");

		list<string> expectedList = { "1 a 5", "1 a 6", "1 b 5", "1 b 6", "2 a 5", "2 a 6", "2 b 5", "2 b 6" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
	
	// select <s, pn.varName, a, pn>
	SECTION("Select tuple not in PKB results, all clauses fulfilled, repeated synonym") {
		shared_ptr<Declaration> printDeclaration1 = make_shared<Declaration>(EntityType::PRINT, printSynonym);

		selectClause->addDeclaration(printDeclaration1);
		unordered_map<string, int> indexMap = { {varSynonym, 0}, {whileSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ "1", "2" });
		pkb->addEntityType(EntityType::STMT);
		pkb->addSetResult({ "3", "4" });
		pkb->addEntityType(EntityType::PRINT);
		pkb->addSetResult({ "5", "6" });
		pkb->addEntityType(EntityType::ASSIGN);

		pkb->addStringResult("a");
		pkb->addStringResult("a");
		pkb->addStringResult("b");
		pkb->addStringResult("b");
		pkb->addStringResult("a");
		pkb->addStringResult("a");
		pkb->addStringResult("b");
		pkb->addStringResult("b");

		list<string> expectedList = { "1 a 5 3", "1 a 6 3", "1 b 5 4", "1 b 6 4", "2 a 5 3", "2 a 6 3", "2 b 5 4", "2 b 6 4" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
	
	// select <s, pn.varName, a>
	SECTION("Select tuple not in PKB results, all clauses fulfilled, selected synonym has empty pkb results") {
		unordered_map<string, int> indexMap = { {varSynonym, 0}, {whileSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ "1", "2" });
		pkb->addEntityType(EntityType::STMT);
		pkb->addSetResult({ });
		pkb->addEntityType(EntityType::PRINT);
		pkb->addSetResult({ "5", "6" });
		pkb->addEntityType(EntityType::ASSIGN);

		pkb->addStringResult("a");
		pkb->addStringResult("a");
		pkb->addStringResult("b");
		pkb->addStringResult("b");
		pkb->addStringResult("a");
		pkb->addStringResult("a");
		pkb->addStringResult("b");
		pkb->addStringResult("b");

		list<string> expectedList = { };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
	
	// select <s, pn.varName, a, pn>
	SECTION("Select tuple not in PKB results, all clauses fulfilled, repeated synonym, selected synonym has empty pkb results") {
		shared_ptr<Declaration> printDeclaration1 = make_shared<Declaration>(EntityType::PRINT, printSynonym);

		selectClause->addDeclaration(printDeclaration1);
		unordered_map<string, int> indexMap = { {varSynonym, 0}, {whileSynonym, 1} };
		vector<vector<string>> table = { {"11", "22"}, {"11", "33"}, {"11", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ "1", "2" });
		pkb->addEntityType(EntityType::STMT);
		pkb->addSetResult({ });
		pkb->addEntityType(EntityType::PRINT);
		pkb->addSetResult({ "5", "6" });
		pkb->addEntityType(EntityType::ASSIGN);

		pkb->addStringResult("a");
		pkb->addStringResult("a");
		pkb->addStringResult("b");
		pkb->addStringResult("b");
		pkb->addStringResult("a");
		pkb->addStringResult("a");
		pkb->addStringResult("b");
		pkb->addStringResult("b");

		list<string> expectedList = {  };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
	
}

TEST_CASE("Projecting list of final results from select clause with mixed tuple") {

	string stmtSynonym = "s";
	string assignSynonym = "a";
	string varSynonym = "v";
	string whileSynonym = "w";
	string printSynonym = "pn";

	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<Declaration> varDeclaration = make_shared<Declaration>(EntityType::VAR, varSynonym);

	shared_ptr<Declaration> printDeclaration = make_shared<Declaration>(EntityType::PRINT, printSynonym);
	printDeclaration->setIsAttribute();
	shared_ptr<Declaration> stmtDeclaration = make_shared<Declaration>(EntityType::STMT, stmtSynonym);
	shared_ptr<Declaration> assignDeclaration = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);

	shared_ptr<SelectClause> selectClause = make_shared<SelectClause>();
	selectClause->addDeclaration(stmtDeclaration);
	selectClause->addDeclaration(printDeclaration);
	selectClause->addDeclaration(assignDeclaration);

	// select <s, pn.varName, a>
	SECTION("Synonyms not in results table have non empty pkb results") {
		unordered_map<string, int> indexMap = { {varSynonym, 0}, {stmtSynonym, 1}, {printSynonym, 2} };
		vector<vector<string>> table = { {"1", "11", "22"}, {"2", "12", "33"}, {"3", "13", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ "3", "4" });
		pkb->addEntityType(EntityType::ASSIGN);

		pkb->addStringResult("x");
		pkb->addStringResult("x");
		pkb->addStringResult("y");
		pkb->addStringResult("y");
		pkb->addStringResult("z");
		pkb->addStringResult("z");

		list<string> expectedList = { "11 x 3", "11 x 4", "12 y 3", "12 y 4", "13 z 3", "13 z 4" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select <s, pn.varName, a>
	SECTION("Synonyms not in results table have empty pkb results") {
		unordered_map<string, int> indexMap = { {varSynonym, 0}, {stmtSynonym, 1}, {printSynonym, 2} };
		vector<vector<string>> table = { {"1", "11", "22"}, {"2", "12", "33"}, {"3", "13", "44"} };
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable->setTable(indexMap, table);

		pkb->addSetResult({ });
		pkb->addEntityType(EntityType::ASSIGN);

		pkb->addStringResult("x");
		pkb->addStringResult("x");
		pkb->addStringResult("y");
		pkb->addStringResult("y");
		pkb->addStringResult("z");
		pkb->addStringResult("z");

		list<string> expectedList = { };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
}

TEST_CASE("Projecting list of final results from select clause without optonal clauses") {

	string stmtSynonym = "s";
	string assignSynonym = "a";
	string varSynonym = "v";
	string whileSynonym = "w";
	string printSynonym = "pn";
	const string TRUE = "TRUE";
	const string FALSE = "FALSE";

	shared_ptr<PKBStub> pkb = make_shared<PKBStub>();
	shared_ptr<Declaration> varDeclaration = make_shared<Declaration>(EntityType::VAR, varSynonym);

	shared_ptr<Declaration> printDeclaration = make_shared<Declaration>(EntityType::PRINT, printSynonym);
	printDeclaration->setIsAttribute();
	shared_ptr<Declaration> stmtDeclaration = make_shared<Declaration>(EntityType::STMT, stmtSynonym);
	shared_ptr<Declaration> assignDeclaration = make_shared<Declaration>(EntityType::ASSIGN, assignSynonym);

	shared_ptr<SelectClause> selectClause = make_shared<SelectClause>();

	// select BOOLEAN
	SECTION("Select BOOLEAN true") {
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		list<string> expectedList = { TRUE };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select a
	SECTION("Select single declaration with non empty pkb values") {
		selectClause->addDeclaration(assignDeclaration);
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();

		pkb->addSetResult({ "1", "2", "3", "4" });
		pkb->addEntityType(EntityType::ASSIGN);

		list<string> expectedList = { "1", "2", "3", "4" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select a
	SECTION("Select single declaration with empty pkb values") {
		selectClause->addDeclaration(assignDeclaration);
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();

		pkb->addSetResult({});
		pkb->addEntityType(EntityType::ASSIGN);

		list<string> expectedList = {};
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select pn.varName
	SECTION("Select single attribute with non empty pkb values") {
		printDeclaration->setIsAttribute();
		selectClause->addDeclaration(printDeclaration);
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();

		pkb->addSetResult({ "1", "2", "3", "4" });
		pkb->addEntityType(EntityType::PRINT);

		pkb->addStringResult("a");
		pkb->addStringResult("b");
		pkb->addStringResult("c");
		pkb->addStringResult("d");

		list<string> expectedList = { "a", "b", "c", "d" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select pn.varName
	SECTION("Select single attribute with empty pkb values") {
		printDeclaration->setIsAttribute();
		selectClause->addDeclaration(printDeclaration);
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();

		pkb->addSetResult({});
		pkb->addEntityType(EntityType::PRINT);

		pkb->addStringResult("a");
		pkb->addStringResult("b");
		pkb->addStringResult("c");
		pkb->addStringResult("d");

		list<string> expectedList = { };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
	
	// select <s, pn.varName, a>
	SECTION("Select tuple in PKB results with non empty pkb values") {
		selectClause->addDeclaration(stmtDeclaration);
		selectClause->addDeclaration(printDeclaration);
		selectClause->addDeclaration(assignDeclaration);

		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();

		pkb->addSetResult({ "11", "22"});
		pkb->addEntityType(EntityType::STMT);
		pkb->addSetResult({ "3", "4" });
		pkb->addEntityType(EntityType::PRINT);
		pkb->addSetResult({ "5", "6" });
		pkb->addEntityType(EntityType::ASSIGN);

		pkb->addStringResult("x");
		pkb->addStringResult("x");
		pkb->addStringResult("y");
		pkb->addStringResult("y");
		pkb->addStringResult("x");
		pkb->addStringResult("x");
		pkb->addStringResult("y");
		pkb->addStringResult("y");

		list<string> expectedList = { "11 x 5", "11 x 6", "11 y 5", "11 y 6", "22 x 5", "22 x 6", "22 y 5", "22 y 6" };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}

	// select <s, pn.varName, a>
	SECTION("Select tuple in PKB results with empty pkb values") {
		selectClause->addDeclaration(stmtDeclaration);
		selectClause->addDeclaration(printDeclaration);
		selectClause->addDeclaration(assignDeclaration);

		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();

		pkb->addSetResult({ "11", "22" });
		pkb->addEntityType(EntityType::STMT);
		pkb->addSetResult({ });
		pkb->addEntityType(EntityType::PRINT);
		pkb->addSetResult({ "5", "6" });
		pkb->addEntityType(EntityType::ASSIGN);

		pkb->addStringResult("x");
		pkb->addStringResult("x");
		pkb->addStringResult("y");
		pkb->addStringResult("y");
		pkb->addStringResult("x");
		pkb->addStringResult("x");
		pkb->addStringResult("y");
		pkb->addStringResult("y");

		list<string> expectedList = { };
		list<string> actualList;
		ResultsProjector::projectResults(resultsTable, selectClause, pkb, actualList);
		TestResultsTableUtil::checkList(actualList, expectedList);
	}
}