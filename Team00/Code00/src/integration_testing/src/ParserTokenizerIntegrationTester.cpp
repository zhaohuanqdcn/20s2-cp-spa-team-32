#include "catch.hpp"
#include "QueryParser.h"
#include "Tokenizer.h"
#include "Query.h"
#include "SemanticException.h"
#include "SyntacticException.h"
#include <vector>
#include <memory>

// ================= Positive Test Cases (No exceptions thrown) =================

TEST_CASE("Test whether Declared Synonyms are Stored Correctly")
{
	std::string input = "read re1, re2\t\n  ; variable\nv1,v2; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta1,a2,a3; prog_line pgl1; prog_line pgl2; call cl1, cl2; Select v1";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re1"] == EntityType::READ);
	REQUIRE(synonyms["re2"] == EntityType::READ);
	REQUIRE(synonyms["v1"] == EntityType::VAR);
	REQUIRE(synonyms["v2"] == EntityType::VAR);
	REQUIRE(synonyms["c"] == EntityType::CONST);
	REQUIRE(synonyms["pcd"] == EntityType::PROC);
	REQUIRE(synonyms["pn"] == EntityType::PRINT);
	REQUIRE(synonyms["w"] == EntityType::WHILE);
	REQUIRE(synonyms["ifs"] == EntityType::IF);
	REQUIRE(synonyms["s1"] == EntityType::STMT);
	REQUIRE(synonyms["a1"] == EntityType::ASSIGN);
	REQUIRE(synonyms["a2"] == EntityType::ASSIGN);
	REQUIRE(synonyms["a3"] == EntityType::ASSIGN);
	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);
	REQUIRE(synonyms["pgl2"] == EntityType::PROGLINE);
	REQUIRE(synonyms["cl1"] == EntityType::CALL);
	REQUIRE(synonyms["cl2"] == EntityType::CALL);


	std::shared_ptr<Declaration> declaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(declaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(declaration->getEntityType() == EntityType::VAR);
	REQUIRE(declaration->getValue() == "v1");
}

TEST_CASE("Test whether Declared Synonyms are Stored Correctly 2")
{
    std::string input = "prog_line p1; Select p1";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    queryParser.parse();
    std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

    REQUIRE(synonyms["p1"] == EntityType::PROGLINE);


    std::shared_ptr<Declaration> declaration = query->getSelectClause()->getDeclarations().at(0);
    REQUIRE(declaration->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(declaration->getEntityType() == EntityType::PROGLINE);
    REQUIRE(declaration->getValue() == "p1");
}

// ----------------- SelectCl + 1 PatternCl -----------------

TEST_CASE("Test Query with Select And Pattern Clause1")
{
	std::string input = "assign a; \nSelect a pattern a(_, _\"9\"_) ";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	//INFO(Token::EntityTypeToString(synonyms["a"]));

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(0));
	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
	std::shared_ptr<Expression> expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(patternQueryInput->getValue() == "_");
	REQUIRE(expression->getValue() == "9");
}

// ----------------- SelectCl + 1 SuchThatCl -----------------

TEST_CASE("Test Query with Select And Such That Clause1")
{
	std::string input = "stmt s; print pn; \nSelect s such that Follows* (pn, s) ";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["s"]));
	INFO(Token::EntityTypeToString(synonyms["pn"]));*/

	REQUIRE(synonyms["pn"] == EntityType::PRINT);
	REQUIRE(synonyms["s"] == EntityType::STMT);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::STMT);
	REQUIRE(selectClDeclaration->getValue() == "s");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS_T);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "pn");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::PRINT);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "s");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::STMT);
}

TEST_CASE("Test Query with Select And Such That Clause2")
{
	std::string input = "variable v; \nSelect v such that Modifies (80, \"aVariable123\") ";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["v"]));*/

	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::VAR);
	REQUIRE(selectClDeclaration->getValue() == "v");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "80");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(rightQueryInput->getValue() == "aVariable123");
}

// ----------------- Test Follows -----------------

TEST_CASE("Test Follows(Any, Any)")
{
	std::string input = "assign a; \nSelect a such that Follows(_, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	//INFO(Token::EntityTypeToString(synonyms["a"]));

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(leftQueryInput->getValue() == "_");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Follows(Any, Synonym)")
{
	std::string input = "assign a; if ifs; \nSelect a such that Follows(_, ifs)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["ifs"] == EntityType::IF);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(leftQueryInput->getValue() == "_");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "ifs");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::IF);
}

TEST_CASE("Test Follows(Any, Integer)")
{
	std::string input = "print pn;\nSelect pn such that Follows(_, 999)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pn"] == EntityType::PRINT);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PRINT);
	REQUIRE(selectClDeclaration->getValue() == "pn");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(leftQueryInput->getValue() == "_");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(rightQueryInput->getValue() == "999");
}

TEST_CASE("Test Follows(Synonym, Any)")
{
	std::string input = "read re;\nSelect re such that Follows(re, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Follows(Synonym, Synonym)")
{
	std::string input = "read re; stmt s1;\nSelect re such that Follows(re, s1)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);
	REQUIRE(synonyms["s1"] == EntityType::STMT);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "s1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::STMT);
}

TEST_CASE("Test Follows(Synonym, Integer)")
{
	std::string input = "read re;\nSelect re such that Follows(re, 999)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(rightQueryInput->getValue() == "999");
}

TEST_CASE("Test Follows(Integer, _)")
{
	std::string input = "while w;\nSelect w such that Follows(3203, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "3203");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Follows(Integer, Synonym)")
{
	std::string input = "while w;\nSelect w such that Follows(3203, w)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "3203");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::WHILE);
}

TEST_CASE("Test Follows(Integer, Integer)")
{
	std::string input = "while w;\nSelect w such that Follows(3203, 4000)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "3203");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(rightQueryInput->getValue() == "4000");
}

// ----------------- Test Parent -----------------

TEST_CASE("Test Parent(Any, Any)")
{
	std::string input = "assign a; \nSelect a such that Parent(_, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	//INFO(Token::EntityTypeToString(synonyms["a"]));

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(leftQueryInput->getValue() == "_");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Parent(Any, Synonym)")
{
	std::string input = "assign a; if ifs; \nSelect a such that Parent(_, ifs)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["ifs"] == EntityType::IF);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(leftQueryInput->getValue() == "_");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "ifs");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::IF);
}

TEST_CASE("Test Parent(Any, Integer)")
{
	std::string input = "print pn;\nSelect pn such that Parent(_, 999)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pn"] == EntityType::PRINT);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PRINT);
	REQUIRE(selectClDeclaration->getValue() == "pn");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(leftQueryInput->getValue() == "_");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(rightQueryInput->getValue() == "999");
}

