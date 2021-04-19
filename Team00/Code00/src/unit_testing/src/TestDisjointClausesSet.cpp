// #include "DisjointClausesSet.h"
// #include "TestResultsTableUtil.h"
// #include "catch.hpp"
// #include "OptionalClause.h"
// #include "RelationshipClause.h"
// #include "PatternClause.h"
// #include "WithClause.h"
// #include "Declaration.h"
// #include "StmtNum.h"
// #include "Expression.h"
// #include "Ident.h"

// TEST_CASE("No clauses") {
// 	DisjointClausesSet disjointClauseSet = DisjointClausesSet({});
// 	vector<vector<shared_ptr<OptionalClause>>> actualGroups = disjointClauseSet.getClauses();
// 	vector<vector<shared_ptr<OptionalClause>>> expectedGroups = {};
// 	TestResultsTableUtil::checkClauseGroups(actualGroups, expectedGroups);
// }

// // Follows(s, a) With v = 10 Pattern a(v, _) 
// TEST_CASE("One clause group") {
// 	shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, "s"));
// 	shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, "a"));
// 	shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, "a"));
// 	shared_ptr<QueryInput> ten = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("10"));
// 	shared_ptr<Expression> expr = make_shared<Expression>("_", ExpressionType::EMPTY);

// 	shared_ptr<RelationshipClause> followsClause = make_shared<RelationshipClause>(RelationshipType::FOLLOWS, stmt, assign);
// 	shared_ptr<PatternClause> assignPatternClause = make_shared<PatternClause>(dynamic_pointer_cast<Declaration>(assign), var, expr);
// 	shared_ptr<WithClause> withClause = make_shared<WithClause>(var, ten);

// 	DisjointClausesSet disjointClauseSet = DisjointClausesSet({ followsClause, withClause, assignPatternClause });
// 	vector<vector<shared_ptr<OptionalClause>>> actualGroups = disjointClauseSet.getClauses();
// 	vector<vector<shared_ptr<OptionalClause>>> expectedGroups = { { followsClause, withClause, assignPatternClause } };
// 	TestResultsTableUtil::checkClauseGroups(actualGroups, expectedGroups);
// }

// // Follows(s, a), With v = "x", Parent(3, 4), pattern w(v1, _), Follows(1, 2), Pattern a(v, _"x"_), Follows*(1, 10), With s = s, pattern ifs(v1, _, _)
// TEST_CASE("Multiple clause groups") {
// 	shared_ptr<QueryInput> stmt = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, "s"));
// 	shared_ptr<QueryInput> assign = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::ASSIGN, "a"));
// 	shared_ptr<QueryInput> ifs = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::IF, "ifs"));
// 	shared_ptr<QueryInput> wh = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::WHILE, "w"));
// 	shared_ptr<QueryInput> var = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, "v"));
// 	shared_ptr<QueryInput> var1 = dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::VAR, "v1"));

// 	shared_ptr<QueryInput> one = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("1"));
// 	shared_ptr<QueryInput> two = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("2"));
// 	shared_ptr<QueryInput> three = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("3"));
// 	shared_ptr<QueryInput> four = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("4"));
// 	shared_ptr<QueryInput> ten = dynamic_pointer_cast<QueryInput>(make_shared<StmtNum>("10"));

// 	shared_ptr<QueryInput> identX = dynamic_pointer_cast<QueryInput>(make_shared<Ident>("x"));

// 	shared_ptr<Expression> expr = make_shared<Expression>("x", ExpressionType::PARTIAL);

// 	shared_ptr<RelationshipClause> followsStmtAssign = make_shared<RelationshipClause>(RelationshipType::FOLLOWS, stmt, assign);
// 	shared_ptr<WithClause> withVar = make_shared<WithClause>(var, identX);
// 	shared_ptr<RelationshipClause> parentThreeFour = make_shared<RelationshipClause>(RelationshipType::PARENT, three, four);
// 	shared_ptr<PatternClause> whilePattern = make_shared<PatternClause>(dynamic_pointer_cast<Declaration>(wh), var1);
// 	shared_ptr<RelationshipClause> followsOneTwo = make_shared<RelationshipClause>(RelationshipType::FOLLOWS, one, two);
// 	shared_ptr<PatternClause> assignPatternClause = make_shared<PatternClause>(dynamic_pointer_cast<Declaration>(assign), var, expr);
// 	shared_ptr<RelationshipClause> followsTOneTen = make_shared<RelationshipClause>(RelationshipType::FOLLOWS_T, one, ten);
// 	shared_ptr<WithClause> withStmt = make_shared<WithClause>(stmt, stmt);
// 	shared_ptr<PatternClause> ifPattern = make_shared<PatternClause>(dynamic_pointer_cast<Declaration>(ifs), var1);

// 	DisjointClausesSet disjointClauseSet = DisjointClausesSet({ 
// 		followsStmtAssign, withVar, parentThreeFour, whilePattern, followsOneTwo, assignPatternClause, followsTOneTen, withStmt, ifPattern });
// 	vector<vector<shared_ptr<OptionalClause>>> actualGroups = disjointClauseSet.getClauses();
// 	vector<vector<shared_ptr<OptionalClause>>> expectedGroups = {
// 		//{ parentThreeFour, followsOneTwo, followsTOneTen },
// 		{ followsStmtAssign, assignPatternClause, withVar, withStmt },
// 		{ ifPattern, whilePattern  } };
// 	TestResultsTableUtil::checkClauseGroups(actualGroups, expectedGroups);
// }