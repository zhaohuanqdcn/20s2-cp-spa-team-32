#include "PKB.h"
#include "StmtNum.h"
#include "Ident.h"
#include "Any.h"

#include "catch.hpp"
using namespace std;


TEST_CASE("PKB setAndGetTypes") {

	PKB pkb = PKB(10);

	REQUIRE_FALSE(pkb.setStatementType(1, EntityType::PROC));
	REQUIRE_FALSE(pkb.setStatementType(2, EntityType::VAR));
	REQUIRE_FALSE(pkb.setStatementType(3, EntityType::CONST));
	REQUIRE_FALSE(pkb.setStatementType(4, EntityType::STMT));
	
	REQUIRE(pkb.insertProcedure("main"));
	REQUIRE(pkb.setStatementType(5, EntityType::WHILE));
	REQUIRE(pkb.setStatementType(6, EntityType::IF));
	REQUIRE(pkb.setStatementType(7, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(8, EntityType::CALL));
	REQUIRE(pkb.setStatementType(9, EntityType::READ));
	REQUIRE(pkb.setStatementType(10, EntityType::READ));
	REQUIRE(pkb.insertConst("-100"));
	REQUIRE(pkb.insertConst("3"));

	REQUIRE_FALSE(pkb.setStatementType(5, EntityType::PRINT));
	REQUIRE_FALSE(pkb.setStatementType(11, EntityType::CALL));
	REQUIRE_FALSE(pkb.setStatementType(0, EntityType::WHILE));
	REQUIRE_FALSE(pkb.setStatementType(-1e9+7, EntityType::IF));

	REQUIRE(pkb.getEntities(EntityType::PRINT).empty());
	unordered_set<string> readResult;
	readResult.insert("9");
	readResult.insert("10");
	REQUIRE(pkb.getEntities(EntityType::READ) == readResult);
	unordered_set<string> whileResult;
	whileResult.insert("5");
	REQUIRE(pkb.getEntities(EntityType::WHILE) == whileResult);
	unordered_set<string> procResult;
	procResult.insert("main");
	REQUIRE(pkb.getEntities(EntityType::PROC) == procResult);
	unordered_set<string> constResult;
	constResult.insert("-100");
	constResult.insert("3");
	REQUIRE(pkb.getEntities(EntityType::CONST) == constResult);

	REQUIRE(pkb.getEntities(EntityType::VAR).empty());
	REQUIRE(pkb.getEntities(EntityType::STMT).size() == 10);
}

TEST_CASE("PKB parentAndFollow") {
	
	PKB pkb = PKB(10);
	// 1 2 3 4 {5 {6}} 7 8 9
	REQUIRE(pkb.insertFollow(1, 2));
	REQUIRE(pkb.insertFollow(2, 3));
	REQUIRE(pkb.insertFollow(3, 4));
	REQUIRE(pkb.insertParent(4, 5));
	REQUIRE(pkb.insertParent(5, 6));
	REQUIRE(pkb.insertFollow(4, 7));
	REQUIRE(pkb.insertFollow(7, 8));
	REQUIRE(pkb.insertFollow(8, 9));

	REQUIRE(pkb.insertFollowStar(1, 2));
	REQUIRE(pkb.insertFollowStar(1, 3));
	REQUIRE(pkb.insertFollowStar(1, 4));
	REQUIRE(pkb.insertFollowStar(1, 7));
	REQUIRE(pkb.insertFollowStar(1, 8));
	REQUIRE(pkb.insertFollowStar(1, 9));
	REQUIRE(pkb.insertFollowStar(2, 3));
	REQUIRE(pkb.insertFollowStar(2, 4));
	REQUIRE(pkb.insertFollowStar(2, 7));
	REQUIRE(pkb.insertFollowStar(2, 8));
	REQUIRE(pkb.insertFollowStar(2, 9));
	REQUIRE(pkb.insertFollowStar(3, 4));
	REQUIRE(pkb.insertFollowStar(3, 7));
	REQUIRE(pkb.insertFollowStar(3, 8));
	REQUIRE(pkb.insertFollowStar(3, 9));
	REQUIRE(pkb.insertFollowStar(4, 7));
	REQUIRE(pkb.insertFollowStar(4, 8));
	REQUIRE(pkb.insertFollowStar(4, 9));
	REQUIRE(pkb.insertFollowStar(7, 8));
	REQUIRE(pkb.insertFollowStar(7, 9));
	REQUIRE(pkb.insertFollowStar(8, 9));
	REQUIRE(pkb.insertParentStar(4, 5));
	REQUIRE(pkb.insertParentStar(4, 6));
	REQUIRE(pkb.insertParentStar(5, 6));

	REQUIRE_FALSE(pkb.insertFollow(0, 1));
	REQUIRE_FALSE(pkb.insertFollow(3, 1));
	REQUIRE_FALSE(pkb.insertParent(0, -1));
	REQUIRE_FALSE(pkb.insertParent(1, 1));

	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(1)), shared_ptr<QueryInput>(new StmtNum(2))));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(3)), shared_ptr<QueryInput>(new StmtNum(4))));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(8)), shared_ptr<QueryInput>(new StmtNum(9))));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, 
		shared_ptr<QueryInput>(new Any()), shared_ptr<QueryInput>(new StmtNum(3))));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(7)), shared_ptr<QueryInput>(new Any())));
	REQUIRE(pkb.getBooleanResultOfRS(FOLLOWS, 
		shared_ptr<QueryInput>(new Any()), shared_ptr<QueryInput>(new Any())));
	REQUIRE(pkb.getBooleanResultOfRS(PARENT, 
		shared_ptr<QueryInput>(new StmtNum(4)), shared_ptr<QueryInput>(new StmtNum(5))));
	REQUIRE(pkb.getBooleanResultOfRS(PARENT, 
		shared_ptr<QueryInput>(new StmtNum(4)), shared_ptr<QueryInput>(new Any())));

	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(9)), 
		shared_ptr<QueryInput>(new StmtNum(10))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(4)), 
		shared_ptr<QueryInput>(new StmtNum(8))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::FOLLOWS, 
		shared_ptr<QueryInput>(new StmtNum(0)), 
		shared_ptr<QueryInput>(new StmtNum(1))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::PARENT, 
		shared_ptr<QueryInput>(new StmtNum(11)), 
		shared_ptr<QueryInput>(new Any())));
	
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::FOLLOWS, 
		shared_ptr<QueryInput>(new Declaration(EntityType::ASSIGN,"a")), 
		shared_ptr<QueryInput>(new StmtNum(1))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::PARENT,
		shared_ptr<QueryInput>(new Any()), 
		shared_ptr<QueryInput>(new Declaration(EntityType::CALL, "c"))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(RelationshipType::PARENT,
		shared_ptr<QueryInput>(new Any()), 
		shared_ptr<QueryInput>(new Ident("dummyIdent"))));

	unordered_set<string> resultFollow = pkb.getSetResultsOfRS(FOLLOWS,
		shared_ptr<QueryInput>(new Any()), 
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")));
	string a[] = { "2", "3", "4", "7", "8", "9" };
	unordered_set<string> expectedFollow{ std::begin(a), std::end(a) };
	REQUIRE(expectedFollow == resultFollow);
	
	unordered_set<string> resultParent = pkb.getSetResultsOfRS(PARENT,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new StmtNum(5)));
	string b[] = { "4" }; 
	unordered_set<string> expectedParent{ std::begin(b), std::end(b) };
	REQUIRE(expectedParent == resultParent);

	unordered_set<string> resultFollowStar = pkb.getSetResultsOfRS(FOLLOWS_T,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")),
		shared_ptr<QueryInput>(new StmtNum(8)));
	string c[] = { "1", "2", "3", "4", "7" };
	unordered_set<string> expectedFollowStar{ std::begin(c), std::end(c) };
	REQUIRE(expectedFollowStar == resultFollowStar);
	
	unordered_map<string, unordered_set<string>> resultParentStar = 
		pkb.getMapResultsOfRS(PARENT_T,
			shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s1")), 
			shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s2")));
	REQUIRE(resultParentStar.size() == 2);
	REQUIRE(unordered_set<string> { "5", "6" } == resultParentStar["4"]);
	REQUIRE(unordered_set<string> { "6" } == resultParentStar["5"]);
}