TEST_CASE("Test Parent(Synonym, Any)")
{
	std::string input = "while w;\nSelect w such that Parent(w, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::WHILE);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Parent(Synonym, Synonym)")
{
	std::string input = "while w; stmt s1;\nSelect w such that Parent(w, s1)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);
	REQUIRE(synonyms["s1"] == EntityType::STMT);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::WHILE);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "s1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::STMT);
}

TEST_CASE("Test Parent(Synonym, Integer)")
{
	std::string input = "while w;\nSelect w such that Parent(w, 999)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::WHILE);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(rightQueryInput->getValue() == "999");
}

TEST_CASE("Test Parent(Integer, _)")
{
	std::string input = "while w;\nSelect w such that Parent(3203, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "3203");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Parent(Integer, Synonym)")
{
	std::string input = "while w;\nSelect w such that Parent(3203, w)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "3203");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::WHILE);
}

TEST_CASE("Test Parent(Integer, Integer)")
{
	std::string input = "while w;\nSelect w such that Parent(3203, 4000)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "3203");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(rightQueryInput->getValue() == "4000");
}

// ----------------- Test Modifies -----------------

TEST_CASE("Test Modifies(Synonym, Any)")
{
	std::string input = "read re;\nSelect re such that Modifies(re, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Modifies(Synonym, Synonym)")
{
	std::string input = "read re; variable v;\nSelect re such that Modifies(re, v)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);
	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::VAR);
}

TEST_CASE("Test Modifies(Synonym, \"Ident\")")
{
	std::string input = "read re;\nSelect re such that Modifies(re, \"hello1\")";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["re"] == EntityType::READ);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::READ);
	REQUIRE(selectClDeclaration->getValue() == "re");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "re");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(rightQueryInput->getValue() == "hello1");
}

TEST_CASE("Test Modifies(Integer, _)")
{
	std::string input = "while w;\nSelect w such that Modifies(3203, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "3203");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Modifies(Integer, Synonym)")
{
	std::string input = "variable v;\nSelect v such that Modifies(3203, v)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::VAR);
	REQUIRE(selectClDeclaration->getValue() == "v");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "3203");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::VAR);
}

TEST_CASE("Test Modifies(Integer, Ident)")
{
	std::string input = "assign a;\nSelect a such that Modifies(3203, \"aVariable\")";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "3203");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(rightQueryInput->getValue() == "aVariable");
}

// ----------------- Test Uses -----------------

TEST_CASE("Test Uses(Synonym, Any)")
{
	std::string input = "print pn;\nSelect pn such that Uses(pn, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pn"] == EntityType::PRINT);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PRINT);
	REQUIRE(selectClDeclaration->getValue() == "pn");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "pn");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::PRINT);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Uses(Synonym, Synonym)")
{
	std::string input = "print pn; variable v;\nSelect pn such that Uses(pn, v)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pn"] == EntityType::PRINT);
	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PRINT);
	REQUIRE(selectClDeclaration->getValue() == "pn");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "pn");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::PRINT);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::VAR);
}

TEST_CASE("Test Uses(Synonym, \"Ident\")")
{
	std::string input = "print pn;\nSelect pn such that Uses(pn, \"hello1\")";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pn"] == EntityType::PRINT);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PRINT);
	REQUIRE(selectClDeclaration->getValue() == "pn");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "pn");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::PRINT);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(rightQueryInput->getValue() == "hello1");
}

TEST_CASE("Test Uses(Integer, _)")
{
	std::string input = "while w;\nSelect w such that Uses(3203, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["w"] == EntityType::WHILE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "3203");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Uses(Integer, Synonym)")
{
	std::string input = "variable v;\nSelect v such that Uses(3203, v)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::VAR);
	REQUIRE(selectClDeclaration->getValue() == "v");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "3203");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::VAR);
}

TEST_CASE("Test Uses(Integer, Ident)")
{
	std::string input = "assign a;\nSelect a such that Uses(3203, \"aVariable\")";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "3203");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(rightQueryInput->getValue() == "aVariable");
}

// ----------------- Test Calls -----------------

TEST_CASE("Test Calls(Any, Any)")
{
	std::string input = "procedure pcd1;\nSelect pcd1 such that Calls(_, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pcd1"] == EntityType::PROC);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROC);
	REQUIRE(selectClDeclaration->getValue() == "pcd1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(leftQueryInput->getValue() == "_");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Calls(Synonym, Any)")
{
	std::string input = "procedure pcd1;\nSelect pcd1 such that Calls(pcd1, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pcd1"] == EntityType::PROC);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROC);
	REQUIRE(selectClDeclaration->getValue() == "pcd1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "pcd1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::PROC);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Calls(\"Ident\", Any)")
{
	std::string input = "procedure pcd1;\nSelect pcd1 such that Calls(\"findDist\", _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pcd1"] == EntityType::PROC);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROC);
	REQUIRE(selectClDeclaration->getValue() == "pcd1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(leftQueryInput->getValue() == "findDist");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Calls(Any, Synonym)")
{
	std::string input = "procedure pcd1;\nSelect pcd1 such that Calls(_, pcd1)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pcd1"] == EntityType::PROC);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROC);
	REQUIRE(selectClDeclaration->getValue() == "pcd1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(leftQueryInput->getValue() == "_");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "pcd1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::PROC);
}

TEST_CASE("Test Calls(Synonym, Synonym)")
{
	std::string input = "procedure pcd1, pcd2;\nSelect pcd1 such that Calls(pcd1, pcd2)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pcd1"] == EntityType::PROC);
	REQUIRE(synonyms["pcd2"] == EntityType::PROC);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROC);
	REQUIRE(selectClDeclaration->getValue() == "pcd1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "pcd1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::PROC);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "pcd2");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::PROC);
}

TEST_CASE("Test Calls(\"Ident\", Synonym)")
{
	std::string input = "procedure pcd1;\nSelect pcd1 such that Calls(\"calculateLength\", pcd1)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pcd1"] == EntityType::PROC);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROC);
	REQUIRE(selectClDeclaration->getValue() == "pcd1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(leftQueryInput->getValue() == "calculateLength");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "pcd1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::PROC);
}

