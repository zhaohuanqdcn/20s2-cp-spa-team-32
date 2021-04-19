#include "catch.hpp"
#include "Tokenizer.h"
#include "SyntacticException.h" // for throwing SyntacticException
#include <vector>

TEST_CASE("Test Tokenizer Ignores Whitespace")
{
	std::string input = "read re\t\n  ; variable\nv; constant\n\tc; procedure\npcd; print\npn; while\nw;if ifs;"
		"stmt s1; assign\n\ta;";
	Tokenizer tokenizer = Tokenizer{ input };
	std::vector<std::shared_ptr<Token>> tokens;
	std::shared_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)27);
	TokenTypes tokenTypes[] = { TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon };
	std::string tokenValues[] = { "read", "re", ";", "variable", "v", ";", "constant", "c", ";", "procedure", "pcd", ";",
		"print", "pn", ";", "while", "w", ";", "if", "ifs", ";", "stmt", "s1", ";", "assign", "a", ";" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 1")
{
	Tokenizer tokenizer = Tokenizer{ "assign a; variable v;\nSelect v such that Modifies(a, v) pattern a(_, _\"v\"_)" };
	std::vector<std::shared_ptr<Token>> tokens;
	std::shared_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)27);
	TokenTypes tokenTypes[] = { TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::Modifies, TokenTypes::LeftParen,
		TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen, TokenTypes::Pattern, TokenTypes::Identifier,
		TokenTypes::LeftParen, TokenTypes::Underscore, TokenTypes::Comma,
		TokenTypes::Underscore, TokenTypes::DoubleQuote, TokenTypes::Identifier,
		TokenTypes::DoubleQuote, TokenTypes::Underscore, TokenTypes::RightParen };
	std::string tokenValues[] = { "assign", "a", ";", "variable", "v", ";", "Select", "v", "such", "that", "Modifies", "(",
		"a", ",", "v", ")", "pattern", "a", "(", "_", ",", "_", "\"", "v", "\"", "_", ")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 2")
{
	Tokenizer tokenizer = Tokenizer{ "stmt s; print pn;\nSelect v such that Follows*(s, pn) pattern a(\"testVariable\", _\"v\"_)" };
	std::vector<std::shared_ptr<Token>> tokens;
	std::shared_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t) 29);
	TokenTypes tokenTypes[] = { TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::FollowsT, TokenTypes::LeftParen,
		TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen, TokenTypes::Pattern, TokenTypes::Identifier,
		TokenTypes::LeftParen, TokenTypes::DoubleQuote, TokenTypes::Identifier,
		TokenTypes::DoubleQuote, TokenTypes::Comma, TokenTypes::Underscore,
		TokenTypes::DoubleQuote, TokenTypes::Identifier, TokenTypes::DoubleQuote,
		TokenTypes::Underscore, TokenTypes::RightParen };
	std::string tokenValues[] = { "stmt", "s", ";", "print", "pn", ";", "Select", "v", "such", "that", "Follows*",
		"(", "s", ",", "pn", ")", "pattern", "a", "(", "\"", "testVariable", "\"", ",", "_", "\"", "v", "\"", "_", ")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}

}