TEST_CASE("PKB usesAndModifies") {

	PKB pkb = PKB(16);
	// 1 2 {3, 4, 5 {6, 7, 8 {9, 10}} 11 {12 {13}} 14, 15} 16

	REQUIRE(pkb.insertUses(2, "x"));
	REQUIRE(pkb.insertUses(2, "y"));
	REQUIRE(pkb.insertUses(2, "z"));
	REQUIRE(pkb.insertUses(2, "a"));
	REQUIRE(pkb.insertUses(2, "b"));
	REQUIRE(pkb.insertUses(2, "c"));
	REQUIRE(pkb.insertUses(2, "t"));
	REQUIRE(pkb.insertUses(3, "x"));
	REQUIRE(pkb.insertUses(3, "y"));
	REQUIRE(pkb.insertUses(5, "a"));
	REQUIRE(pkb.insertUses(5, "b"));
	REQUIRE(pkb.insertUses(5, "a"));
	REQUIRE(pkb.insertUses(5, "c"));
	REQUIRE(pkb.insertUses(5, "z"));
	REQUIRE(pkb.insertUses(8, "a"));
	REQUIRE(pkb.insertUses(8, "c"));
	REQUIRE(pkb.insertUses(8, "z"));
	REQUIRE(pkb.insertUses(9, "a"));
	REQUIRE(pkb.insertUses(9, "c"));
	REQUIRE(pkb.insertUses(9, "z"));
	REQUIRE(pkb.insertUses(11, "z"));
	REQUIRE(pkb.insertUses(11, "t"));
	REQUIRE(pkb.insertUses(12, "z"));
	REQUIRE(pkb.insertUses(12, "t"));
	REQUIRE(pkb.insertUses(15, "a"));
	REQUIRE(pkb.insertUses(16, "m"));

	REQUIRE(pkb.insertModifies(2, "x"));
	REQUIRE(pkb.insertModifies(2, "y"));
	REQUIRE(pkb.insertModifies(2, "z"));
	REQUIRE(pkb.insertModifies(2, "t"));
	REQUIRE(pkb.insertModifies(2, "count"));
	REQUIRE(pkb.insertModifies(2, "y"));
	REQUIRE(pkb.insertModifies(3, "x"));
	REQUIRE(pkb.insertModifies(5, "y"));
	REQUIRE(pkb.insertModifies(5, "z"));
	REQUIRE(pkb.insertModifies(5, "t"));
	REQUIRE(pkb.insertModifies(6, "y"));
	REQUIRE(pkb.insertModifies(7, "z"));
	REQUIRE(pkb.insertModifies(8, "t"));
	REQUIRE(pkb.insertModifies(9, "t"));
	REQUIRE(pkb.insertModifies(14, "count"));
	REQUIRE(pkb.insertModifies(15, "y"));

	REQUIRE(pkb.getBooleanResultOfRS(MODIFIES, 
		shared_ptr<QueryInput>(new StmtNum(3)), shared_ptr<QueryInput>(new Ident("x"))));
	REQUIRE(pkb.getBooleanResultOfRS(MODIFIES, 
		shared_ptr<QueryInput>(new StmtNum(5)), shared_ptr<QueryInput>(new Ident("t"))));
	REQUIRE(pkb.getBooleanResultOfRS(USES, 
		shared_ptr<QueryInput>(new StmtNum(8)), shared_ptr<QueryInput>(new Ident("c"))));
	REQUIRE(pkb.getBooleanResultOfRS(USES, 
		shared_ptr<QueryInput>(new StmtNum(11)), shared_ptr<QueryInput>(new Any())));
	REQUIRE(pkb.getBooleanResultOfRS(USES, 
		shared_ptr<QueryInput>(new Any()), shared_ptr<QueryInput>(new Ident("z"))));
	
	unordered_set<string> resultUses1 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new Ident("t")));
	string c1[] = { "2", "11", "12" };
	REQUIRE(unordered_set<string>{ std::begin(c1), std::end(c1) } == resultUses1);
	
	unordered_set<string> resultUses2 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new Ident("z")));
	string c2[] = { "2", "5", "8", "9", "11", "12" };
	REQUIRE(unordered_set<string>{ std::begin(c2), std::end(c2) } == resultUses2);
	
	unordered_set<string> resultUses3 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new StmtNum(2)), 
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "v")));
	string c3[] = { "a", "b", "c", "x", "y", "z", "t" };
	REQUIRE(unordered_set<string>{ std::begin(c3), std::end(c3) } == resultUses3);

	unordered_set<string> resultModifies1 = pkb.getSetResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new Ident("t")));
	string d1[] = { "2", "5", "8", "9" };
	REQUIRE(unordered_set<string>{ std::begin(d1), std::end(d1) } == resultModifies1);

	unordered_set<string> resultModifies2 = pkb.getSetResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new Declaration(EntityType::STMT, "s")), 
		shared_ptr<QueryInput>(new Ident("y")));
	string d2[] = { "2", "5", "6", "15" };
	REQUIRE(unordered_set<string>{ std::begin(d2), std::end(d2) } == resultModifies2);

	unordered_set<string> resultModifies3 = pkb.getSetResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new StmtNum(2)), 
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "v")));
	string d3[] = {  "x", "y", "z", "t", "count" };
	REQUIRE(unordered_set<string>{ std::begin(d3), std::end(d3) } == resultModifies3);

}