TEST_CASE("Test Calls(Any, \"Ident\")")
{
	std::string input = "procedure pcd1;\nSelect pcd1 such that Calls(_, \"findDist\")";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pcd1"] == EntityType::PROC);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROC);
	REQUIRE(selectClDeclaration->getValue() == "pcd1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(leftQueryInput->getValue() == "_");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(rightQueryInput->getValue() == "findDist");
}

TEST_CASE("Test Calls(Synonym, \"Ident\")")
{
	std::string input = "procedure pcd1;\nSelect pcd1 such that Calls(pcd1, \"calculateLength\")";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pcd1"] == EntityType::PROC);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROC);
	REQUIRE(selectClDeclaration->getValue() == "pcd1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "pcd1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::PROC);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(rightQueryInput->getValue() == "calculateLength");
}

TEST_CASE("Test Calls( \"Ident\",  \"Ident\")")
{
	std::string input = "procedure pcd1;\nSelect pcd1 such that Calls(\"findMax\", \"calculateLength\")";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pcd1"] == EntityType::PROC);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROC);
	REQUIRE(selectClDeclaration->getValue() == "pcd1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(leftQueryInput->getValue() == "findMax");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(rightQueryInput->getValue() == "calculateLength");
}

// ----------------- Test Next -----------------

TEST_CASE("Test Next(Any, Any)")
{
	std::string input = "prog_line pgl1;\nSelect pgl1 such that Next(_, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROGLINE);
	REQUIRE(selectClDeclaration->getValue() == "pgl1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(leftQueryInput->getValue() == "_");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Next(Synonym, Any)")
{
	std::string input = "prog_line pgl1;\nSelect pgl1 such that Next(pgl1, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROGLINE);
	REQUIRE(selectClDeclaration->getValue() == "pgl1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "pgl1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::PROGLINE);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Next(Integer, Any)")
{
	std::string input = "prog_line pgl1, pgl2;\nSelect pgl1 such that Next*(777, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);
	REQUIRE(synonyms["pgl2"] == EntityType::PROGLINE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROGLINE);
	REQUIRE(selectClDeclaration->getValue() == "pgl1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT_T);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "777");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(rightQueryInput->getValue() == "_");
}

TEST_CASE("Test Next(Any, Synonym)")
{
	std::string input = "prog_line pgl1;\nSelect pgl1 such that Next*(_, pgl1)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROGLINE);
	REQUIRE(selectClDeclaration->getValue() == "pgl1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT_T);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(leftQueryInput->getValue() == "_");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "pgl1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::PROGLINE);
}

TEST_CASE("Test Next(Synonym, Synonym)")
{
	std::string input = "prog_line pgl1, pgl2;\nSelect pgl1 such that Next*(pgl1, pgl2)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);
	REQUIRE(synonyms["pgl2"] == EntityType::PROGLINE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROGLINE);
	REQUIRE(selectClDeclaration->getValue() == "pgl1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT_T);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "pgl1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::PROGLINE);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "pgl2");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::PROGLINE);
}

TEST_CASE("Test Next(Integer, Synonym)")
{
	std::string input = "prog_line pgl1;\nSelect pgl1 such that Next(82, pgl1)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROGLINE);
	REQUIRE(selectClDeclaration->getValue() == "pgl1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "82");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(rightQueryInput->getValue() == "pgl1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::PROGLINE);
}

TEST_CASE("Test Next(Any, Integer)")
{
	std::string input = "prog_line pgl1;\nSelect pgl1 such that Next*(_, 32)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROGLINE);
	REQUIRE(selectClDeclaration->getValue() == "pgl1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT_T);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(leftQueryInput->getValue() == "_");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(rightQueryInput->getValue() == "32");
}

TEST_CASE("Test Next(Synonym, Integer)")
{
	std::string input = "prog_line pgl1;\nSelect pgl1 such that Next*(pgl1, 77)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROGLINE);
	REQUIRE(selectClDeclaration->getValue() == "pgl1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT_T);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(leftQueryInput->getValue() == "pgl1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(leftQueryInput)->getEntityType() == EntityType::PROGLINE);
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(rightQueryInput->getValue() == "77");
}

TEST_CASE("Test Next(Integer, Integer)")
{
	std::string input = "prog_line pgl1;\nSelect pgl1 such that Next(55, 555)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROGLINE);
	REQUIRE(selectClDeclaration->getValue() == "pgl1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT);
	REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(leftQueryInput->getValue() == "55");
	REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(rightQueryInput->getValue() == "555");
}

// ----------------- AffectsBip ---------------------------

TEST_CASE("Test Affects(Integer, Synonym)")
{
    std::string input = "assign a;\nSelect a such that AffectsBip(17, a)";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    queryParser.parse();
    std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

    REQUIRE(synonyms["a"] == EntityType::ASSIGN);

    std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
    REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
    REQUIRE(selectClDeclaration->getValue() == "a");

    std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
    RelationshipType relationshipType = relationshipCl->getRelationshipType();
    std::shared_ptr<QueryInput> leftQueryInput = relationshipCl->getLeftInput();
    std::shared_ptr<QueryInput> rightQueryInput = relationshipCl->getRightInput();
    REQUIRE(relationshipType == RelationshipType::AFFECTSBIP);
    REQUIRE(leftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
    REQUIRE(leftQueryInput->getValue() == "17");
    REQUIRE(rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(rightQueryInput->getValue() == "a");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(rightQueryInput)->getEntityType() == EntityType::ASSIGN);
}

// ----------------- Test Edge cases of handling Relationship* Tokens when parsing expression -----------

TEST_CASE("Test Edge cases of handling Relationship* Tokens when parsing expression")
{
    std::string input = "assign a; \nSelect a pattern a(\"x\", \"Follows* Parent* (3 + Uses*var1) / AffectsBip*Modifies*Calls*Next*NextBip*4/ 6 - Affects*2 - 3\")";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    queryParser.parse();
    std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

    /*INFO(Token::EntityTypeToString(synonyms["w"]));
    INFO(Token::EntityTypeToString(synonyms["a"]));*/

    REQUIRE(synonyms["a"] == EntityType::ASSIGN);

    std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
    REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
    REQUIRE(selectClDeclaration->getValue() == "a");

    std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(0));
    std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
    std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
    std::shared_ptr<Expression> expression = patternCl->getExpression();
    REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(patternClDeclaration->getValue() == "a");
    REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
    REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
    REQUIRE(patternQueryInput->getValue() == "x");
    REQUIRE(expression->getValue() == "(((((((((((Follows*Parent)*(3+(Uses*var1)))/AffectsBip)*Modifies)*Calls)*Next)*NextBip)*4)/6)-(Affects*2))-3)");
    REQUIRE(expression->getType() == ExpressionType::EXACT);

}

// ----------------- SelectCl + SuchThatCl + PatternCl -----------------

TEST_CASE("Test Query with Select, Such That And Pattern Clause 1")
{
	std::string input = "variable var1; assign a;\nSelect var1 such that Uses(56, var1) pattern a(_, _\"9000\"_) ";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["var1"]));
	INFO(Token::EntityTypeToString(synonyms["a"]));*/

	REQUIRE(synonyms["var1"] == EntityType::VAR);
	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::VAR);
	REQUIRE(selectClDeclaration->getValue() == "var1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "56");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "var1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);

	std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(1));
	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
	std::shared_ptr<Expression> expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(patternQueryInput->getValue() == "_");
	REQUIRE(expression->getValue() == "9000");
	REQUIRE(expression->getType() == ExpressionType::PARTIAL);
}

TEST_CASE("Test Query with Select, Such That And Pattern Clause 2")
{
	std::string input = "while w; assign a; \nSelect w such that Parent*(w, a) pattern a(\"xyz\", _\"xyz\"_) ";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["w"]));
	INFO(Token::EntityTypeToString(synonyms["a"]));*/

	REQUIRE(synonyms["w"] == EntityType::WHILE);
	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT_T);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::WHILE);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::ASSIGN);

	std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(1));
	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
	std::shared_ptr<Expression> expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(patternQueryInput->getValue() == "xyz");
	REQUIRE(expression->getValue() == "xyz");
	REQUIRE(expression->getType() == ExpressionType::PARTIAL);
}

