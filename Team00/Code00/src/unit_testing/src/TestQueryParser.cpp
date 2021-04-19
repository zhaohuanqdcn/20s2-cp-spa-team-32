#include "catch.hpp"
#include "QueryParser.h"
#include "TokenizerStub.h"
#include "Query.h"
#include <vector>
#include <memory>

// ----------------- SelectCl + SuchThatCl + PatternCl -----------------

TEST_CASE("Test Query with Select, Such That And Pattern Clause 1")
{
	std::string input = "variable var1; assign a;\nSelect var1 such that Uses(56, var1) pattern a(_, _\"9000\"_) ";
	auto query = std::make_shared<Query>();
	std::vector<Token> tokens{Token(TokenTypes::DesignEntity, "variable"), Token(TokenTypes::Identifier, "var1"), Token(TokenTypes::Semicolon, ";"),
			Token(TokenTypes::DesignEntity, "assign"), Token(TokenTypes::Identifier, "a"), Token(TokenTypes::Semicolon, ";"), 
			Token(TokenTypes::Select, "Select"), Token(TokenTypes::Identifier, "var1"), Token(TokenTypes::Such, "such"),
			Token(TokenTypes::That, "that"), Token(TokenTypes::Uses, "Uses"), Token(TokenTypes::LeftParen, "("),
			Token(TokenTypes::Integer, "56"), Token(TokenTypes::Comma, ","), Token(TokenTypes::Identifier, "var1"),
			Token(TokenTypes::RightParen, ")"), Token(TokenTypes::Pattern, "pattern"), Token(TokenTypes::Identifier, "a"), 
			Token(TokenTypes::LeftParen, "("), Token(TokenTypes::Underscore, "_"), Token(TokenTypes::Comma, ","),
			Token(TokenTypes::Underscore, "_"), Token(TokenTypes::DoubleQuote, "\""), Token(TokenTypes::Integer, "9000"),
			Token(TokenTypes::DoubleQuote, "\""), Token(TokenTypes::Underscore, "_"), Token(TokenTypes::RightParen, ")")
	};
	auto tokenizer = std::make_shared<TokenizerStub>(TokenizerStub(tokens));
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

// ----------------- SelectCl + SuchThatCl + PatternCl + With Clause -----------------

TEST_CASE("Test Query with Select, Such That, Pattern and With Clause 1")
{
    std::string input = "while w; variable var1;\nSelect var1 such that Modifies(52, var1) pattern w(_, _) with var1.varName = \"hello\"";
    auto query = std::make_shared<Query>();
    std::vector<Token> tokens{ 
            Token(TokenTypes::DesignEntity, "while"), Token(TokenTypes::Identifier, "w"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::DesignEntity, "variable"), Token(TokenTypes::Identifier, "var1"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::Select, "Select"), Token(TokenTypes::Identifier, "var1"), Token(TokenTypes::Such, "such"),
            Token(TokenTypes::That, "that"), Token(TokenTypes::Modifies, "Modifies"), Token(TokenTypes::LeftParen, "("),
            Token(TokenTypes::Integer, "52"), Token(TokenTypes::Comma, ","), Token(TokenTypes::Identifier, "var1"),
            Token(TokenTypes::RightParen, ")"), Token(TokenTypes::Pattern, "pattern"), Token(TokenTypes::Identifier, "w"),
            Token(TokenTypes::LeftParen, "("), Token(TokenTypes::Underscore, "_"), Token(TokenTypes::Comma, ","),
            Token(TokenTypes::Underscore, "_"), Token(TokenTypes::RightParen, ")"), Token(TokenTypes::With, "with"),
            Token(TokenTypes::Identifier, "var1"), Token(TokenTypes::Dot, "."), Token(TokenTypes::AttrName, "varName"),
            Token(TokenTypes::Equals, "="), Token(TokenTypes::DoubleQuote, "\""), Token(TokenTypes::Identifier, "hello"),
            Token(TokenTypes::DoubleQuote, "\"")
    };
    auto tokenizer = std::make_shared<TokenizerStub>(TokenizerStub(tokens));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    queryParser.parse();
    std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

    /*INFO(Token::EntityTypeToString(synonyms["var1"]));
    INFO(Token::EntityTypeToString(synonyms["a"]));*/

    REQUIRE(synonyms["var1"] == EntityType::VAR);
    REQUIRE(synonyms["w"] == EntityType::WHILE);

    std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
    REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(selectClDeclaration->getEntityType() == EntityType::VAR);
    REQUIRE(selectClDeclaration->getValue() == "var1");

    std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
    RelationshipType relationshipType = relationshipCl->getRelationshipType();
    std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
    std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
    REQUIRE(relationshipType == RelationshipType::MODIFIES);
    REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
    REQUIRE(suchThatClLeftQueryInput->getValue() == "52");
    REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(suchThatClRightQueryInput->getValue() == "var1");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::VAR);
    

    std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(1));
    std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
    std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
    std::shared_ptr<Expression> expression = patternCl->getExpression();
    REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(patternClDeclaration->getValue() == "w");
    REQUIRE(patternClDeclaration->getEntityType() == EntityType::WHILE);
    REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::ANY);
    REQUIRE(patternQueryInput->getValue() == "_");

    std::shared_ptr<WithClause> withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(2));
    std::shared_ptr<QueryInput> withClLeftQueryInput = withCl->getLeftInput();
    std::shared_ptr<QueryInput> withClRightQueryInput = withCl->getRightInput();
    REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(withClLeftQueryInput->getValue() == "var1");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::VAR);
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
    REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
    REQUIRE(withClRightQueryInput->getValue() == "hello");
}