TEST_CASE("PKB multiProcedures") {
	
	PKB pkb = PKB(10);

	// main {1 {2, 3 {4 {5}}} first {6 {7}} second {8, 9, 10}
	// main -> first -> second

	REQUIRE(pkb.insertProcUses("main", "x"));
	REQUIRE(pkb.insertProcUses("main", "y"));
	REQUIRE(pkb.insertProcUses("main", "z"));
	REQUIRE(pkb.insertProcUses("main", "t"));
	REQUIRE(pkb.insertProcUses("main", "count"));
	REQUIRE(pkb.insertProcUses("first", "y"));
	REQUIRE(pkb.insertProcUses("first", "z"));
	REQUIRE(pkb.insertProcUses("first", "t"));
	REQUIRE(pkb.insertProcUses("first", "count"));
	REQUIRE(pkb.insertProcUses("second", "z"));
	REQUIRE(pkb.insertProcUses("second", "count"));

	REQUIRE(pkb.insertProcModifies("main", "count"));
	REQUIRE(pkb.insertProcModifies("main", "m"));
	REQUIRE(pkb.insertProcModifies("first", "count"));
	REQUIRE(pkb.insertProcModifies("second", "count"));

	REQUIRE(pkb.insertCalls("main", "first"));
	REQUIRE(pkb.insertCalls("first", "second"));
	REQUIRE(pkb.insertCallsStar("main", "first"));
	REQUIRE(pkb.insertCallsStar("main", "second"));
	REQUIRE(pkb.insertCallsStar("first", "second"));

	REQUIRE(pkb.getBooleanResultOfRS(MODIFIES, 
		shared_ptr<QueryInput>(new Ident("main")), shared_ptr<QueryInput>(new Ident("m"))));
	REQUIRE(pkb.getBooleanResultOfRS(MODIFIES, 
		shared_ptr<QueryInput>(new Ident("second")), shared_ptr<QueryInput>(new Ident("count"))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(MODIFIES, 
		shared_ptr<QueryInput>(new Ident("second")), shared_ptr<QueryInput>(new Ident("z"))));
	REQUIRE(pkb.getBooleanResultOfRS(USES, 
		shared_ptr<QueryInput>(new Ident("main")), shared_ptr<QueryInput>(new Ident("t"))));
	REQUIRE(pkb.getBooleanResultOfRS(USES,
		shared_ptr<QueryInput>(new Ident("first")), shared_ptr<QueryInput>(new Ident("count"))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(USES,
		shared_ptr<QueryInput>(new Ident("first")), shared_ptr<QueryInput>(new Ident("x"))));
	REQUIRE(pkb.getBooleanResultOfRS(CALLS, 
		shared_ptr<QueryInput>(new Ident("main")), shared_ptr<QueryInput>(new Ident("first"))));
	REQUIRE(pkb.getBooleanResultOfRS(CALLS_T, 
		shared_ptr<QueryInput>(new Ident("main")), shared_ptr<QueryInput>(new Ident("second"))));
	REQUIRE_FALSE(pkb.getBooleanResultOfRS(CALLS_T, 
		shared_ptr<QueryInput>(new Ident("first")), shared_ptr<QueryInput>(new Ident("main"))));

	unordered_set<string> resultUses1 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new Declaration(EntityType::PROC, "p")), 
		shared_ptr<QueryInput>(new Ident("t")));
	string c1[] = { "main", "first" };
	REQUIRE(unordered_set<string>{ std::begin(c1), std::end(c1) } == resultUses1);

	unordered_set<string> resultUses2 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new Declaration(EntityType::PROC, "proc")), 
		shared_ptr<QueryInput>(new Ident("count")));
	string c2[] = { "main", "first", "second" };
	REQUIRE(unordered_set<string>{ std::begin(c2), std::end(c2) } == resultUses2);

	unordered_set<string> resultUses3 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new Ident("first")), 
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "var")));
	string c3[] = { "y", "z", "t", "count" };
	REQUIRE(unordered_set<string>{ std::begin(c3), std::end(c3) } == resultUses3);
	
	unordered_set<string> resultUses4 = pkb.getSetResultsOfRS(USES,
		shared_ptr<QueryInput>(new Ident("count")), 
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "var")));
	REQUIRE(unordered_set<string>{ } == resultUses4);
	
	unordered_set<string> resultModifies1 = pkb.getSetResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new Declaration(EntityType::PROC, "s")), 
		shared_ptr<QueryInput>(new Ident("count")));
	string d1[] = { "main", "first", "second" };
	REQUIRE(unordered_set<string>{ std::begin(d1), std::end(d1) } == resultModifies1);

	unordered_set<string> resultModifies2 = pkb.getSetResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new Ident("main")), 
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "V")));
	string d2[] = { "count", "m" };
	REQUIRE(unordered_set<string>{ std::begin(d2), std::end(d2) } == resultModifies2);

	unordered_map<string, unordered_set<string>> resultModifies3 = pkb.getMapResultsOfRS(MODIFIES,
		shared_ptr<QueryInput>(new Declaration(EntityType::PROC, "p")),
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "V")));
	REQUIRE(unordered_set<string>{ "m", "count" } == resultModifies3["main"]);
	REQUIRE(unordered_set<string>{ "count" } == resultModifies3["first"]);
	REQUIRE(unordered_set<string>{ "count" } == resultModifies3["second"]);

}