TEST_CASE("Test Query with Select, Such That And Pattern Clause 3")  // Complicated expression
{
	std::string input = "prog_line pgl1; assign a; \nSelect pgl1 such that Next*(pgl1, 27) pattern a(\"xyz\", _\"(a + (((b))))\"_) ";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["w"]));
	INFO(Token::EntityTypeToString(synonyms["a"]));*/

	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);
	REQUIRE(synonyms["a"] == EntityType::ASSIGN);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::PROGLINE);
	REQUIRE(selectClDeclaration->getValue() == "pgl1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT_T);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "pgl1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROGLINE);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "27");

	std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(1));
	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
	std::shared_ptr<Expression> expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(patternQueryInput->getValue() == "xyz");
	REQUIRE(expression->getValue() == "(a+b)");
	REQUIRE(expression->getType() == ExpressionType::PARTIAL);
}

// ----------------- SelectCl + Multiple PatternCl / Single PatternCl with multiple 'and' -----------------

TEST_CASE("Test Query with Select, And Multiple Pattern Clause 1")
{
	std::string input = "assign a; if ifs; while w; variable v; \nSelect w pattern a(\"xyz\", \"xyz\") " 
			" pattern ifs(v,_,_) pattern w(_, _) pattern w(\"x\", _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["ifs"] == EntityType::IF);
	REQUIRE(synonyms["w"] == EntityType::WHILE);
	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(0));
	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
	std::shared_ptr<Expression> expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(patternQueryInput->getValue() == "xyz");
	REQUIRE(expression->getValue() == "xyz");
	REQUIRE(expression->getType() == ExpressionType::EXACT);

	patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(1));
	patternClDeclaration = patternCl->getSynonym();
	patternQueryInput = patternCl->getQueryInput();
	expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "ifs");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::IF);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);

	patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(2));
	patternClDeclaration = patternCl->getSynonym();
	patternQueryInput = patternCl->getQueryInput();
	expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "w");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(patternQueryInput->getValue() == "_");

	patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(3));
	patternClDeclaration = patternCl->getSynonym();
	patternQueryInput = patternCl->getQueryInput();
	expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "w");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(patternQueryInput->getValue() == "x");
}

TEST_CASE("Test Query with Select, And Single Pattern Clause with Multiple 'and' ")
{
	std::string input = "assign a; if ifs; while w; variable v; \nSelect w pattern a(\"xyz\", _\"xyz\"_) and"
		" ifs(v,_,_) and w(_, _) and w(\"x\", _) and a(v, \"5 % 2 + 7 - var1 * var2 + var3 / var4 \")";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["ifs"] == EntityType::IF);
	REQUIRE(synonyms["w"] == EntityType::WHILE);
	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(selectClDeclaration->getValue() == "w");

	std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(0));
	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
	std::shared_ptr<Expression> expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(patternQueryInput->getValue() == "xyz");
	REQUIRE(expression->getValue() == "xyz");
	REQUIRE(expression->getType() == ExpressionType::PARTIAL);

	patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(1));
	patternClDeclaration = patternCl->getSynonym();
	patternQueryInput = patternCl->getQueryInput();
	expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "ifs");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::IF);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);

	patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(2));
	patternClDeclaration = patternCl->getSynonym();
	patternQueryInput = patternCl->getQueryInput();
	expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "w");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(patternQueryInput->getValue() == "_");

	patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(3));
	patternClDeclaration = patternCl->getSynonym();
	patternQueryInput = patternCl->getQueryInput();
	expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "w");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(patternQueryInput->getValue() == "x");

	patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(4));
	patternClDeclaration = patternCl->getSynonym();
	patternQueryInput = patternCl->getQueryInput();
	expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);
	REQUIRE(expression->getValue() == "((((5%2)+7)-(var1*var2))+(var3/var4))");
	REQUIRE(expression->getType() == ExpressionType::EXACT);
}

// ----------------- SelectCl + Multiple SuchThatCl / Single SuchThatCl with multiple 'and' -----------------

TEST_CASE("Test Query with Select, And Multiple Such That Clauses 1")
{
	std::string input = "assign a; if ifs; while w; variable v; \nSelect a such that Modifies(a, v) "
		" such that Uses(ifs, v) such that Parent*(w, a) such that Calls (\"x\", \"y\")";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["w"]));
	INFO(Token::EntityTypeToString(synonyms["a"]));*/

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["ifs"] == EntityType::IF);
	REQUIRE(synonyms["w"] == EntityType::WHILE);
	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(1));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "ifs");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::IF);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(2));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT_T);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::WHILE);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::ASSIGN);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(3));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "x");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(suchThatClRightQueryInput->getValue() == "y");
	
}

TEST_CASE("Test Query with Select, And Single Such That Clause with Multiple 'and' ")
{
	std::string input = "assign a; prog_line pgl1, pgl2; while w; variable v; \nSelect a such that Follows(a, w) "
		" and Next(5, 6) and Next*(pgl1, pgl2) and Modifies (7, \"y\")";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["w"]));
	INFO(Token::EntityTypeToString(synonyms["a"]));*/

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);
	REQUIRE(synonyms["pgl2"] == EntityType::PROGLINE);
	REQUIRE(synonyms["w"] == EntityType::WHILE);
	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::WHILE);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(1));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "5");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "6");

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(2));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT_T);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "pgl1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROGLINE);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "pgl2");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::PROGLINE);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(3));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "7");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(suchThatClRightQueryInput->getValue() == "y");
}