TEST_CASE("Test Combination 3")
{
	Tokenizer tokenizer = Tokenizer{ "read re;\nSelect re such that Parent(121, re) pattern a(\"dummyVariable\", _\"3230\"_)" };
	std::vector<std::shared_ptr<Token>> tokens;
	std::shared_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)26);
	TokenTypes tokenTypes[] = { TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::Parent, TokenTypes::LeftParen,
		TokenTypes::Integer, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen, TokenTypes::Pattern, TokenTypes::Identifier,
		TokenTypes::LeftParen, TokenTypes::DoubleQuote, TokenTypes::Identifier,
		TokenTypes::DoubleQuote, TokenTypes::Comma, TokenTypes::Underscore, TokenTypes::DoubleQuote, TokenTypes::Integer,
		TokenTypes::DoubleQuote, TokenTypes::Underscore, TokenTypes::RightParen };
	std::string tokenValues[] = { "read", "re", ";", "Select", "re", "such", "that", "Parent", "(",
		"121", ",", "re", ")", "pattern", "a", "(", "\"", "dummyVariable", "\"", ",", "_", "\"", "3230", "\"", "_", ")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 4")
{
	Tokenizer tokenizer = Tokenizer{ "variable var1;\nSelect a such that Uses(56, var1) pattern a(_, _\"9000\"_)" };
	std::vector<std::shared_ptr<Token>> tokens;
	std::shared_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)24);
	TokenTypes tokenTypes[] = { TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::Uses, TokenTypes::LeftParen,
		TokenTypes::Integer, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen, TokenTypes::Pattern, TokenTypes::Identifier,
		TokenTypes::LeftParen, TokenTypes::Underscore, TokenTypes::Comma,
		TokenTypes::Underscore, TokenTypes::DoubleQuote, TokenTypes::Integer,
		TokenTypes::DoubleQuote, TokenTypes::Underscore, TokenTypes::RightParen };
	std::string tokenValues[] = { "variable", "var1", ";", "Select", "a", "such", "that", "Uses", "(",
		"56", ",", "var1", ")", "pattern", "a", "(", "_", ",", "_", "\"", "9000", "\"", "_", ")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 5")
{
	Tokenizer tokenizer = Tokenizer{ "while w; if ifs;\nSelect w such that Parent*(w, ifs)" };
	std::vector<std::shared_ptr<Token>> tokens;
	std::shared_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)16);
	TokenTypes tokenTypes[] = { TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::ParentT, TokenTypes::LeftParen,
		TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen };
	std::string tokenValues[] = { "while", "w", ";", "if", "ifs", ";", "Select", "w", "such", "that", "Parent*",
		"(", "w", ",", "ifs", ")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 6")
{
	Tokenizer tokenizer = Tokenizer{ "procedure p1, p2; assign a12, a21; \nSelect p1 such that Parent*(a12, a21) and Calls*(p1, p2)" };
	std::vector<std::shared_ptr<Token>> tokens;
	std::shared_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t) 27);
	TokenTypes tokenTypes[] = { 
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::ParentT, TokenTypes::LeftParen,
		TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen, TokenTypes::And, TokenTypes::CallsT, TokenTypes::LeftParen,
		TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen };
	std::string tokenValues[] = { "procedure","p1",",","p2",";","assign","a12",",","a21",";","Select","p1",
			"such","that","Parent*","(","a12",",","a21",")","and","Calls*","(","p1",",","p2",")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 7")
{
	Tokenizer tokenizer = Tokenizer{ "prog_line pgl1, pgl2; \nSelect pgl1 such that Next*(pgl2, pgl1) and Next(pgl1, pgl2)" };
	std::vector<std::shared_ptr<Token>> tokens;
	std::shared_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t) 22);
	TokenTypes tokenTypes[] = {
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::NextT, TokenTypes::LeftParen,
		TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen, TokenTypes::And, TokenTypes::Next, TokenTypes::LeftParen,
		TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier,
		TokenTypes::RightParen };
	std::string tokenValues[] = { "prog_line","pgl1",",","pgl2",";","Select","pgl1","such","that","Next*",
			"(","pgl2",",","pgl1",")","and","Next","(","pgl1",",","pgl2",")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		INFO(tokens[i]->toString());
		INFO(Token::TokenTypeToString(tokenTypes[i]));
		INFO(tokenValues[i]);
		INFO("=====================================");
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 8")
{
	Tokenizer tokenizer = Tokenizer{ "assign a; \nSelect a pattern a(\"sourceVar\", \"3 + 2 / sourceVar % v1 - v2 + v3\")" };
	std::vector<std::shared_ptr<Token>> tokens;
	std::shared_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)26);
	TokenTypes tokenTypes[] = {
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Pattern,
		TokenTypes::Identifier, TokenTypes::LeftParen, TokenTypes::DoubleQuote, 
		TokenTypes::Identifier, TokenTypes::DoubleQuote, TokenTypes::Comma, TokenTypes::DoubleQuote,
		TokenTypes::Integer, TokenTypes::ExprSymbol, TokenTypes::Integer, TokenTypes::TermSymbol,
		TokenTypes::Identifier, TokenTypes::TermSymbol, TokenTypes::Identifier, TokenTypes::ExprSymbol, TokenTypes::Identifier,
		TokenTypes::ExprSymbol, TokenTypes::Identifier, TokenTypes::DoubleQuote, TokenTypes::RightParen };
	std::string tokenValues[] = { "assign","a",";","Select","a","pattern","a","(",
			"\"","sourceVar","\"",",","\"","3","+","2","/","sourceVar","%","v1","-","v2","+","v3","\"",")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 9")
{
	Tokenizer tokenizer = Tokenizer{ "if ifs; assign a; \nSelect ifs such that pattern a(_, \"3 % 5 % 6 / 3 + 2 + myvalue\") and pattern ifs(\"sourceVar\", _ , _ )" };
	std::vector<std::shared_ptr<Token>> tokens;
	std::shared_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t) 41);
	TokenTypes tokenTypes[] = {
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such, TokenTypes::That,
		TokenTypes::Pattern,
		TokenTypes::Identifier, TokenTypes::LeftParen, TokenTypes::Underscore, TokenTypes::Comma, TokenTypes::DoubleQuote,
		TokenTypes::Integer, TokenTypes::TermSymbol, TokenTypes::Integer, TokenTypes::TermSymbol,
		TokenTypes::Integer, TokenTypes::TermSymbol, TokenTypes::Integer, TokenTypes::ExprSymbol,
		TokenTypes::Integer, TokenTypes::ExprSymbol, TokenTypes::Identifier, TokenTypes::DoubleQuote, TokenTypes::RightParen,
		TokenTypes::And, TokenTypes::Pattern, TokenTypes::Identifier, TokenTypes::LeftParen, TokenTypes::DoubleQuote,
		TokenTypes::Identifier, TokenTypes::DoubleQuote, TokenTypes::Comma, TokenTypes::Underscore, TokenTypes::Comma,
		TokenTypes::Underscore, TokenTypes::RightParen
	};
	std::string tokenValues[] = { "if","ifs",";","assign","a",";","Select","ifs","such","that","pattern","a","(",
		"_",",","\"","3","%","5","%","6","/","3","+","2","+","myvalue","\"",")","and","pattern","ifs","(","\"",
		"sourceVar","\"",",","_",",","_",")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 10")
{
	Tokenizer tokenizer = Tokenizer{ "call c1, c2; \nSelect c1 such that Follows(c1, c2)" };
	std::vector<std::shared_ptr<Token>> tokens;
	std::shared_ptr<Token> current = std::move(tokenizer.readNext());
	while (current) {
		tokens.push_back(std::move(current));
		current = std::move(tokenizer.readNext());
	}

	REQUIRE(tokens.size() == (size_t)15);
	TokenTypes tokenTypes[] = {
		TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier, TokenTypes::Semicolon,
		TokenTypes::Select, TokenTypes::Identifier, TokenTypes::Such,
		TokenTypes::That, TokenTypes::Follows, TokenTypes::LeftParen,
		TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier, TokenTypes::RightParen };
	std::string tokenValues[] = { "call","c1",",","c2",";","Select","c1","such","that","Follows","(","c1",",","c2",")" };

	for (size_t i = 0; i < tokens.size(); i++) {
		REQUIRE(tokens[i]->getType() == tokenTypes[i]);
		REQUIRE(tokens[i]->getValue() == tokenValues[i]);
	}
}