TEST_CASE("Test Query with Select, Such That, Pattern and With Clause 2")
{
    std::string input = "call c; assign a; if ifs; \nSelect c such that NextBip*(c, a) pattern ifs(_, _, _) with c.procName = \"procA\"";
    auto query = std::make_shared<Query>();
    std::vector<Token> tokens{
            Token(TokenTypes::DesignEntity, "call"), Token(TokenTypes::Identifier, "c"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::DesignEntity, "assign"), Token(TokenTypes::Identifier, "a"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::DesignEntity, "if"), Token(TokenTypes::Identifier, "ifs"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::Select, "Select"), Token(TokenTypes::Identifier, "c"), Token(TokenTypes::Such, "such"),
            Token(TokenTypes::That, "that"), Token(TokenTypes::NextBipT, "NextBip*"), Token(TokenTypes::LeftParen, "("),
            Token(TokenTypes::Identifier, "c"), Token(TokenTypes::Comma, ","), Token(TokenTypes::Identifier, "a"),
            Token(TokenTypes::RightParen, ")"), Token(TokenTypes::Pattern, "pattern"), Token(TokenTypes::Identifier, "ifs"),
            Token(TokenTypes::LeftParen, "("), Token(TokenTypes::Underscore, "_"), Token(TokenTypes::Comma, ","),
            Token(TokenTypes::Underscore, "_"), Token(TokenTypes::Comma, ","), Token(TokenTypes::Underscore, "_"),
            Token(TokenTypes::RightParen, ")"), Token(TokenTypes::With, "with"),
            Token(TokenTypes::Identifier, "c"), Token(TokenTypes::Dot, "."), Token(TokenTypes::AttrName, "procName"),
            Token(TokenTypes::Equals, "="), Token(TokenTypes::DoubleQuote, "\""), Token(TokenTypes::Identifier, "procA"),
            Token(TokenTypes::DoubleQuote, "\"")
    };
    auto tokenizer = std::make_shared<TokenizerStub>(TokenizerStub(tokens));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    queryParser.parse();
    std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

    /*INFO(Token::EntityTypeToString(synonyms["var1"]));
    INFO(Token::EntityTypeToString(synonyms["a"]));*/

    REQUIRE(synonyms["c"] == EntityType::CALL);
    REQUIRE(synonyms["ifs"] == EntityType::IF);
    REQUIRE(synonyms["a"] == EntityType::ASSIGN);

    std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
    REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(selectClDeclaration->getEntityType() == EntityType::CALL);
    REQUIRE(selectClDeclaration->getValue() == "c");

    std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
    RelationshipType relationshipType = relationshipCl->getRelationshipType();
    std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
    std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
    REQUIRE(relationshipType == RelationshipType::NEXTBIP_T);
    REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(suchThatClLeftQueryInput->getValue() == "c");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::CALL);
    REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(suchThatClRightQueryInput->getValue() == "a");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::ASSIGN);


    std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(1));
    std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
    std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
    std::shared_ptr<Expression> expression = patternCl->getExpression();
    REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(patternClDeclaration->getValue() == "ifs");
    REQUIRE(patternClDeclaration->getEntityType() == EntityType::IF);
    REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::ANY);
    REQUIRE(patternQueryInput->getValue() == "_");

    std::shared_ptr<WithClause> withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(2));
    std::shared_ptr<QueryInput> withClLeftQueryInput = withCl->getLeftInput();
    std::shared_ptr<QueryInput> withClRightQueryInput = withCl->getRightInput();
    REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(withClLeftQueryInput->getValue() == "c");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::CALL);
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == true);
    REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
    REQUIRE(withClRightQueryInput->getValue() == "procA");
}