TEST_CASE("Test Query with Select and Multiple Such That Clause each having Multiple 'and' ")
{
	std::string input = "assign a; prog_line pgl1, pgl2; procedure pcd; while w; variable v; \nSelect a such that Follows(a, w) "
		" and Next(5, 6) such that Next*(pgl1, pgl2) and Modifies (7, \"y\") such that Uses(a, v) and Calls(pcd, \"helloWorldProcedure\")";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);
	REQUIRE(synonyms["pgl2"] == EntityType::PROGLINE);
	REQUIRE(synonyms["pcd"] == EntityType::PROC);
	REQUIRE(synonyms["w"] == EntityType::WHILE);
	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::FOLLOWS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::WHILE);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(1));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "5");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "6");

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(2));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT_T);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "pgl1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROGLINE);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "pgl2");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::PROGLINE);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(3));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "7");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(suchThatClRightQueryInput->getValue() == "y");

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(4));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(5));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "pcd");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROC);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(suchThatClRightQueryInput->getValue() == "helloWorldProcedure");
}

// ----------------- SelectCl + Multiple SuchThatCl + Multiple pattern interleaved -----------------

TEST_CASE("Test Query with Select, And Multiple Such That Clauses, And Multiple pattern clauses interleaved 1")
{
	std::string input = "assign a; if ifs; while w; variable v; \nSelect a pattern a(\"x\", \"7 * (3 + 2) / 4 - 3\") such that Modifies(a, v) "
		" pattern ifs(v,_,_) such that Uses(ifs, v) pattern w(v,_) such that Parent*(w, a) and Calls (\"x\", \"y\")";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["w"]));
	INFO(Token::EntityTypeToString(synonyms["a"]));*/

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["ifs"] == EntityType::IF);
	REQUIRE(synonyms["w"] == EntityType::WHILE);
	REQUIRE(synonyms["v"] == EntityType::VAR);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(selectClDeclaration->getValue() == "a");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(1));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(3));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "ifs");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::IF);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(5));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT_T);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "w");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::WHILE);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::ASSIGN);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(6));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "x");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(suchThatClRightQueryInput->getValue() == "y");

	std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(0));
	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
	std::shared_ptr<Expression> expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(patternQueryInput->getValue() == "x");
	REQUIRE(expression->getValue() == "(((7*(3+2))/4)-3)");
	REQUIRE(expression->getType() == ExpressionType::EXACT);

	patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(2));
	patternClDeclaration = patternCl->getSynonym();
	patternQueryInput = patternCl->getQueryInput();
	expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "ifs");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::IF);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);

	patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(4));
	patternClDeclaration = patternCl->getSynonym();
	patternQueryInput = patternCl->getQueryInput();
	expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "w");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);
}

TEST_CASE("Test Query with Select, And Multiple Such That Clauses, And Multiple pattern clauses interleaved 2")
{
	std::string input = "assign a; if ifs; while w; variable v; stmt s1; prog_line pgl1, pgl2; procedure pcd1;"
		"\nSelect s1 pattern a(v,_) and ifs(v,_,_)"
		"such that Next(pgl1, pgl2) and Calls*(pcd1, _) pattern w(\"controlVar\" , _) and a(\"x\", _\"y\"_)"
		"such that Modifies(s1, v) and Parent(s1, a)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	/*INFO(Token::EntityTypeToString(synonyms["w"]));
	INFO(Token::EntityTypeToString(synonyms["a"]));*/

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["ifs"] == EntityType::IF);
	REQUIRE(synonyms["w"] == EntityType::WHILE);
	REQUIRE(synonyms["v"] == EntityType::VAR);
	REQUIRE(synonyms["s1"] == EntityType::STMT);
	REQUIRE(synonyms["pgl1"] == EntityType::PROGLINE);
	REQUIRE(synonyms["pgl2"] == EntityType::PROGLINE);
	REQUIRE(synonyms["pcd1"] == EntityType::PROC);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::STMT);
	REQUIRE(selectClDeclaration->getValue() == "s1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(2));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "pgl1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROGLINE);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "pgl2");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::PROGLINE);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(3));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::CALLS_T);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "pcd1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROC);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::ANY);
	REQUIRE(suchThatClRightQueryInput->getValue() == "_");

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(6));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "s1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::STMT);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(7));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::PARENT);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "s1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::STMT);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::ASSIGN);

	std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(0));
	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
	std::shared_ptr<Expression> expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);
	REQUIRE(expression->getValue() == "_");
	REQUIRE(expression->getType() == ExpressionType::EMPTY);

	patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(1));
	patternClDeclaration = patternCl->getSynonym();
	patternQueryInput = patternCl->getQueryInput();
	expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "ifs");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::IF);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);

	patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(4));
	patternClDeclaration = patternCl->getSynonym();
	patternQueryInput = patternCl->getQueryInput();
	expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "w");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(patternQueryInput->getValue() == "controlVar");

	patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(5));
	patternClDeclaration = patternCl->getSynonym();
	patternQueryInput = patternCl->getQueryInput();
	expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "a");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::ASSIGN);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(patternQueryInput->getValue() == "x");
	REQUIRE(expression->getValue() == "y");
	REQUIRE(expression->getType() == ExpressionType::PARTIAL);
}

// ----------------- SelectCl + SuchThatCl + With clause -----------------

TEST_CASE("Test Query with Select, Such That, and With 1")
{
	std::string input = "stmt s1; assign a; procedure p; constant c;"
		"\nSelect s1 such that Affects*(s1, 9) and Affects(7, 10) with s1.stmt# = a.stmt# and c.value = 7 and p.procName = \"procA\"";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["s1"] == EntityType::STMT);
	REQUIRE(synonyms["p"] == EntityType::PROC);
	REQUIRE(synonyms["c"] == EntityType::CONST);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::STMT);
	REQUIRE(selectClDeclaration->getValue() == "s1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::AFFECTS_T);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "s1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::STMT);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "9");

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(1));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::AFFECTS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "7");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "10");

	std::shared_ptr<WithClause> withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(2));
	std::shared_ptr<QueryInput> withClLeftQueryInput = withCl->getLeftInput();
	std::shared_ptr<QueryInput> withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "s1");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::STMT);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
	REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClRightQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClRightQueryInput)->getEntityType() == EntityType::ASSIGN);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClRightQueryInput)->getIsAttribute() == false);

	withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(3));
	withClLeftQueryInput = withCl->getLeftInput();
	withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "c");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::CONST);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
	REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(withClRightQueryInput->getValue() == "7");

	withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(4));
	withClLeftQueryInput = withCl->getLeftInput();
	withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "p");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::PROC);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
	REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(withClRightQueryInput->getValue() == "procA");
}

