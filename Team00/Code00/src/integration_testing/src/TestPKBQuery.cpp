#include <memory>
#include <list>

#include "PKB.h"
#include "Query.h"
#include "QueryEvaluator.h"
#include "QueryInput.h"
#include "QueryInputType.h"
#include "StmtNum.h"
#include "Ident.h"
#include "Any.h"
#include "ResultsProjector.h"
#include "TestResultsTableUtil.h"
#include "catch.hpp"

TEST_CASE("testSuchThatClauseSuccess") {

	shared_ptr<PKB> pkb = make_shared<PKB>(10);

	// 1 2 3 4 {5 {6, 7} 8} 9, 10
	REQUIRE(pkb->insertProcedure("main"));
	REQUIRE(pkb->insertFollow(1, 2));
	REQUIRE(pkb->insertFollow(2, 3));
	REQUIRE(pkb->insertFollow(3, 4));
	REQUIRE(pkb->insertParent(4, 5));
	REQUIRE(pkb->insertParent(5, 6));
	REQUIRE(pkb->insertParent(5, 7));
	REQUIRE(pkb->insertFollow(6, 7));
	REQUIRE(pkb->insertParent(4, 8));
	REQUIRE(pkb->insertFollow(5, 8));
	REQUIRE(pkb->insertFollow(4, 9));
	REQUIRE(pkb->insertFollow(9, 10));

	REQUIRE(pkb->insertFollowStar(1, 2));
	REQUIRE(pkb->insertFollowStar(1, 3));
	REQUIRE(pkb->insertFollowStar(1, 4));
	REQUIRE(pkb->insertFollowStar(1, 9));
	REQUIRE(pkb->insertFollowStar(1, 10));
	REQUIRE(pkb->insertFollowStar(2, 3));
	REQUIRE(pkb->insertFollowStar(2, 4));
	REQUIRE(pkb->insertFollowStar(2, 9));
	REQUIRE(pkb->insertFollowStar(2, 10));
	REQUIRE(pkb->insertFollowStar(3, 4));
	REQUIRE(pkb->insertFollowStar(3, 9));
	REQUIRE(pkb->insertFollowStar(3, 10));
	REQUIRE(pkb->insertFollowStar(4, 9));
	REQUIRE(pkb->insertFollowStar(4, 10));
	REQUIRE(pkb->insertFollowStar(4, 10));
	REQUIRE(pkb->insertFollowStar(6, 7));
	REQUIRE(pkb->insertFollowStar(5, 8));

	REQUIRE(pkb->insertParentStar(4, 5));
	REQUIRE(pkb->insertParentStar(4, 6));
	REQUIRE(pkb->insertParentStar(4, 7));
	REQUIRE(pkb->insertParentStar(4, 8));
	REQUIRE(pkb->insertParentStar(5, 6));
	REQUIRE(pkb->insertParentStar(5, 7));
	
	// 1 2 3 4 {5 {6, 7} 8} 9, 10
	REQUIRE(pkb->insertNext(1, 2));
	REQUIRE(pkb->insertNext(2, 3));
	REQUIRE(pkb->insertNext(3, 4));
	REQUIRE(pkb->insertNext(4, 5));
	REQUIRE(pkb->insertNext(5, 6));
	REQUIRE(pkb->insertNext(6, 7));
	REQUIRE(pkb->insertNext(7, 6));
	REQUIRE(pkb->insertNext(7, 8));
	REQUIRE(pkb->insertNext(5, 8));
	REQUIRE(pkb->insertNext(8, 5));
	REQUIRE(pkb->insertNext(8, 9));
	REQUIRE(pkb->insertNext(4, 9));
	REQUIRE(pkb->insertNext(9, 10));

	REQUIRE(pkb->insertModifies(3, "x"));
	REQUIRE(pkb->insertUses(3, "x"));
	REQUIRE(pkb->insertExpression(3, *(new Expression("x", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(3, *(new Expression("x+1", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(3, *(new Expression("x+1", ExpressionType::EXACT))));
	REQUIRE(pkb->setStatementType(3, EntityType::ASSIGN));
	REQUIRE(pkb->setStatementType(4, EntityType::WHILE));
	REQUIRE(pkb->setStatementType(5, EntityType::WHILE));
	REQUIRE(pkb->setStatementType(8, EntityType::ASSIGN));
	REQUIRE(pkb->insertUses(4, "t"));
	REQUIRE(pkb->insertUses(4, "v"));
	REQUIRE(pkb->insertUses(4, "x"));
	REQUIRE(pkb->insertUses(4, "y"));
	REQUIRE(pkb->insertModifies(4, "y"));
	REQUIRE(pkb->insertUses(5, "v"));
	REQUIRE(pkb->insertModifies(8, "y"));
	REQUIRE(pkb->insertUses(8, "x"));
	REQUIRE(pkb->insertUses(8, "y"));
	REQUIRE(pkb->insertExpression(8, *(new Expression("x", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(8, *(new Expression("x+1", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(8, *(new Expression("(x+1)*y", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(8, *(new Expression("(x+1)*y", ExpressionType::EXACT))));

	REQUIRE(pkb->insertProcUses("main", "x"));
	REQUIRE(pkb->insertProcUses("main", "y"));
	REQUIRE(pkb->insertProcUses("main", "t"));
	REQUIRE(pkb->insertProcUses("main", "v"));
	REQUIRE(pkb->insertProcModifies("main", "x"));
	REQUIRE(pkb->insertProcModifies("main", "y"));
	REQUIRE(pkb->setControlVariable(4, "t"));
	REQUIRE(pkb->setControlVariable(5, "v"));

	SECTION("nonBooleanResultsFollowsT") {
		// select s such that follow*(1, s)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::STMT, "s");

		shared_ptr<QueryInput> stmt =
			dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, "s"));
		shared_ptr<QueryInput> stmtNum =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(1));

		query->addDeclarationToSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::FOLLOWS_T, stmtNum, stmt);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "10", "2", "3", "4", "9" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("nonBooleanResultsFollows") {
		// select s such that follows(s, 2)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::STMT, "s");

		shared_ptr<QueryInput> stmtNum2 =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(2));

		query->addDeclarationToSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::FOLLOWS, declaration, stmtNum2);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "1" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("nonBooleanResultsParent") {
		// select s such that Parent(5, s)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::STMT, "s");

		shared_ptr<QueryInput> stmtNum5 =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(5));

		query->addDeclarationToSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::PARENT, stmtNum5, declaration);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "6", "7" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("nonBooleanResultsNext") {
		// select n such that Next(7, n)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::PROGLINE, "n");

		shared_ptr<QueryInput> stmtNum7 =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(7));

		query->addDeclarationToSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::NEXT, stmtNum7, declaration);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "6", "8" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("setResultsProcUses") {
		// select v such that Uses("main", v)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::VAR, "v");

		shared_ptr<QueryInput> proc =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));

		query->addDeclarationToSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::USES, proc, declaration);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "t", "v", "x", "y" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("setResultsUses") {
		// select v such that Uses(4, v)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::VAR, "v");

		shared_ptr<QueryInput> stmtNum4 =
			dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>(4));

		query->addDeclarationToSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::USES, stmtNum4, declaration);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "t", "v", "x", "y" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("setResultsProcUses") {
		// select v such that Uses("main", v)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::VAR, "v");

		shared_ptr<QueryInput> proc =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));

		query->addDeclarationToSelectClause(declaration);
		query->addRelationshipClause(RelationshipType::USES, proc, declaration);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "t", "v", "x", "y" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("mapResultsAssignPattern") {
		// select v pattern a(v, _"x+1"_)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration1 =
			make_shared<Declaration>(EntityType::ASSIGN, "a");
		shared_ptr<Declaration> declaration2 =
			make_shared<Declaration>(EntityType::VAR, "v");
		shared_ptr<Expression> expression = make_shared<Expression>("x+1", ExpressionType::PARTIAL);

		query->addDeclarationToSelectClause(declaration2);
		query->addAssignPatternClause(declaration1, declaration2, expression);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "x", "y" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("setResultsContainerPattern") {
		// select w pattern w("v", _)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration =
			make_shared<Declaration>(EntityType::WHILE, "w");
		shared_ptr<QueryInput> variable =
			dynamic_pointer_cast<QueryInput>(make_shared<Ident>("v"));

		query->addDeclarationToSelectClause(declaration);
		query->addContainerPatternClause(declaration, variable);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "5" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

}

TEST_CASE("testWithCaluse") {

	shared_ptr<PKB> pkb = make_shared<PKB>(10);
	/*
	procedure main {
		read A;
		read B;
		z = A * B + 1;
		print z;
		call A;
	}
	procedure A {
		read C;
		print A;
		call B;
	}
	procedure B {
		main = A * B;
		print A;
	}
	*/
	REQUIRE(pkb->setStatementType(1, EntityType::READ));
	REQUIRE(pkb->setStatementType(2, EntityType::READ));
	REQUIRE(pkb->setStatementType(3, EntityType::ASSIGN));
	REQUIRE(pkb->setStatementType(4, EntityType::PRINT));
	REQUIRE(pkb->setStatementType(5, EntityType::CALL));
	REQUIRE(pkb->setStatementType(6, EntityType::READ));
	REQUIRE(pkb->setStatementType(7, EntityType::PRINT));
	REQUIRE(pkb->setStatementType(8, EntityType::CALL));
	REQUIRE(pkb->setStatementType(9, EntityType::ASSIGN));
	REQUIRE(pkb->setStatementType(10, EntityType::PRINT));

	REQUIRE(pkb->insertProcedure("main"));
	REQUIRE(pkb->insertProcedure("A"));
	REQUIRE(pkb->insertProcedure("B"));
	REQUIRE(pkb->insertVariable("A"));
	REQUIRE(pkb->insertVariable("B"));
	REQUIRE(pkb->insertVariable("z"));
	REQUIRE(pkb->insertVariable("C"));
	REQUIRE(pkb->insertVariable("main"));
	REQUIRE(pkb->insertConst("1"));
	REQUIRE(pkb->insertConst("2"));
	REQUIRE(pkb->insertConst("5"));
	REQUIRE(pkb->insertConst("6"));
	REQUIRE(pkb->insertConst("7"));

	REQUIRE(pkb->insertUsedName(1, "A"));
	REQUIRE(pkb->insertUsedName(2, "B"));
	REQUIRE(pkb->insertUsedName(4, "z"));
	REQUIRE(pkb->insertUsedName(5, "A"));
	REQUIRE(pkb->insertUsedName(6, "C"));
	REQUIRE(pkb->insertUsedName(7, "A"));
	REQUIRE(pkb->insertUsedName(8, "B"));
	REQUIRE(pkb->insertUsedName(10, "A"));

	REQUIRE(pkb->insertFollowStar(1, 2));
	REQUIRE(pkb->insertFollowStar(1, 3));
	REQUIRE(pkb->insertFollowStar(1, 4));
	REQUIRE(pkb->insertFollowStar(1, 5));
	REQUIRE(pkb->insertFollowStar(2, 3));
	REQUIRE(pkb->insertFollowStar(2, 4));
	REQUIRE(pkb->insertFollowStar(2, 5));
	REQUIRE(pkb->insertFollowStar(3, 4));
	REQUIRE(pkb->insertFollowStar(3, 5));
	REQUIRE(pkb->insertFollowStar(4, 5));
	REQUIRE(pkb->insertFollowStar(6, 7));
	REQUIRE(pkb->insertFollowStar(6, 8));
	REQUIRE(pkb->insertFollowStar(7, 8));
	REQUIRE(pkb->insertFollowStar(9, 10));

	REQUIRE(pkb->insertModifies(1, "A"));
	REQUIRE(pkb->insertModifies(2, "B"));
	REQUIRE(pkb->insertModifies(3, "z"));
	REQUIRE(pkb->insertModifies(5, "C"));
	REQUIRE(pkb->insertModifies(5, "main"));
	REQUIRE(pkb->insertModifies(6, "C"));
	REQUIRE(pkb->insertModifies(8, "main"));
	REQUIRE(pkb->insertModifies(9, "main"));

	REQUIRE(pkb->insertExpression(3, *(new Expression("A", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(3, *(new Expression("B", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(3, *(new Expression("A*B", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(3, *(new Expression("(A*B)+1", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(3, *(new Expression("(A*B)+1", ExpressionType::EXACT))));
	REQUIRE(pkb->insertExpression(9, *(new Expression("A", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(9, *(new Expression("B", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(9, *(new Expression("A*B", ExpressionType::PARTIAL))));
	REQUIRE(pkb->insertExpression(9, *(new Expression("A*B", ExpressionType::EXACT))));


	pkb->init();

	SECTION("twoDeclarations") {
		// Select <p, v> with p.procName = var.varName
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> declaration1 =
			make_shared<Declaration>(EntityType::PROC, "p");
		shared_ptr<Declaration> declaration2 =
			make_shared<Declaration>(EntityType::VAR, "v");

		query->addDeclarationToSelectClause(declaration1);
		query->addDeclarationToSelectClause(declaration2);
		query->addWithClause(declaration1, declaration2);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "A A", "B B", "main main" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("multipleWithClauses") {
		// Select <c, r, proc> with c.procName = var and r.varName = proc and proc = c.procName
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> p =
			make_shared<Declaration>(EntityType::PROC, "proc");
		shared_ptr<Declaration> v =
			make_shared<Declaration>(EntityType::VAR, "var");
		shared_ptr<Declaration> r1 =
			make_shared<Declaration>(EntityType::READ, "r");
		shared_ptr<Declaration> r2 =
			make_shared<Declaration>(EntityType::READ, "r");
		shared_ptr<Declaration> c1 =
			make_shared<Declaration>(EntityType::CALL, "c");
		shared_ptr<Declaration> c2 =
			make_shared<Declaration>(EntityType::CALL, "c");
		c1->setIsAttribute();
		r1->setIsAttribute();
		query->addDeclarationToSelectClause(c2);
		query->addDeclarationToSelectClause(r2);
		query->addDeclarationToSelectClause(p);
		query->addWithClause(c1, v);
		query->addWithClause(r1, p);
		query->addWithClause(p, c1);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "5 1 A", "8 2 B" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("multipleClauses") {
		// Select <pn, v, r> such that follows*(a, pn) pattern a(v, _"A*B"_) with r.varName = proc
		//                                  3 4, 9 10        3 z, 9 main                1 A, 2 B
		// (4 z, 10 main) cross (1, 2)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> proc =
			make_shared<Declaration>(EntityType::PROC, "proc");
		shared_ptr<Declaration> v =
			make_shared<Declaration>(EntityType::VAR, "v");
		shared_ptr<Declaration> r1 =
			make_shared<Declaration>(EntityType::READ, "r");
		shared_ptr<Declaration> r2 =
			make_shared<Declaration>(EntityType::READ, "r");
		shared_ptr<Declaration> pn =
			make_shared<Declaration>(EntityType::PRINT, "pn");
		shared_ptr<Declaration> a =
			make_shared<Declaration>(EntityType::ASSIGN, "a");
		r1->setIsAttribute();
		shared_ptr<Expression> exp = make_shared<Expression>("A*B", ExpressionType::PARTIAL);
		query->addDeclarationToSelectClause(pn);
		query->addDeclarationToSelectClause(v);
		query->addDeclarationToSelectClause(r2);
		query->addRelationshipClause(RelationshipType::FOLLOWS_T,
			dynamic_pointer_cast<QueryInput>(a),
			dynamic_pointer_cast<QueryInput>(pn));
		query->addAssignPatternClause(a, v, exp);
		query->addWithClause(r1, proc);

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "4 z 1", "4 z 2", "10 main 1", "10 main 2" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}

	SECTION("disconnectedClauses") {
		// Select <v2, v3> such that modifies(s, v1) pattern a(v2, _"A*B"_) with c.procName = v3
		//                            irrelevant          3 z, 9 main              5 A, 8 B
		// (z, main) cross (A, B)
		shared_ptr<QueryInterface> query =
			dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
		shared_ptr<Declaration> s =
			make_shared<Declaration>(EntityType::STMT, "s");
		shared_ptr<Declaration> v1 =
			make_shared<Declaration>(EntityType::VAR, "v1");
		shared_ptr<Declaration> v2 =
			make_shared<Declaration>(EntityType::VAR, "v2");
		shared_ptr<Declaration> v3 =
			make_shared<Declaration>(EntityType::VAR, "v3");
		shared_ptr<Declaration> c =
			make_shared<Declaration>(EntityType::CALL, "c");
		shared_ptr<Declaration> a =
			make_shared<Declaration>(EntityType::ASSIGN, "a");
		c->setIsAttribute();
		shared_ptr<Expression> exp = make_shared<Expression>("A*B", ExpressionType::PARTIAL);
		query->addDeclarationToSelectClause(v2);
		query->addDeclarationToSelectClause(v3);
		query->addAssignPatternClause(a, v2, exp);
		query->addWithClause(c, v3);
		query->addRelationshipClause(RelationshipType::MODIFIES,
			dynamic_pointer_cast<QueryInput>(s),
			dynamic_pointer_cast<QueryInput>(v1));

		QueryEvaluator qe = QueryEvaluator(query, pkb);

		list<string> expected{ "z A", "z B", "main A", "main B" };
		list<string> result{ };
		shared_ptr<ResultsTable> resultsTable = qe.evaluate();
		ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
		TestResultsTableUtil::checkList(result, expected);
	}
	/*
	procedure main {
		read A;
		read B;
		z = A * B + 1;
		print z;
		call A;
	}
	procedure A {
		read C;
		print A;
		call B;
	}
	procedure B {
		main = A * B;
		print A;
	}
	*/
}