TEST_CASE("Test Query with Select, Such That, Pattern and With Clause 3")
{
    std::string input = "assign a1; assign a2; if ifs; \nSelect a1 such that AffectsBip*(a1, a2) pattern ifs(_, _, _) with a1.stmt# = a2.stmt#";
    auto query = std::make_shared<Query>();
    std::vector<Token> tokens{
            Token(TokenTypes::DesignEntity, "assign"), Token(TokenTypes::Identifier, "a1"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::DesignEntity, "assign"), Token(TokenTypes::Identifier, "a2"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::DesignEntity, "if"), Token(TokenTypes::Identifier, "ifs"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::Select, "Select"), Token(TokenTypes::Identifier, "a1"), Token(TokenTypes::Such, "such"),
            Token(TokenTypes::That, "that"), Token(TokenTypes::AffectsBipT, "AffectsBip*"), Token(TokenTypes::LeftParen, "("),
            Token(TokenTypes::Identifier, "a1"), Token(TokenTypes::Comma, ","), Token(TokenTypes::Identifier, "a2"),
            Token(TokenTypes::RightParen, ")"), Token(TokenTypes::Pattern, "pattern"), Token(TokenTypes::Identifier, "ifs"),
            Token(TokenTypes::LeftParen, "("), Token(TokenTypes::Underscore, "_"), Token(TokenTypes::Comma, ","),
            Token(TokenTypes::Underscore, "_"), Token(TokenTypes::Comma, ","), Token(TokenTypes::Underscore, "_"),
            Token(TokenTypes::RightParen, ")"), Token(TokenTypes::With, "with"),
            Token(TokenTypes::Identifier, "a1"), Token(TokenTypes::Dot, "."), Token(TokenTypes::AttrName, "stmt#"),
            Token(TokenTypes::Equals, "="), 
            Token(TokenTypes::Identifier, "a2"), Token(TokenTypes::Dot, "."), Token(TokenTypes::AttrName, "stmt#"),
    };
    auto tokenizer = std::make_shared<TokenizerStub>(TokenizerStub(tokens));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    queryParser.parse();
    std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

    /*INFO(Token::EntityTypeToString(synonyms["var1"]));
    INFO(Token::EntityTypeToString(synonyms["a"]));*/

    REQUIRE(synonyms["a2"] == EntityType::ASSIGN);
    REQUIRE(synonyms["ifs"] == EntityType::IF);
    REQUIRE(synonyms["a1"] == EntityType::ASSIGN);

    std::shared_ptr<Declaration> selectClDeclaration = query->getSelectClause()->getDeclarations().at(0);
    REQUIRE(selectClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(selectClDeclaration->getEntityType() == EntityType::ASSIGN);
    REQUIRE(selectClDeclaration->getValue() == "a1");

    std::shared_ptr<RelationshipClause> relationshipCl = std::dynamic_pointer_cast<RelationshipClause>(query->getOptionalClauses().at(0));
    RelationshipType relationshipType = relationshipCl->getRelationshipType();
    std::shared_ptr<QueryInput> suchThatClLeftQueryInput = relationshipCl->getLeftInput();
    std::shared_ptr<QueryInput> suchThatClRightQueryInput = relationshipCl->getRightInput();
    REQUIRE(relationshipType == RelationshipType::AFFECTSBIP_T);
    REQUIRE(suchThatClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(suchThatClLeftQueryInput->getValue() == "a1");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
    REQUIRE(suchThatClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(suchThatClRightQueryInput->getValue() == "a2");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(suchThatClRightQueryInput)->getEntityType() == EntityType::ASSIGN);


    std::shared_ptr<PatternClause> patternCl = std::dynamic_pointer_cast<PatternClause>(query->getOptionalClauses().at(1));
    std::shared_ptr<Declaration> patternClDeclaration = patternCl->getSynonym();
    std::shared_ptr<QueryInput> patternQueryInput = patternCl->getQueryInput();
    std::shared_ptr<Expression> expression = patternCl->getExpression();
    REQUIRE(patternClDeclaration->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(patternClDeclaration->getValue() == "ifs");
    REQUIRE(patternClDeclaration->getEntityType() == EntityType::IF);
    REQUIRE(patternQueryInput->getQueryInputType() == QueryInputType::ANY);
    REQUIRE(patternQueryInput->getValue() == "_");

    std::shared_ptr<WithClause> withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(2));
    std::shared_ptr<QueryInput> withClLeftQueryInput = withCl->getLeftInput();
    std::shared_ptr<QueryInput> withClRightQueryInput = withCl->getRightInput();
    REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(withClLeftQueryInput->getValue() == "a1");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::ASSIGN);
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
    REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(withClRightQueryInput->getValue() == "a2");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClRightQueryInput)->getEntityType() == EntityType::ASSIGN);
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClRightQueryInput)->getIsAttribute() == false);
}

