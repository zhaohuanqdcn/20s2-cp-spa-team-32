#include "catch.hpp"
#include "QueryEvaluator.h"
#include "Query.h"
#include "QueryInterface.h"
#include "QueryParser.h"
#include "ResultsProjector.h"
#include "PKBStub.h"
#include "TestResultsTableUtil.h"
#include "Tokenizer.h"

TEST_CASE("First Test") {
    list<string> results;
    string input;

    SECTION("Query with only Select Clause") {
        input = "read re1, re2\t\n  ; variable\nv1,v2; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
            "stmt s1; assign\n\ta1,a2,a3; Select v1";
        auto query = std::make_shared<Query>();
        auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
        QueryParser queryParser = QueryParser{ tokenizer, query };
        queryParser.parse();

        auto pkb = make_shared<PKBStub>();
        pkb->addSetResult({"x", "y", "z", "c", "a", "b"});
        pkb->addEntityType(EntityType::VAR);
        QueryEvaluator queryEvaluator = QueryEvaluator(query, pkb);
        auto evaluatedResults = queryEvaluator.evaluate();
        ResultsProjector::projectResults(evaluatedResults, query->getSelectClause(), pkb, results);
        TestResultsTableUtil::checkList(results, { "a", "b", "c", "x", "y", "z" });

    }

    SECTION("Query with Select Clause + Such that Clause") {
        input = "print pn; stmt s1; Select s1 such that Follows*(s1, pn)";
            
        auto query = std::make_shared<Query>();
        auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
        QueryParser queryParser = QueryParser{ tokenizer, query };
        queryParser.parse();

        auto pkb = make_shared<PKBStub>();
        std::unordered_map<std::string, std::unordered_set<std::string>> expectedMap = { {"2", {"3", "4"}}, { "5", {"1", "8"} }, {"13", {"14", "15"} } };
        pkb->addSetResult({ "2", "3", "5", "7", "11", "13" });
        pkb->addMapResult(expectedMap);
        QueryEvaluator queryEvaluator = QueryEvaluator(query, pkb);
        auto evaluatedResults = queryEvaluator.evaluate();
        ResultsProjector::projectResults(evaluatedResults, query->getSelectClause(), pkb, results);
        TestResultsTableUtil::checkList(results, {"2", "13", "5" });

    }

    SECTION("Query with Select Clause + Pattern Clause") {
        input = "assign a; variable x; Select x pattern a(x, _)";

        auto query = std::make_shared<Query>();
        auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
        QueryParser queryParser = QueryParser{ tokenizer, query };
        queryParser.parse();

        auto pkb = make_shared<PKBStub>();
        std::unordered_map<std::string, std::unordered_set<std::string>> expectedMap = { {"2", {"x1", "count"}}, { "5", {"i", "x"} }, {"13", {"num8"} } };
        pkb->addSetResult({ "henz", "count", "num8", "i", "david", "x", "x2", "x1" });
        pkb->addMapResult(expectedMap);
        QueryEvaluator queryEvaluator = QueryEvaluator(query, pkb);
        auto evaluatedResults = queryEvaluator.evaluate();
        ResultsProjector::projectResults(evaluatedResults, query->getSelectClause(), pkb, results);
        TestResultsTableUtil::checkList(results, { "count", "x1", "num8", "i", "x" });

    }

    SECTION("Query with Select Clause with related synonym + both Clauses") {
        input = "assign a; variable x; stmt s; Select x such that Uses(s, x) pattern a(x, _)";

        auto query = std::make_shared<Query>();
        auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
        QueryParser queryParser = QueryParser{ tokenizer, query };
        queryParser.parse();

        auto pkb = make_shared<PKBStub>();
        std::unordered_map<std::string, std::unordered_set<std::string>> expectedPatternMap = { {"2", {"x1", "count"}}, { "5", {"i", "x"} }, {"13", {"num8"} } };
        std::unordered_map<std::string, std::unordered_set<std::string>> expectedRelationshipMap = { {"4", {"x1"}}, { "53", {"x", "martin"} }, {"5", {"bruce"}}, {"2", {"x2"}} };
        pkb->addSetResult({ "henz", "count", "num8", "i", "x", "x2", "x1", "martin", "bruce", "tuan" });
        pkb->addMapResult(expectedRelationshipMap);
        pkb->addMapResult(expectedPatternMap);
        QueryEvaluator queryEvaluator = QueryEvaluator(query, pkb);
        auto evaluatedResults = queryEvaluator.evaluate();
        ResultsProjector::projectResults(evaluatedResults, query->getSelectClause(), pkb, results);
        TestResultsTableUtil::checkList(results, { "x1", "x" });

    }

    SECTION("Query with Select Clause with unrelated synonym + both Clauses") {
        input = "assign a; variable x; stmt s; Select s such that Uses(a, x) pattern a(x, _)";

        auto query = std::make_shared<Query>();
        auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
        QueryParser queryParser = QueryParser{ tokenizer, query };
        queryParser.parse();

        auto pkb = make_shared<PKBStub>();
        std::unordered_map<std::string, std::unordered_set<std::string>> expectedPatternMap = { {"2", {"x1", "count"}}, { "5", {"i", "x"} }, {"13", {"num8"} } };
        std::unordered_map<std::string, std::unordered_set<std::string>> expectedRelationshipMap = { {"4", {"x1"}}, { "15", {"x", "martin"} }, {"5", {"x"}}, {"2", {"x2"}} };
        pkb->addSetResult({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15" });
        pkb->addMapResult(expectedRelationshipMap);
        pkb->addMapResult(expectedPatternMap);
        pkb->addEntityType(EntityType::STMT);
        QueryEvaluator queryEvaluator = QueryEvaluator(query, pkb);
        auto evaluatedResults = queryEvaluator.evaluate();
        ResultsProjector::projectResults(evaluatedResults, query->getSelectClause(), pkb, results);
        TestResultsTableUtil::checkList(results, { "1", "10", "11", "12", "13", "14", "15", "2", "3", "4", "5", "6", "7", "8", "9" });

    }

    SECTION("Query with Select Clause + multiple With clause ") {
        input = "constant c; stmt s; Select s with s.stmt# = c.value and s.stmt# = 8";

        auto query = std::make_shared<Query>();
        auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
        QueryParser queryParser = QueryParser{ tokenizer, query };
        queryParser.parse();

        auto pkb = make_shared<PKBStub>();
        std::unordered_map<std::string, std::unordered_set<std::string>> withClauseMap = { {"2", {"2"}}, { "4", {"4"} }, {"8", {"8"} } };
        pkb->addMapResult(withClauseMap);
        pkb->addSetResult({ "1", "2", "3", "4", "5", "6", "7", "8" });
        pkb->addEntityType(EntityType::STMT);

        QueryEvaluator queryEvaluator = QueryEvaluator(query, pkb);
        auto evaluatedResults = queryEvaluator.evaluate();
        ResultsProjector::projectResults(evaluatedResults, query->getSelectClause(), pkb, results);
        TestResultsTableUtil::checkList(results, { "8" });

    }

    SECTION("Query with Select Clause + multiple With clause ") {
        input = "stmt s; procedure p; variable v; assign a; Select s with s.stmt# = a.stmt# and s.stmt# = 6 such that"
            " Modifies(a, v) with v.varName = p.procName";

        auto query = std::make_shared<Query>();
        auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
        QueryParser queryParser = QueryParser{ tokenizer, query };
        queryParser.parse();

        auto pkb = make_shared<PKBStub>();
        std::unordered_set<std::string> stmtAssignWithClause = { "2", "4", "6", "8" };
        std::unordered_map<std::string, std::unordered_set<std::string>> modifiesMap = { {"2", {"var1"}}, { "4", {"var2"} },
                {"8", {"var2"}}, {"6", {"var3"} } };
        std::unordered_map<std::string, std::unordered_set<std::string>> varProcMap = { {"var1", {"var1"}}, { "var3", {"var3"} }
                ,{ "var5", {"var5"} } };
        pkb->addSetResult(stmtAssignWithClause);
        pkb->addEntityType(EntityType::ASSIGN);
        pkb->addSetResult({ "5", "6", "7", "8" });
        pkb->addEntityType(EntityType::STMT);

        pkb->addMapResult(modifiesMap);
        pkb->addMapResult(varProcMap);
        QueryEvaluator queryEvaluator = QueryEvaluator(query, pkb);
        auto evaluatedResults = queryEvaluator.evaluate();
        ResultsProjector::projectResults(evaluatedResults, query->getSelectClause(), pkb, results);
        TestResultsTableUtil::checkList(results, { "6" });

    }

    SECTION("Query with Select Clause + multiple With clause ") {
        input = "stmt s; procedure p; variable v; assign a; Select s with s.stmt# = a.stmt# such that"
            " Modifies(a, v) with v.varName = p.procName";

        auto query = std::make_shared<Query>();
        auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
        QueryParser queryParser = QueryParser{ tokenizer, query };
        queryParser.parse();

        auto pkb = make_shared<PKBStub>();
        std::unordered_set<std::string> stmtAssignWithClause = { "2", "4", "6", "8" };
        std::unordered_map<std::string, std::unordered_set<std::string>> modifiesMap = { {"2", {"var1"}}, { "4", {"var2"} },
                {"8", {"var2"}}, {"6", {"var3"} } };
        std::unordered_map<std::string, std::unordered_set<std::string>> varProcMap = { {"var1", {"var1"}}, { "var3", {"var3"} }
                ,{ "var5", {"var5"} } };
        pkb->addSetResult(stmtAssignWithClause);
        pkb->addEntityType(EntityType::ASSIGN);
        pkb->addMapResult(modifiesMap);
        pkb->addMapResult(varProcMap);
        QueryEvaluator queryEvaluator = QueryEvaluator(query, pkb);
        auto evaluatedResults = queryEvaluator.evaluate();
        ResultsProjector::projectResults(evaluatedResults, query->getSelectClause(), pkb, results);
        TestResultsTableUtil::checkList(results, { "2", "6" });

    }
}

