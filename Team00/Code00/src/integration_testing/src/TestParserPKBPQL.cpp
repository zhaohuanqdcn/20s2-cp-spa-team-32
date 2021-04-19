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
#include "Parser.h"
#include "DesignExtractor.h"
#include "DesignExtractorHelper.h"


TEST_CASE("second test") {
    vector<string> codes = {
    "procedure main {",
        "while (1 == 2) {",
        "m = 1;",
        "while (x == 0) {",
        "while (2 > 3) {",
        "call first;",
        "}}}}",
    "procedure first {",
        "while (y + 1 == t) {",
        "call second;",
        "}}",
    "procedure second {",
        "count = 0;",
        "count = count + z;",
        "a = a * b - c / d + e - f * g % h;",
        "}"
    };
     
    SIMPLETokenStream stream{ codes };
    SIMPLETokenStream secondStream{ stream };

    DesignExtractor extractor;
    Parser parser{ extractor };

    auto error = parser.parseProgram(stream);
    cerr << error.getErrorMessage() << endl;
    REQUIRE_FALSE(error.hasError());
    shared_ptr<PKB> pkb = extractor.extractToPKB();

    // auto expressions = extractor.getExpression(10);
    // cerr << "All the expressions of 10" << endl;
    // for (auto ex: expressions) {
    //     cerr << ex.getValue() << endl;
    // }
    // cerr << "End of getting expressions" << endl;
 
    SECTION("assignPattern1") {
        // select a pattern a("count", _"count"_)
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::ASSIGN, "a");
        shared_ptr<QueryInput> variable =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("a"));
        shared_ptr<Expression> expression = 
            make_shared<Expression>("(a*b)", ExpressionType::PARTIAL);

        query->addDeclarationToSelectClause(declaration);
        query->addAssignPatternClause(declaration, variable, expression);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "10" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    } 
}

TEST_CASE("First test") {
    vector<string> codes = {
    "procedure main {",
        "while (1 == 2) {",
        "m = 1;",
        "while (x == 0) {",
        "while (2 > 3) {",
        "call first;",
        "}}}}",
    "procedure first {",
        "while (y + 1 == t) {",
        "call second;",
        "}}",
    "procedure second {",
        "count = 0;",
        "count = count + z;",
        "count = count + (10 * 2);",
        "}"
    };
     
    SIMPLETokenStream stream{ codes };
    SIMPLETokenStream secondStream{ stream };

    DesignExtractor extractor;
    Parser parser{ extractor };

    auto error = parser.parseProgram(stream);
    REQUIRE_FALSE(error.hasError());
    shared_ptr<PKB> pkb = extractor.extractToPKB();

    // auto expressions = extractor.getExpression(10);
    // cerr << "All the expressions of 10" << endl;
    // for (auto ex: expressions) {
    //     cerr << ex.getValue() << endl;
    // }
    // cerr << "End of getting expressions" << endl;
 
    SECTION("assignPattern1") {
        // select a pattern a("count", _"count"_)
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::ASSIGN, "a");
        shared_ptr<QueryInput> variable =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("count"));
        shared_ptr<Expression> expression = 
            make_shared<Expression>("(10*2)", ExpressionType::PARTIAL);

        query->addDeclarationToSelectClause(declaration);
        query->addAssignPatternClause(declaration, variable, expression);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "10" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    } 
}

