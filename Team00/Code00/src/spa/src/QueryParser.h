#pragma once
#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include "TokenizerInterface.h"
#include <vector>
#include <unordered_map>
#include <set>
#include "Declaration.h"
#include "Any.h"
#include "StmtNum.h"
#include "Ident.h"
#include "Expression.h"
#include "QueryInterface.h"

class QueryParser
{
private:

    std::shared_ptr<Token> currToken;
    // Tokenizer to be used
    std::shared_ptr<TokenizerInterface> tokenizer;
    // Object to pass to Query Evaluator
    std::shared_ptr<QueryInterface> query;

    // To check whether all synonyms in select, such that and pattern clauses have been declared
    std::unordered_map<std::string, EntityType> synonyms;

    void getNextToken();
    std::shared_ptr<Token> accept(TokenTypes type);
    std::shared_ptr<Token> expect(TokenTypes type);
    std::shared_ptr<QueryInput> expect(std::shared_ptr<QueryInput> queryInput, bool isStmtRef);
    void selectClause();
    void resultClause();
    bool tuple();
    bool elem();
    bool declaration();
    bool suchThatClause();
    bool withClause();
    void attrCompare();
    std::shared_ptr<QueryInput> ref();
    bool patternClause();
    void patternAssign(std::string synoynmValue);
    void patternWhile(std::string synoynmValue);
    void patternIf(std::string synoynmValue);
    void relRef();
    std::shared_ptr<QueryInput> stmtRef(std::set<EntityType> allowedDesignEntities, bool acceptsUnderscore);
    std::shared_ptr<QueryInput> entRef(std::set<EntityType> allowedDesignEntities, bool acceptsUnderscore);
    bool modifies();
    bool uses();
    bool parent();
    bool follows();
    bool calls();
    bool next();
    bool nextBip();
    bool affects();
    bool affectsBip();
    std::shared_ptr<Expression> expressionSpec();
    void expression(Expression& result);
    void term(Expression& result);
    void factor(Expression& result);

public:

    QueryParser(std::shared_ptr<TokenizerInterface> tokenizer, std::shared_ptr<QueryInterface> query);

    void parse();

    // Getters for testing
    std::unordered_map<std::string, EntityType> getSynonyms();
    std::shared_ptr<QueryInterface> getQuery();

};

#endif