TEST_CASE("PKB factorPattern") {
	
	PKB pkb = PKB(10);
	// 1 {2, 3, 4, 5, 6 {7 {8, 9}} 10}
	// 3. x = x + y + 1
	// 4. y = z + a + 1
	// 5. x = count - 1;
	// 8. z = a + x * y - 10;
	REQUIRE(pkb.setStatementType(3, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(4, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(5, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(8, EntityType::ASSIGN));
	REQUIRE(pkb.insertModifies(1, "x"));
	REQUIRE(pkb.insertModifies(1, "y"));
	REQUIRE(pkb.insertModifies(1, "z"));
	REQUIRE(pkb.insertModifies(3, "x"));
	REQUIRE(pkb.insertModifies(4, "y"));
	REQUIRE(pkb.insertModifies(5, "x"));
	REQUIRE(pkb.insertModifies(7, "z"));
	REQUIRE(pkb.insertModifies(8, "z"));
	REQUIRE(pkb.insertExpression(3, *(new Expression("x", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(3, *(new Expression("y", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(3, *(new Expression("1", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(4, *(new Expression("z", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(4, *(new Expression("a", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(4, *(new Expression("1", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(5, *(new Expression("count", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(5, *(new Expression("1", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(8, *(new Expression("a", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(8, *(new Expression("x", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(8, *(new Expression("y", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(8, *(new Expression("10", ExpressionType::PARTIAL))));

	unordered_map<string, unordered_set<string>> resultPattern = 
		pkb.getMapResultsOfAssignPattern(
			shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "v")), 
			*(new Expression("1", ExpressionType::PARTIAL)));

	REQUIRE(unordered_set<string>{ "x" } == resultPattern["3"]);
	REQUIRE(unordered_set<string>{ "y" } == resultPattern["4"]);
	REQUIRE(unordered_set<string>{ } == resultPattern["8"]);
	
	unordered_set<string> result1 = pkb.getSetResultsOfAssignPattern(shared_ptr<QueryInput>(new Any()), *(new Expression("x", ExpressionType::PARTIAL)));
	REQUIRE(unordered_set<string>{ "3", "8" } == result1);

	unordered_set<string> result2 = pkb.getSetResultsOfAssignPattern(shared_ptr<QueryInput>(new Any()), *(new Expression("y", ExpressionType::PARTIAL)));
	REQUIRE(unordered_set<string>{ "3", "8" } == result2);

	unordered_set<string> result3 = pkb.getSetResultsOfAssignPattern(shared_ptr<QueryInput>(new Ident("x")), *(new Expression("1", ExpressionType::PARTIAL)));
	REQUIRE(unordered_set<string>{ "3", "5" } == result3);

	unordered_set<string> result4 = pkb.getSetResultsOfAssignPattern(shared_ptr<QueryInput>(new StmtNum(3)), *(new Expression("1", ExpressionType::PARTIAL)));
	REQUIRE(unordered_set<string>{ } == result4);

	unordered_set<string> result5 = pkb.getSetResultsOfAssignPattern(shared_ptr<QueryInput>(new Ident("x")), *(new Expression("_", ExpressionType::EMPTY)));
	REQUIRE(unordered_set<string>{ "3", "5" } == result5);

	unordered_map<string, unordered_set<string>> result6 = pkb.getMapResultsOfAssignPattern(
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "x")), 
		*(new Expression("1", ExpressionType::PARTIAL)));
	REQUIRE(unordered_set<string>{ "x" } == result6["3"]);
	REQUIRE(unordered_set<string>{ "y" } == result6["4"]);
	REQUIRE(unordered_set<string>{ "x" } == result6["5"]);

	unordered_map<string, unordered_set<string>> result7 = pkb.getMapResultsOfAssignPattern(
		shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "VAR")),
		*(new Expression("_", ExpressionType::EMPTY)));
	REQUIRE(unordered_set<string>{ "x" } == result7["3"]);
	REQUIRE(unordered_set<string>{ "y" } == result7["4"]);
	REQUIRE(unordered_set<string>{ "x" } == result7["5"]);
	REQUIRE(unordered_set<string>{ "z" } == result7["8"]);

	unordered_set<string> result8 = pkb.getSetResultsOfAssignPattern(shared_ptr<QueryInput>(new StmtNum(3)), *(new Expression("1", ExpressionType::EXACT)));
	REQUIRE(unordered_set<string>{ } == result8);
}

TEST_CASE("PKB fullAssignPattern") {

	PKB pkb = PKB(7);
	/*
	a = (a + b) * 10;
	b = a + b - 100;
	c = (((a + b)) + c);
	x = a + (b + c);
	a = x * x * x;
	v = x * x * x;
	x = z + 10;
	*/
	REQUIRE(pkb.setStatementType(1, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(2, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(3, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(4, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(5, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(6, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(7, EntityType::ASSIGN));
	REQUIRE(pkb.insertModifies(1, "a"));
	REQUIRE(pkb.insertModifies(2, "b"));
	REQUIRE(pkb.insertModifies(3, "c"));
	REQUIRE(pkb.insertModifies(4, "x"));
	REQUIRE(pkb.insertModifies(5, "a"));
	REQUIRE(pkb.insertModifies(6, "v"));
	REQUIRE(pkb.insertModifies(7, "x"));
	REQUIRE(pkb.insertExpression(1, *(new Expression("a", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(1, *(new Expression("b", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(1, *(new Expression("10", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(1, *(new Expression("a+b", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(1, *(new Expression("(a+b)*10", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(1, *(new Expression("(a+b)*10", ExpressionType::EXACT))));
	REQUIRE(pkb.insertExpression(2, *(new Expression("a", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(2, *(new Expression("b", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(2, *(new Expression("100", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(2, *(new Expression("a+b", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(2, *(new Expression("(a+b)-100", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(2, *(new Expression("(a+b)-100", ExpressionType::EXACT))));
	REQUIRE(pkb.insertExpression(3, *(new Expression("a", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(3, *(new Expression("b", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(3, *(new Expression("c", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(3, *(new Expression("a+b", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(3, *(new Expression("(a+b)+c", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(3, *(new Expression("(a+b)+c", ExpressionType::EXACT))));
	REQUIRE(pkb.insertExpression(4, *(new Expression("a", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(4, *(new Expression("b", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(4, *(new Expression("c", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(4, *(new Expression("b+c", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(4, *(new Expression("a+(b+c)", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(4, *(new Expression("a+(b+c)", ExpressionType::EXACT))));
	REQUIRE(pkb.insertExpression(5, *(new Expression("x", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(5, *(new Expression("x*x", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(5, *(new Expression("(x*x)*x", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(5, *(new Expression("(x*x)*x", ExpressionType::EXACT))));
	REQUIRE(pkb.insertExpression(6, *(new Expression("x", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(6, *(new Expression("x*x", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(6, *(new Expression("(x*x)*x", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(6, *(new Expression("(x*x)*x", ExpressionType::EXACT))));
	REQUIRE(pkb.insertExpression(7, *(new Expression("z", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(7, *(new Expression("10", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(7, *(new Expression("z+10", ExpressionType::PARTIAL))));
	REQUIRE(pkb.insertExpression(7, *(new Expression("z+10", ExpressionType::EXACT))));

	// pattern a(v, _"a+b"_)
	unordered_map<string, unordered_set<string>> result1 =
		pkb.getMapResultsOfAssignPattern(
			shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "v")),
			*(new Expression("a+b", ExpressionType::PARTIAL)));
	REQUIRE(unordered_set<string>{ "a" } == result1["1"]);
	REQUIRE(unordered_set<string>{ "b" } == result1["2"]);
	REQUIRE(unordered_set<string>{ "c" } == result1["3"]);
	REQUIRE(unordered_set<string>{ } == result1["4"]);

	// pattern a(v, "x * x * x")
	unordered_map<string, unordered_set<string>> result2 =
		pkb.getMapResultsOfAssignPattern(
			shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "v")),
			*(new Expression("(x*x)*x", ExpressionType::EXACT)));
	REQUIRE(unordered_set<string>{ "a" } == result2["5"]);
	REQUIRE(unordered_set<string>{ "v" } == result2["6"]);

	// pattern a(_, _"10"_)
	unordered_set<string> result3 = pkb.getSetResultsOfAssignPattern(
		shared_ptr<QueryInput>(new Any()), 
		*(new Expression("10", ExpressionType::PARTIAL)));
	REQUIRE(unordered_set<string>{ "1", "7" } == result3);

	// pattern a("x", _)
	unordered_set<string> result4 = pkb.getSetResultsOfAssignPattern(
		shared_ptr<QueryInput>(new Ident("x")),
		*(new Expression("", ExpressionType::EMPTY)));
	REQUIRE(unordered_set<string>{ "4", "7" } == result4);

	// pattern a(_, _"x * x"_)
	unordered_set<string> result5 = pkb.getSetResultsOfAssignPattern(
			shared_ptr<QueryInput>(new Any()),
			*(new Expression("x*x", ExpressionType::PARTIAL)));
	REQUIRE(unordered_set<string>{ "5", "6" } == result5);

	// pattern a("a", _"a"_)
	unordered_set<string> result6 = pkb.getSetResultsOfAssignPattern(
		shared_ptr<QueryInput>(new Ident("a")),
		*(new Expression("a", ExpressionType::PARTIAL)));
	REQUIRE(unordered_set<string>{ "1" } == result6);

}


TEST_CASE("PKB containerPattern") {
	
	PKB pkb = PKB(10);
	// 1 {2} 3 {4, 5} 6 {7 {8, 9 {10}}}
	REQUIRE(pkb.setStatementType(1, EntityType::WHILE));
	REQUIRE(pkb.setStatementType(3, EntityType::IF));
	REQUIRE(pkb.setStatementType(6, EntityType::WHILE));
	REQUIRE(pkb.setStatementType(7, EntityType::IF));
	REQUIRE(pkb.setStatementType(9, EntityType::WHILE));
	
	REQUIRE(pkb.setControlVariable(1, "x"));
	REQUIRE(pkb.setControlVariable(3, "y"));
	REQUIRE(pkb.setControlVariable(7, "y"));
	REQUIRE(pkb.setControlVariable(9, "count"));
	REQUIRE(unordered_set<string>{ "1", "6", "9" } == pkb.getEntities(EntityType::WHILE));
	REQUIRE(unordered_set<string>{ "3", "7" } == pkb.getEntities(EntityType::IF));
	REQUIRE_FALSE(pkb.setControlVariable(0, "t"));
	REQUIRE_FALSE(pkb.setControlVariable(2, "x"));

	unordered_set<string> result1 = pkb.getSetResultsOfContainerPattern(
		EntityType::WHILE, shared_ptr<QueryInput>(new Any()));
	REQUIRE(unordered_set<string>{ "1", "9" } == result1);

	unordered_set<string> result2 = pkb.getSetResultsOfContainerPattern(
		EntityType::IF, shared_ptr<QueryInput>(new Ident("y")));
	REQUIRE(unordered_set<string>{ "3", "7" } == result2);

	unordered_set<string> result3 = pkb.getSetResultsOfContainerPattern(
		EntityType::ASSIGN, shared_ptr<QueryInput>(new Ident("count")));
	REQUIRE(unordered_set<string>{ } == result3);

	unordered_map<string, unordered_set<string>> result4 = pkb.getMapResultsOfContainerPattern(
		EntityType::WHILE, shared_ptr<QueryInput>(new Declaration(EntityType::VAR, "VAR")));
	REQUIRE(unordered_set<string>{ "x" } == result4["1"]);
	REQUIRE(unordered_set<string>{ "count" } == result4["9"]);
	REQUIRE(unordered_set<string>{ } == result4["6"]);
	REQUIRE(unordered_set<string>{ } == result4["7"]);
}

TEST_CASE("PKB withCaluse") {
	
	PKB pkb = PKB(10);
	/*
	procedure main {
		read A;
		read B;
		z = A * B + 1 * 2;
		print z;
		call A;
	}
	procedure A {
		read C;
		print A;
		call B;
	}
	procedure B {
		main = 5 + 6 + 7;
		print A;
	}
	*/
	REQUIRE(pkb.setStatementType(1, EntityType::READ));
	REQUIRE(pkb.setStatementType(2, EntityType::READ));
	REQUIRE(pkb.setStatementType(3, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(4, EntityType::PRINT));
	REQUIRE(pkb.setStatementType(5, EntityType::CALL));
	REQUIRE(pkb.setStatementType(6, EntityType::READ));
	REQUIRE(pkb.setStatementType(7, EntityType::PRINT));
	REQUIRE(pkb.setStatementType(8, EntityType::CALL));
	REQUIRE(pkb.setStatementType(9, EntityType::ASSIGN));
	REQUIRE(pkb.setStatementType(10, EntityType::PRINT));
	
	REQUIRE(pkb.insertProcedure("main"));
	REQUIRE(pkb.insertProcedure("A"));
	REQUIRE(pkb.insertProcedure("B"));
	REQUIRE(pkb.insertVariable("A"));
	REQUIRE(pkb.insertVariable("B"));
	REQUIRE(pkb.insertVariable("z"));
	REQUIRE(pkb.insertVariable("C"));
	REQUIRE(pkb.insertVariable("main"));
	REQUIRE(pkb.insertConst("1"));
	REQUIRE(pkb.insertConst("2"));
	REQUIRE(pkb.insertConst("5"));
	REQUIRE(pkb.insertConst("6"));
	REQUIRE(pkb.insertConst("7"));

	REQUIRE(pkb.insertUsedName(1, "A"));
	REQUIRE(pkb.insertUsedName(2, "B"));
	REQUIRE(pkb.insertUsedName(4, "z"));
	REQUIRE(pkb.insertUsedName(5, "A"));
	REQUIRE(pkb.insertUsedName(6, "C"));
	REQUIRE(pkb.insertUsedName(7, "A"));
	REQUIRE(pkb.insertUsedName(8, "B"));
	REQUIRE(pkb.insertUsedName(10, "A"));

	REQUIRE_FALSE(pkb.insertUsedName(3, "A"));
	REQUIRE_FALSE(pkb.insertUsedName(9, "main"));

	pkb.init();

	SECTION("getUsedName") {
		REQUIRE("A" == pkb.getNameFromStmtNum("1"));
		REQUIRE("z" == pkb.getNameFromStmtNum("4"));
		REQUIRE("C" == pkb.getNameFromStmtNum("6"));
		REQUIRE("A" == pkb.getNameFromStmtNum("7"));
	}

	SECTION("twoDeclarations") {
		// with p.procName = var.varName
		unordered_map<string, unordered_set<string>> result1 =
			pkb.getDeclarationsMatchResults(
				make_shared<Declaration>(EntityType::PROC, "p"),
				make_shared<Declaration>(EntityType::VAR, "var"));
		REQUIRE(unordered_set<string>{ "A" } == result1["A"]);
		REQUIRE(unordered_set<string>{ "B" } == result1["B"]);
		REQUIRE(unordered_set<string>{ "main" } == result1["main"]);
		REQUIRE(unordered_set<string>{ } == result1["C"]);
		REQUIRE(unordered_set<string>{ } == result1["z"]);

		// with cn.value = c.stmt#
		unordered_map<string, unordered_set<string>> result2 =
			pkb.getDeclarationsMatchResults(
				make_shared<Declaration>(EntityType::CONST, "cn"),
				make_shared<Declaration>(EntityType::CALL, "c"));
		REQUIRE(unordered_set<string>{ "5" } == result2["5"]);
		REQUIRE(unordered_set<string>{ } == result2["1"]);
		REQUIRE(unordered_set<string>{ } == result2["2"]);

		// with cn.value = r.stmt#
		unordered_map<string, unordered_set<string>> result3 =
			pkb.getDeclarationsMatchResults(
				make_shared<Declaration>(EntityType::CONST, "cn"),
				make_shared<Declaration>(EntityType::READ, "r"));
		REQUIRE(unordered_set<string>{ "1" } == result3["1"]);
		REQUIRE(unordered_set<string>{ "2" } == result3["2"]);
		REQUIRE(unordered_set<string>{ "6" } == result3["6"]);
		REQUIRE(unordered_set<string>{ } == result3["5"]);
		REQUIRE(unordered_set<string>{ } == result3["7"]);

		// with cn.value = s.stmt#
		unordered_map<string, unordered_set<string>> result4 =
			pkb.getDeclarationsMatchResults(
				make_shared<Declaration>(EntityType::CONST, "cn"),
				make_shared<Declaration>(EntityType::STMT, "s"));
		REQUIRE(unordered_set<string>{ "1" } == result4["1"]);
		REQUIRE(unordered_set<string>{ "2" } == result4["2"]);
		REQUIRE(unordered_set<string>{ "5" } == result4["5"]);
		REQUIRE(unordered_set<string>{ "6" } == result4["6"]);
		REQUIRE(unordered_set<string>{ "7" } == result4["7"]);

		// with cn.value = n
		unordered_map<string, unordered_set<string>> result5 =
			pkb.getDeclarationsMatchResults(
				make_shared<Declaration>(EntityType::CONST, "cn"),
				make_shared<Declaration>(EntityType::PROGLINE, "n"));
		REQUIRE(unordered_set<string>{ "1" } == result5["1"]);
		REQUIRE(unordered_set<string>{ "2" } == result5["2"]);
		REQUIRE(unordered_set<string>{ "5" } == result5["5"]);
		REQUIRE(unordered_set<string>{ "6" } == result5["6"]);
		REQUIRE(unordered_set<string>{ "7" } == result5["7"]);
	}

	SECTION("declarationIdent") {
		// with pn.varName = "A"
		unordered_set<string> result1 = pkb.getAttributeMatchNameResults(
			EntityType::PRINT, make_shared<Ident>("A"));
		REQUIRE(unordered_set<string>{ "7", "10" } == result1);

		// with c.procName = "A"
		unordered_set<string> result2 = pkb.getAttributeMatchNameResults(
			EntityType::CALL, make_shared<Ident>("A"));
		REQUIRE(unordered_set<string>{ "5" } == result2);

		// with r.procName = "z"
		unordered_set<string> result3 = pkb.getAttributeMatchNameResults(
			EntityType::READ, make_shared<Ident>("z"));
		REQUIRE(unordered_set<string>{ } == result3);
	}

	SECTION("declarationAttribute") {
		// with c.procName = var
		unordered_map<string, unordered_set<string>> result1 =
			pkb.getDeclarationMatchAttributeResults(
				make_shared<Declaration>(EntityType::VAR, "var"), EntityType::CALL);
		REQUIRE(unordered_set<string>{ "5" } == result1["A"]);
		REQUIRE(unordered_set<string>{ "8" } == result1["B"]);
		REQUIRE(unordered_set<string>{ } == result1["C"]);
		REQUIRE(unordered_set<string>{ } == result1["z"]);

		// with c.procName = proc
		unordered_map<string, unordered_set<string>> result2 =
			pkb.getDeclarationMatchAttributeResults(
				make_shared<Declaration>(EntityType::PROC, "proc"), EntityType::CALL);
		REQUIRE(unordered_set<string>{ "5" } == result2["A"]);
		REQUIRE(unordered_set<string>{ "8" } == result2["B"]);
		REQUIRE(unordered_set<string>{ } == result2["main"]);

		// with r.varName = proc
		unordered_map<string, unordered_set<string>> result3 =
			pkb.getDeclarationMatchAttributeResults(
				make_shared<Declaration>(EntityType::PROC, "proc"), EntityType::READ);
		REQUIRE(unordered_set<string>{ "1" } == result3["A"]);
		REQUIRE(unordered_set<string>{ "2" } == result3["B"]);
		REQUIRE(unordered_set<string>{ } == result3["C"]);
		REQUIRE(unordered_set<string>{ } == result3["main"]);

		// with r.varName = var
		unordered_map<string, unordered_set<string>> result4 =
			pkb.getDeclarationMatchAttributeResults(
				make_shared<Declaration>(EntityType::VAR, "var"), EntityType::READ);
		REQUIRE(unordered_set<string>{ "1" } == result4["A"]);
		REQUIRE(unordered_set<string>{ "2" } == result4["B"]);
		REQUIRE(unordered_set<string>{ "6" } == result4["C"]);
		REQUIRE(unordered_set<string>{ } == result4["z"]);

		// with pn.varName = var
		unordered_map<string, unordered_set<string>> result5 =
			pkb.getDeclarationMatchAttributeResults(
				make_shared<Declaration>(EntityType::VAR, "var"), EntityType::PRINT);
		REQUIRE(unordered_set<string>{ "7", "10" } == result5["A"]);
		REQUIRE(unordered_set<string>{ "4" } == result5["z"]);
		REQUIRE(unordered_set<string>{ } == result5["B"]);
		REQUIRE(unordered_set<string>{ } == result5["C"]);
	}

	SECTION("twoAttributes") {
		// with c.procName = r.varName
		unordered_map<string, unordered_set<string>> result1 =
			pkb.getAttributesMatchResults(EntityType::CALL, EntityType::READ);
		REQUIRE(unordered_set<string>{ "1" } == result1["5"]);
		REQUIRE(unordered_set<string>{ "2" } == result1["8"]);

		// with c.procName = p.varName
		unordered_map<string, unordered_set<string>> result2 =
			pkb.getAttributesMatchResults(EntityType::CALL, EntityType::PRINT);
		REQUIRE(unordered_set<string>{ "7", "10" } == result2["5"]);
		REQUIRE(unordered_set<string>{ } == result2["8"]);

		// with p.varName = c.procName
		unordered_map<string, unordered_set<string>> result3 =
			pkb.getAttributesMatchResults(EntityType::PRINT, EntityType::CALL);
		REQUIRE(unordered_set<string>{ "5" } == result3["7"]);
		REQUIRE(unordered_set<string>{ "5" } == result3["10"]);
		REQUIRE(unordered_set<string>{ } == result3["4"]);

		// with r.varName = p.varName
		unordered_map<string, unordered_set<string>> result4 =
			pkb.getAttributesMatchResults(EntityType::READ, EntityType::PRINT);
		REQUIRE(unordered_set<string>{ "7", "10" } == result4["1"]);
		REQUIRE(unordered_set<string>{ } == result4["2"]);
	}
}