TEST_CASE("Test Query with Select, Such That, and With 2")
{
	std::string input = "stmt s1; variable v; call call; print print; read read;"
		"\nSelect s1 with v.varName = \"var1\" such that Next*(7, 8) with call.procName = \"procA\""
		" and call.stmt# = 5 and print.stmt# = 10 and print.varName = \"var2\""
		" and read.stmt# = 11 and read.varName = \"var3\" ";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["s1"] == EntityType::STMT);
	REQUIRE(synonyms["v"] == EntityType::VAR);
	REQUIRE(synonyms["call"] == EntityType::CALL);
	REQUIRE(synonyms["print"] == EntityType::PRINT);
	REQUIRE(synonyms["read"] == EntityType::READ);

	std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
	REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(selectClDeclaration->getEntityType() == EntityType::STMT);
	REQUIRE(selectClDeclaration->getValue() == "s1");

	std::shared_ptr<WithClause> withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(0));
	std::shared_ptr<QueryInput> withClLeftQueryInput = withCl->getLeftInput();
	std::shared_ptr<QueryInput> withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::VAR);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
	REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(withClRightQueryInput->getValue() == "var1");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(1));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::NEXT_T);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "7");
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "8");

	withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(2));
	withClLeftQueryInput = withCl->getLeftInput();
	withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "call");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::CALL);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == true);
	REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(withClRightQueryInput->getValue() == "procA");

	withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(3));
	withClLeftQueryInput = withCl->getLeftInput();
	withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "call");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::CALL);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
	REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(withClRightQueryInput->getValue() == "5");

	withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(4));
	withClLeftQueryInput = withCl->getLeftInput();
	withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "print");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::PRINT);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
	REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(withClRightQueryInput->getValue() == "10");

	withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(5));
	withClLeftQueryInput = withCl->getLeftInput();
	withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "print");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::PRINT);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == true);
	REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(withClRightQueryInput->getValue() == "var2");

	withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(6));
	withClLeftQueryInput = withCl->getLeftInput();
	withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "read");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
	REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(withClRightQueryInput->getValue() == "11");

	withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(7));
	withClLeftQueryInput = withCl->getLeftInput();
	withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "read");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == true);
	REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(withClRightQueryInput->getValue() == "var3");
}

// ----------------- SelectCl + Boolean ResultCl + All other clauses -----------------

TEST_CASE("Test Query with Select + Boolean ResultCl + All other clauses 1")
{
	std::string input = "assign a; constant c; variable v; while w;"
		"\nSelect BOOLEAN such that Modifies(a, v) and Affects(a, 10) with a.stmt# = c.value pattern w(v, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["c"] == EntityType::CONST);
	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["v"] == EntityType::VAR);
	REQUIRE(synonyms["w"] == EntityType::WHILE);

	// Select BOOLEAN means that Query's Select Clause object's list of declarations will be empty
	REQUIRE(query->getSelectClause()->getDeclarations().size() == 0);

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(1));
    relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::AFFECTS);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
	REQUIRE(suchThatClRightQueryInput->getValue() == "10");

	std::shared_ptr<WithClause> withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(2));
	std::shared_ptr<QueryInput> withClLeftQueryInput = withCl->getLeftInput();
	std::shared_ptr<QueryInput> withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "a");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
	REQUIRE(withClRightQueryInput->getValue() == "c");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClRightQueryInput)->getEntityType() == EntityType::CONST);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClRightQueryInput)->getIsAttribute() == false);

	std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(3));
	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
	std::shared_ptr<Expression> expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "w");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);
}

// ----------------- SelectCl + Tuple ResultCl + All other clauses -----------------

TEST_CASE("Test Query with Select + Tuple Result Cl + All other clauses 1")
{
	std::string input = "assign a; stmt s; variable v; call call; constant c; procedure proc; if ifs; print pr; read rd;"
		"\nSelect <a.stmt#, v, s, c.value, proc.procName, call.procName, call>" 
		" with v.varName = \"var123\" such that Uses(proc, v) and Modifies(proc, v) with rd.varName = pr.varName pattern ifs(v, _, _)";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	queryParser.parse();
	std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

	REQUIRE(synonyms["a"] == EntityType::ASSIGN);
	REQUIRE(synonyms["s"] == EntityType::STMT);
	REQUIRE(synonyms["v"] == EntityType::VAR);
	REQUIRE(synonyms["call"] == EntityType::CALL);
	REQUIRE(synonyms["c"] == EntityType::CONST);
	REQUIRE(synonyms["proc"] == EntityType::PROC);
	REQUIRE(synonyms["ifs"] == EntityType::IF);
	REQUIRE(synonyms["pr"] == EntityType::PRINT);
	REQUIRE(synonyms["rd"] == EntityType::READ);

	// Select BOOLEAN means that Query's Select Clause object's list of declarations will be empty
	std::vector<std::shared_ptr<Declaration>> declarations = query->getSelectClause()->getDeclarations();
	REQUIRE(declarations.at(0)->getValue() == "a");
	REQUIRE(declarations.at(0)->getIsAttribute() == false);
	REQUIRE(declarations.at(1)->getValue() == "v");
	REQUIRE(declarations.at(1)->getIsAttribute() == false);
	REQUIRE(declarations.at(2)->getValue() == "s");
	REQUIRE(declarations.at(2)->getIsAttribute() == false);
	REQUIRE(declarations.at(3)->getValue() == "c");
	REQUIRE(declarations.at(3)->getIsAttribute() == false);
	REQUIRE(declarations.at(4)->getValue() == "proc");
	REQUIRE(declarations.at(4)->getIsAttribute() == false);
	REQUIRE(declarations.at(5)->getValue() == "call");
	REQUIRE(declarations.at(5)->getIsAttribute() == true);
	REQUIRE(declarations.at(6)->getValue() == "call");
	REQUIRE(declarations.at(6)->getIsAttribute() == false);

	std::shared_ptr<WithClause> withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(0));
	std::shared_ptr<QueryInput> withClLeftQueryInput = withCl->getLeftInput();
	std::shared_ptr<QueryInput> withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::VAR);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
	REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
	REQUIRE(withClRightQueryInput->getValue() == "var123");

	std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(1));
	RelationshipType relationshipType = relationshipCl->getRelationshipType();
	std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::USES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "proc");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROC);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);

	relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(2));
	relationshipType = relationshipCl->getRelationshipType();
	suchThatClLeftQueryInput = relationshipCl->getLeftInput();
	suchThatClRightQueryInput = relationshipCl->getRightInput();
	REQUIRE(relationshipType == RelationshipType::MODIFIES);
	REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClLeftQueryInput->getValue() == "proc");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::PROC);
	REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(suchThatClRightQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);

	withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(3));
	withClLeftQueryInput = withCl->getLeftInput();
	withClRightQueryInput = withCl->getRightInput();
	REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClLeftQueryInput->getValue() == "rd");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::READ);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == true);
	REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(withClRightQueryInput->getValue() == "pr");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClRightQueryInput)->getEntityType() == EntityType::PRINT);
	REQUIRE(std::dynamic_pointer_cast<Declaration>(withClRightQueryInput)->getIsAttribute() == true);

	std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(4));
	std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
	std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
	std::shared_ptr<Expression> expression = patternCl->getExpression();
	REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternClDeclaration->getValue() == "ifs");
	REQUIRE(patternClDeclaration->getEntityType() == EntityType::IF);
	REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
	REQUIRE(patternQueryInput->getValue() == "v");
	REQUIRE(std::dynamic_pointer_cast<Declaration>(patternQueryInput)->getEntityType() == EntityType::VAR);
}