TEST_CASE("Test multipleProcedures") {
    vector<string> codes = {
    "procedure main {",
        "while (1 == 2) {",
        "m = 1;",
        "while (x == 0) {",
        "while (2 > 3) {",
        "call first;",
        "}}}}",
    "procedure first {",
        "while (y + 1 == t) {",
        "call second;",
        "}}",
    "procedure second {",
        "count = 0;",
        "count = count + z;",
        "count = count + (10 * 2);",
        "}"
    };
    SIMPLETokenStream stream{ codes };
    SIMPLETokenStream secondStream{ stream };

    DesignExtractor extractor;
    Parser parser{ extractor };

    auto error = parser.parseProgram(stream);
    REQUIRE_FALSE(error.hasError());
    shared_ptr<PKB> pkb = extractor.extractToPKB();


    
    SECTION("followsT") {
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

        list<string> expected{};
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    }


    SECTION("call Star") {
        // select p such that Call*(p, "second")
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::PROC, "p");

        shared_ptr<QueryInput> caller =
            dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, "p"));
        shared_ptr<QueryInput> callee =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("second"));

        query->addDeclarationToSelectClause(declaration);
        query->addRelationshipClause(RelationshipType::CALLS_T, caller, callee);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{"main", "first" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    } 
    
    SECTION("stmtUses") {
        // select s such that uses(s, "count")
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::STMT, "s");

        shared_ptr<QueryInput> stmt =
            dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, "s"));
        shared_ptr<QueryInput> variable =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("count"));

        query->addDeclarationToSelectClause(declaration);
        query->addRelationshipClause(RelationshipType::USES, stmt, variable);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "1", "3", "4", "5", "6", "7", "9", "10" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        // cerr << "using " << endl;
        // for (auto x: result) {
        //     cerr << x << " ";
        // }
        // cerr << endl;
        TestResultsTableUtil::checkList(result, expected);
    }

    SECTION("stmtModifies") {
        // select s such that modifies(s, "count")
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::STMT, "s");

        shared_ptr<QueryInput> stmt =
            dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::STMT, "s"));
        shared_ptr<QueryInput> variable =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("count"));

        query->addDeclarationToSelectClause(declaration);
        query->addRelationshipClause(RelationshipType::MODIFIES, stmt, variable);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "1", "3", "4", "5", "6", "7", "8", "9", "10"};
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        // cerr << "using " << endl;
        // for (auto x: result) {
        //     cerr << x << " ";
        // }
        // cerr << endl;
        TestResultsTableUtil::checkList(result, expected);
    }

    SECTION("procUses") {
        // select p such that uses(p, "count")
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::PROC, "p");

        shared_ptr<QueryInput> proc =
            dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, "p"));
        shared_ptr<QueryInput> variable =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("count"));

        query->addDeclarationToSelectClause(declaration);
        query->addRelationshipClause(RelationshipType::USES, proc, variable);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "main", "first", "second" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    }

    SECTION("procCallsT") {
        // select p such that calls*("main", p)
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::PROC, "p");

        shared_ptr<QueryInput> proc =
            dynamic_pointer_cast<QueryInput>(make_shared<Declaration>(EntityType::PROC, "p"));
        shared_ptr<QueryInput> procName =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("main"));

        query->addDeclarationToSelectClause(declaration);
        query->addRelationshipClause(RelationshipType::CALLS_T, procName, proc);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "first", "second" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    }

    SECTION("assignPattern") {
        // select a pattern a("count", _"count"_)
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::ASSIGN, "a");
        shared_ptr<QueryInput> variable =
            dynamic_pointer_cast<QueryInput>(make_shared<Ident>("count"));
        shared_ptr<Expression> expression = 
            make_shared<Expression>("count", ExpressionType::PARTIAL);

        query->addDeclarationToSelectClause(declaration);
        query->addAssignPatternClause(declaration, variable, expression);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "9", "10" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    }

    SECTION("containerPattern") {
        // select w pattern w(_, _)
        shared_ptr<QueryInterface> query =
            dynamic_pointer_cast<QueryInterface>(make_shared<Query>());
        shared_ptr<Declaration> declaration =
            make_shared<Declaration>(EntityType::WHILE, "w");
        shared_ptr<QueryInput> wildcard = make_shared<Any>();

        query->addDeclarationToSelectClause(declaration);
        query->addContainerPatternClause(declaration, wildcard);

        QueryEvaluator qe = QueryEvaluator(query, pkb);

        list<string> expected{ "3", "6" };
        list<string> result{ };
        shared_ptr<ResultsTable> resultsTable = qe.evaluate();
        ResultsProjector::projectResults(resultsTable, query->getSelectClause(), pkb, result);
        TestResultsTableUtil::checkList(result, expected);
    }

}


TEST_CASE("Test getNameFromStmt") {
    vector<string> codes = {
        "procedure main {\
            read A;\
            read B;\
            z = A * B + 1;\
            print z;\
            call A;\
        }\
        procedure A{\
            read C;\
            print A;\
            call B;\
        }\
        procedure B{\
            main = A * B;\
            print A;\
        }"
    };

    SIMPLETokenStream stream{ codes };
    SIMPLETokenStream secondStream{ stream };

    DesignExtractor extractor;
    Parser parser{ extractor };

    auto error = parser.parseProgram(stream);
    REQUIRE_FALSE(error.hasError());
    shared_ptr<PKB> pkb = extractor.extractToPKB();

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
        shared_ptr<Expression> exp = make_shared<Expression>("(A*B)", ExpressionType::PARTIAL);
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
        shared_ptr<Expression> exp = make_shared<Expression>("(A*B)", ExpressionType::PARTIAL);
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
}