TEST_CASE("Test Query with Select Boolean and With Clause")
{
    std::string input = "call cl; prog_line n1; constant c; \nSelect BOOLEAN with c.value = 15 and n1 = n1";
    auto query = std::make_shared<Query>();
    std::vector<Token> tokens{
            Token(TokenTypes::DesignEntity, "call"), Token(TokenTypes::Identifier, "cl"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::DesignEntity, "prog_line"), Token(TokenTypes::Identifier, "n1"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::DesignEntity, "constant"), Token(TokenTypes::Identifier, "c"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::Select, "Select"), Token(TokenTypes::Boolean, "BOOLEAN"), Token(TokenTypes::With, "with"),
            Token(TokenTypes::Identifier, "c"), Token(TokenTypes::Dot, "."), Token(TokenTypes::AttrName, "value"),
            Token(TokenTypes::Equals, "="), Token(TokenTypes::Integer, "15"), Token(TokenTypes::And, "and"),
            Token(TokenTypes::Identifier, "n1"),
            Token(TokenTypes::Equals, "="),
            Token(TokenTypes::Identifier, "n1"),
    };
    auto tokenizer = std::make_shared<TokenizerStub>(TokenizerStub(tokens));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    queryParser.parse();
    std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

    /*INFO(Token::EntityTypeToString(synonyms["var1"]));
    INFO(Token::EntityTypeToString(synonyms["a"]));*/

    REQUIRE(synonyms["cl"] == EntityType::CALL);
    REQUIRE(synonyms["n1"] == EntityType::PROGLINE);
    REQUIRE(synonyms["c"] == EntityType::CONST);

    REQUIRE(query->getSelectClause()->getDeclarations().size() <= 0);

    std::shared_ptr<WithClause> withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(0));
    std::shared_ptr<QueryInput> withClLeftQueryInput = withCl->getLeftInput();
    std::shared_ptr<QueryInput> withClRightQueryInput = withCl->getRightInput();
    REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(withClLeftQueryInput->getValue() == "c");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::CONST);
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
    REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
    REQUIRE(withClRightQueryInput->getValue() == "15");

    withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(1));
    withClLeftQueryInput = withCl->getLeftInput();
    withClRightQueryInput = withCl->getRightInput();
    REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(withClLeftQueryInput->getValue() == "n1");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::PROGLINE);
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
    REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(withClRightQueryInput->getValue() == "n1");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClRightQueryInput)->getEntityType() == EntityType::PROGLINE);
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClRightQueryInput)->getIsAttribute() == false);
}