// ----------------- Final Test Cases for bugs found during System test -------------------------------

TEST_CASE("Test allow spacing between the synonym, dot and attribute name : 's . stmt# ' is allowed")
{
    std::string input = "stmt s;"
        "Select s with s . stmt# = 12";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    queryParser.parse();
    std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

    REQUIRE(synonyms["s"] == EntityType::STMT);

    // Select BOOLEAN means that Query's Select Clause object's list of declarations will be empty
    std::vector<std::shared_ptr<Declaration>> declarations = query->getSelectClause()->getDeclarations();
    REQUIRE(declarations.at(0)->getValue() == "s");
    REQUIRE(declarations.at(0)->getIsAttribute() == false);

    std::shared_ptr<WithClause> withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(0));
    std::shared_ptr<QueryInput> withClLeftQueryInput = withCl->getLeftInput();
    std::shared_ptr<QueryInput> withClRightQueryInput = withCl->getRightInput();
    REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(withClLeftQueryInput->getValue() == "s");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::STMT);
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
    REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
    REQUIRE(withClRightQueryInput->getValue() == "12");

}

// ----------------- Negative Test Cases (Appropriate Exceptions thrown) -----------------

TEST_CASE("Test Invalid Query without Select Clause")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta;";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SyntacticException const& err) {
		REQUIRE(std::string(err.what()) == "End of query reached when a token was expected");
	}
}

TEST_CASE("Test synonym being redeclared as different synonym type")
{
	std::string input = "read re\t\n  ; variable\nv, re; Select re;";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym re with Entity Type of Read being redeclared as Entity Type of Variable");
	}
}

TEST_CASE("Test undeclared synonym in Select Clause")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re1;";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Undeclared synonym encountered in Result clause: re1");
	}
}

TEST_CASE("Test undeclared synonym in Pattern Clause")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re pattern a1(v, _);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Undeclared synonym encountered in Pattern clause: a1");
	}
}

TEST_CASE("Test synonym not of assignment type in Pattern Clause")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re pattern pcd(v, _);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym in Pattern Clause: pcd not of assign, if or while entity type");
	}
}

TEST_CASE("Test invalid expression spec in Pattern Clause")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re pattern a(v, _\";\"_);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SyntacticException const& err) {
		REQUIRE(std::string(err.what()) == "Unexpected token encountered when parsing Factor: Token Type of Semicolon"
			" with value: ;");
	}
}

TEST_CASE("Test undeclared synonym in Such That Clause Left Argument")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(re1, ifs);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Undeclared synonym encountered in stmtRef: re1");
	}
}

TEST_CASE("Test undeclared synonym in Such That Clause Right Argument")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(ifs, re1);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Undeclared synonym encountered in stmtRef: re1");
	}
}

TEST_CASE("Test disallowed variable synonym in left argument of Such That Clause with Follows")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(v, re);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym v not allowed, has Entity Type of Variable");
	}
}

TEST_CASE("Test disallowed constant synonym in left argument of Such That Clause with Follows")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(c, re);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym c not allowed, has Entity Type of Constant");
	}
}

TEST_CASE("Test disallowed procedure synonym in left argument of Such That Clause with Follows")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(pcd, re);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym pcd not allowed, has Entity Type of Procedure");
	}
}

TEST_CASE("Test disallowed variable synonym in right argument of Such That Clause with Follows")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(re, v);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym v not allowed, has Entity Type of Variable");
	}
}

TEST_CASE("Test disallowed constant synonym in right argument of Such That Clause with Follows")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(re, c);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym c not allowed, has Entity Type of Constant");
	}
}

TEST_CASE("Test disallowed procedure synonym in right argument of Such That Clause with Follows")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Follows(re, pcd);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym pcd not allowed, has Entity Type of Procedure");
	}
}

TEST_CASE("Test disallowed \"_\" in left argument of Such That Clause with Modifies")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select pn such that Modifies(_, v);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "_ is not allowed as first argument in stmtRef");
	}
}

TEST_CASE("Test disallowed print synonym in left argument of Such That Clause with Modifies")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Modifies(pn, v);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym pn not allowed, has Entity Type of Print");
	}
}

TEST_CASE("Test disallowed constant synonym in left argument of Such That Clause with Modifies")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Modifies(c, v);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym c not allowed, has Entity Type of Constant");
	}
}

TEST_CASE("Test disallowed variable synonym in left argument of Such That Clause with Modifies")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Modifies(v, v);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym v not allowed, has Entity Type of Variable");
	}
}

TEST_CASE("Test disallowed constant synonym in right argument of Such That Clause with Modifies")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Modifies(re, c);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym c not allowed, has Entity Type of Constant");
	}
}

TEST_CASE("Test disallowed \"_\" in left argument of Such That Clause with Uses")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select pn such that Uses(_, v);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "_ is not allowed as first argument in stmtRef");
	}
}

TEST_CASE("Test disallowed read synonym in left argument of Such That Clause with Uses")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select pn such that Uses(re, v);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym re not allowed, has Entity Type of Read");
	}
}

TEST_CASE("Test disallowed constant synonym in left argument of Such That Clause with Uses")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select pn such that Uses(c, v);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym c not allowed, has Entity Type of Constant");
	}
}

