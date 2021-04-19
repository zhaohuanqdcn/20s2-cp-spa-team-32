#include "catch.hpp"
#include "QueryOptimizer.h"
#include "OptionalClause.h"
#include "RelationshipClause.h"
#include "PatternClause.h"
#include "WithClause.h"
#include "Declaration.h"
#include "StmtNum.h"
#include "Expression.h"
#include "Ident.h"
#include "TestResultsTableUtil.h"

TEST_CASE("Test Sort Clauses by results size") {
	shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, "s"));
	shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, "a"));
	shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, "a"));
	shared_ptr<QueryInput> ten = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("10"));
	shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);

	shared_ptr<RelationshipClause> followsClause1 = make_shared<RelationshipClause>(RelationshipType::FOLLOWS, stmt, assign);
	shared_ptr<RelationshipClause> followsClause2 = make_shared<RelationshipClause>(RelationshipType::FOLLOWS_T, stmt, ten);
	shared_ptr<PatternClause> assignPatternClause1 = make_shared<PatternClause>(dynamic_pointer_cast<Declaration>(assign), var, expr);
	shared_ptr<PatternClause> assignPatternClause2 = make_shared<PatternClause>(dynamic_pointer_cast<Declaration>(assign), var, expr);
	shared_ptr<WithClause> withClause1 = make_shared<WithClause>(stmt, assign);
	shared_ptr<WithClause> withClause2 = make_shared<WithClause>(stmt, stmt);

	followsClause1->addMapResult({ {"1", {"2", "3"}}, { "4", { "5" } } }); // result size 3
	REQUIRE(followsClause1->getResultSize() == 3);

	followsClause2->addSetResult({ "1", "2", "3", "4", "5" }); // result size 5
	REQUIRE(followsClause2->getResultSize() == 5);

	assignPatternClause1->addMapResult({
		{"1", {"2", "3", "4"}}, { "5", { "6", "7" } },
		{"11", {"12", "13", "14"}}, { "15", { "16", "17" } },
		{"21", {"22", "23", "24"}}, { "25", { "26", "27" } } }); // result size 15 
	REQUIRE(assignPatternClause1->getResultSize() == 15);

	assignPatternClause2->addMapResult({
		{"1", {"2", "3", "4"}}, { "5", { "6" } },
		{"11", {"12", "13", "14"}}, { "15", { "16" } },
		{"21", {"22", "23", "24"}}, { "25", { "26" } } }); // result size 12 
	REQUIRE(assignPatternClause2->getResultSize() == 12);

	withClause1->addSetResult({ "1", "2", "3", "4" }); // result size 4
	REQUIRE(withClause1->getResultSize() == 4);

	withClause2->addSetResult({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11" }); // result size 11
	REQUIRE(withClause2->getResultSize() == 11);

	vector<shared_ptr<OptionalClause>> actual = { followsClause2, followsClause1,
		assignPatternClause1, assignPatternClause2, withClause2, withClause1 };
	actual = QueryOptimizer::sortClausesByResultSize(actual);
	vector<shared_ptr<OptionalClause>> expected = { followsClause1 , withClause1, followsClause2, withClause2, assignPatternClause2, assignPatternClause1 };
	TestResultsTableUtil::checkClauseList(actual, expected);
}

TEST_CASE("Test Sort ResultTables by table size") {
	shared_ptr<ResultsTable> size3 = make_shared<ResultsTable>();
	size3->populateWithMap({ { "1", {"2", "3"} }, { "4", { "5" } } }, {"s", "a"});
	REQUIRE(size3->getTableSize() == 3);

	shared_ptr<ResultsTable> size5 = make_shared<ResultsTable>();
	size5->populateWithSet({ "1", "2", "3", "4", "5" }, { "s" });
	REQUIRE(size5->getTableSize() == 5);

	shared_ptr<ResultsTable> size15 = make_shared<ResultsTable>();
	size15->populateWithMap({
		{"1", {"2", "3", "4"}}, { "5", { "6", "7" } },
		{"11", {"12", "13", "14"}}, { "15", { "16", "17" } },
		{"21", {"22", "23", "24"}}, { "25", { "26", "27" } } },
		{"a", "v"}
	);
	REQUIRE(size15->getTableSize() == 15);

	shared_ptr<ResultsTable> size12 = make_shared<ResultsTable>();
	size12->populateWithMap({
		{"1", {"2", "3", "4"}}, { "5", { "6" } },
		{"11", {"12", "13", "14"}}, { "15", { "16" } },
		{"21", {"22", "23", "24"}}, { "25", { "26" } } },
		{"a", "v1"}
	);
	REQUIRE(size12->getTableSize() == 12);

	shared_ptr<ResultsTable> size4 = make_shared<ResultsTable>();
	size4->populateWithSet({ "1", "2", "3", "4" }, { "n" });
	REQUIRE(size4->getTableSize() == 4);

	shared_ptr<ResultsTable> size11 = make_shared<ResultsTable>();
	size11->populateWithSet({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11" }, { "s" });
	REQUIRE(size11->getTableSize() == 11);

	vector<shared_ptr<ResultsTable>> actual = { size5, size3, size15, size12, size11, size4 };
	actual = QueryOptimizer::sortTablesBySize(actual);
	vector<shared_ptr<ResultsTable>> expected = { size3, size4, size5, size11, size12, size15 };
	for (size_t i = 0; i < actual.size(); i++) {
		shared_ptr<ResultsTable> actualTable = actual.at(i);
		shared_ptr<ResultsTable> expectedTable = expected.at(i);

		TestResultsTableUtil::checkTable(actualTable->getTableValues(), expectedTable->getTableValues());
	}
}