TEST_CASE("Test Query with Select Tuple and With Clause")
{
    std::string input = "call cl; print pn; constant c; \nSelect <cl.stmt#, pn.varName, c.value> with c.value = 15 and pn.varName = \"var1\"";
    auto query = std::make_shared<Query>();
    std::vector<Token> tokens{
            Token(TokenTypes::DesignEntity, "call"), Token(TokenTypes::Identifier, "cl"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::DesignEntity, "print"), Token(TokenTypes::Identifier, "pn"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::DesignEntity, "constant"), Token(TokenTypes::Identifier, "c"), Token(TokenTypes::Semicolon, ";"),
            Token(TokenTypes::Select, "Select"), Token(TokenTypes::LeftAngleBracket, "<"), Token(TokenTypes::Identifier, "cl"),
            Token(TokenTypes::Dot, "."), Token(TokenTypes::AttrName, "stmt#"), Token(TokenTypes::Comma, ","), 
            Token(TokenTypes::Identifier, "pn"),
            Token(TokenTypes::Dot, "."), Token(TokenTypes::AttrName, "varName"), Token(TokenTypes::Comma, ","), 
            Token(TokenTypes::Identifier, "c"),
            Token(TokenTypes::Dot, "."), Token(TokenTypes::AttrName, "value"), Token(TokenTypes::RightAngleBracket, ">"),
            Token(TokenTypes::With, "with"),
            Token(TokenTypes::Identifier, "c"), Token(TokenTypes::Dot, "."), Token(TokenTypes::AttrName, "value"),
            Token(TokenTypes::Equals, "="), Token(TokenTypes::Integer, "15"), Token(TokenTypes::And, "and"),
            Token(TokenTypes::Identifier, "pn"), Token(TokenTypes::Dot, "."), Token(TokenTypes::AttrName, "varName"),
            Token(TokenTypes::Equals, "="),
            Token(TokenTypes::DoubleQuote, "\""), Token(TokenTypes::Identifier, "var1"), Token(TokenTypes::DoubleQuote, "\""),
    };
    auto tokenizer = std::make_shared<TokenizerStub>(TokenizerStub(tokens));
    QueryParser queryParser = QueryParser{ tokenizer, query };
    queryParser.parse();
    std::unordered_map<std::string, EntityType> synonyms = queryParser.getSynonyms();

    /*INFO(Token::EntityTypeToString(synonyms["var1"]));
    INFO(Token::EntityTypeToString(synonyms["a"]));*/

    REQUIRE(synonyms["cl"] == EntityType::CALL);
    REQUIRE(synonyms["pn"] == EntityType::PRINT);
    REQUIRE(synonyms["c"] == EntityType::CONST);

    std::shared_ptr<Declaration> selectClDeclaration1 = query->getSelectClause()->getDeclarations().at(0);
    REQUIRE(selectClDeclaration1->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(selectClDeclaration1->getEntityType() == EntityType::CALL);
    REQUIRE(selectClDeclaration1->getValue() == "cl");

    std::shared_ptr<Declaration> selectClDeclaration2 = query->getSelectClause()->getDeclarations().at(1);
    REQUIRE(selectClDeclaration2->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(selectClDeclaration2->getEntityType() == EntityType::PRINT);
    REQUIRE(selectClDeclaration2->getIsAttribute() == true);
    REQUIRE(selectClDeclaration2->getValue() == "pn");

    std::shared_ptr<Declaration> selectClDeclaration3 = query->getSelectClause()->getDeclarations().at(2);
    REQUIRE(selectClDeclaration3->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(selectClDeclaration3->getEntityType() == EntityType::CONST);
    REQUIRE(selectClDeclaration3->getValue() == "c");

    std::shared_ptr<WithClause> withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(0));
    std::shared_ptr<QueryInput> withClLeftQueryInput = withCl->getLeftInput();
    std::shared_ptr<QueryInput> withClRightQueryInput = withCl->getRightInput();
    REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(withClLeftQueryInput->getValue() == "c");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::CONST);
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == false);
    REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::STMT_NUM);
    REQUIRE(withClRightQueryInput->getValue() == "15");

    withCl = std::dynamic_pointer_cast<WithClause>(query->getOptionalClauses().at(1));
    withClLeftQueryInput = withCl->getLeftInput();
    withClRightQueryInput = withCl->getRightInput();
    REQUIRE(withClLeftQueryInput->getQueryInputType() == QueryInputType::DECLARATION);
    REQUIRE(withClLeftQueryInput->getValue() == "pn");
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getEntityType() == EntityType::PRINT);
    REQUIRE(std::dynamic_pointer_cast<Declaration>(withClLeftQueryInput)->getIsAttribute() == true);
    REQUIRE(withClRightQueryInput->getQueryInputType() == QueryInputType::IDENT);
    REQUIRE(withClRightQueryInput->getValue() == "var1");
}