TEST_CASE("Test disallowed variable synonym in left argument of Such That Clause with Uses")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select re such that Uses(v, v);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym v not allowed, has Entity Type of Variable");
	}
}

TEST_CASE("Test disallowed while synonym in right argument of Such That Clause with Uses")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta; Select pn such that Uses(pn, w);";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Synonym w not allowed, has Entity Type of While");
	}
}

TEST_CASE("Test different types in attr compare")
{
	std::string input = "variable\nv; print\npn;"
		"Select pn with pn.stmt# = v.varName";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Invalid Attribute comparison in with clause (leftRef has different type from"
			" rightRef): pn compared to v");
	}
}

TEST_CASE("Test invalid attribute name in attr compare")
{
    std::string input = "read r; Select r with r.procName = \"procA\"";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SemanticException const& err) {
        REQUIRE(std::string(err.what()) == "Invalid attrName procName for synonym: r with Entity Type of Read");
    }
}

TEST_CASE("Test invalid synonym in attr compare")
{
    std::string input = "stmt s; prog_line n; Select BOOLEAN with n = 15 and s = 10";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SemanticException const& err) {
        REQUIRE(std::string(err.what()) == "Synonym in With Clause: s not of prog_line entity type");
    }
}

TEST_CASE("Test same types in attr compare but guaranteed to be false/empty 1")
{
	std::string input = "call cl; print pn;"
		"Select pn with cl.stmt# = pn.stmt#";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Attribute comparison of 2 synonym attributes guaranteed to be false/empty in with"
			" clause: cl compared to pn");
	}
}

TEST_CASE("Test same types in attr compare but guaranteed to be false/empty 2")
{
	std::string input = "print pn;"
		"Select pn with 10 = 12";
	auto query = std::make_shared<Query>();
	auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
	QueryParser queryParser = QueryParser{ tokenizer, query };
	try {
		queryParser.parse();
		REQUIRE(false);
	}
	catch (SemanticException const& err) {
		REQUIRE(std::string(err.what()) == "Attribute comparison of 2 integers guaranteed to be false/empty in with"
			" clause: 10 compared to 12");
	}
}

// ----------------- Negative test cases to debug bugs found during System test --------------------------

TEST_CASE("Test syntax errors not being thrown 1")
{
    std::string input = "variable v, v1;"
        "Select v, v1>";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SyntacticException const& err) {
        REQUIRE(std::string(err.what()) == "There are tokens remaining after parsing has been completed. Next token"
            " found: ,");
    }
}

TEST_CASE("Test syntax errors not being thrown 2")
{
    std::string input = "prog_line n1;"
        "Select n1 and n1 = 1";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SyntacticException const& err) {
        REQUIRE(std::string(err.what()) == "There are tokens remaining after parsing has been completed. Next token"
            " found: and");
    }
}

TEST_CASE("Test syntax error not being thrown 3")
{
    std::string input = "stmt s; procedure p; variable v;"
        "Select s p.proc Name = v.varName";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SyntacticException const& err) {
        REQUIRE(std::string(err.what()) == "There are tokens remaining after parsing has been completed. Next token"
            " found: p");
    }
}

TEST_CASE("Test prog_line not allowed to have attributes 1")
{
    std::string input = "prog_line n1;"
        "Select n1.stmt#";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SemanticException const& err) {
        REQUIRE(std::string(err.what()) == "Invalid attrName stmt# for synonym: n1 with Entity Type of Program line");
    }
}

TEST_CASE("Test prog_line not allowed to have attributes 2")
{
    std::string input = "prog_line n1;"
        "Select <n1.stmt#>";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SemanticException const& err) {
        REQUIRE(std::string(err.what()) == "Invalid attrName stmt# for synonym: n1 with Entity Type of Program line");
    }
}

TEST_CASE("Test prog_line not allowed to have attributes 3")
{
    std::string input = "prog_line n1;"
        "Select n1 with n1.stmt# = 3";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SemanticException const& err) {
        REQUIRE(std::string(err.what()) == "Invalid attrName stmt# for synonym: n1 with Entity Type of Program line");
    }
}

TEST_CASE("Test transitive relationship tokens not allowed to be split 1")
{
    std::string input = "prog_line n1;"
        "Select n1 such that Follows * (n1, 7)";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SyntacticException const& err) {
        REQUIRE(std::string(err.what()) == "Unexpected token encountered: Token Type of TermSymbol with value: *");
    }
}

TEST_CASE("Test transitive relationship tokens not allowed to be split 2")
{
    std::string input = "prog_line a;"
        "Select a such that Affects * (a, 7)";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SyntacticException const& err) {
        REQUIRE(std::string(err.what()) == "Unexpected token encountered: Token Type of TermSymbol with value: *");
    }
}

TEST_CASE("Test transitive relationship tokens not allowed to be split 3")
{
    std::string input = "prog_line a;"
        "Select a such that AffectsBip * (a, 7)";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SyntacticException const& err) {
        REQUIRE(std::string(err.what()) == "Unexpected token encountered: Token Type of TermSymbol with value: *");
    }
}

TEST_CASE("Test transitive relationship tokens not allowed to be split 4")
{
    std::string input = "stmt s; Select s such that Follows   *    (s, t)";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SyntacticException const& err) {
        REQUIRE(std::string(err.what()) == "Unexpected token encountered: Token Type of TermSymbol with value: *");
    }
}

TEST_CASE("Test Next(synonym, synonym) fails")
{
    std::string input = "prog_line n1; Select n1 such that Next*(n1, n1) and Next(n1, n1)";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SemanticException const& err) {
        REQUIRE(std::string(err.what()) == "Same synonym detected on both sides in Next");
    }
}

TEST_CASE("Test Boolean query 1")
{
    std::string input = "Select BOOLEAN with 1 = 1 = 1 = 1";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SyntacticException const& err) {
        REQUIRE(std::string(err.what()) == "There are tokens remaining after parsing has been completed. Next token found: =");
    }
}

TEST_CASE("Test Boolean query 2")
{
    std::string input = "stmt s; Select BOOLEAN such that NextBip*(6,6) and NextBip*(s,s) and NextBip(7, 7)";
    auto query = std::make_shared<Query>();
    auto tokenizer = std::make_shared<Tokenizer>(Tokenizer(input));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    try {
        queryParser.parse();
        REQUIRE(false);
    }
    catch (SemanticException const& err) {
        REQUIRE(std::string(err.what()) == "Left Stmt# 7 cannot be equal to right stmt# 7 in NextBip");
    }
}