TEST_CASE("Test Combination 11")
{
    Tokenizer tokenizer = Tokenizer{ "prog_line n1, n2; assign a1, a2; \nSelect <a1, n2> such that AffectsBip(a1, a2) and NextBip(n1, n2)" };
    std::vector<std::shared_ptr<Token>> tokens;
    std::shared_ptr<Token> current = std::move(tokenizer.readNext());
    while (current) {
        tokens.push_back(std::move(current));
        current = std::move(tokenizer.readNext());
    }

    REQUIRE(tokens.size() == (size_t)31);
    TokenTypes tokenTypes[] = {
        TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier, TokenTypes::Semicolon,
        TokenTypes::DesignEntity, TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier, TokenTypes::Semicolon,
        TokenTypes::Select, TokenTypes::LeftAngleBracket, TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier, 
        TokenTypes::RightAngleBracket, TokenTypes::Such, TokenTypes::That, TokenTypes::AffectsBip, TokenTypes::LeftParen,
        TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier, TokenTypes::RightParen, TokenTypes::And,
        TokenTypes::NextBip, TokenTypes::LeftParen, TokenTypes::Identifier, TokenTypes::Comma, TokenTypes::Identifier, 
        TokenTypes::RightParen,
    };
    std::string tokenValues[] = { "prog_line","n1",",","n2",";", "assign","a1",",","a2",";",
        "Select", "<", "a1", ",", "n2", ">", "such","that","AffectsBip","(","a1",",","a2",")", "and",
        "NextBip", "(", "n1", ",", "n2", ")"
    };

    for (size_t i = 0; i < tokens.size(); i++) {
        REQUIRE(tokens[i]->getType() == tokenTypes[i]);
        REQUIRE(tokens[i]->getValue() == tokenValues[i]);
    }
}

// Negative test cases

TEST_CASE("Negative Test Combination 1")
{
	Tokenizer tokenizer = Tokenizer{ "prog#" };
	std::vector<std::shared_ptr<Token>> tokens;
	try {
		tokenizer.readNext();
        tokenizer.readNext();
		REQUIRE(false);
	}
	catch (SyntacticException const& err) {
		REQUIRE(std::string(err.what()) == "Invalid identifier encountered: prog#");
	}
}

TEST_CASE("Negative Test Combination 2")
{
    Tokenizer tokenizer = Tokenizer{ "prog_linepg1 ;" };
    std::vector<std::shared_ptr<Token>> tokens;
    try {
        tokenizer.readNext();
        tokenizer.readNext();
        REQUIRE(false);
    }
    catch (SyntacticException const& err) {
        REQUIRE(std::string(err.what()) == "Invalid identifier encountered: prog_linepg");
    }
}