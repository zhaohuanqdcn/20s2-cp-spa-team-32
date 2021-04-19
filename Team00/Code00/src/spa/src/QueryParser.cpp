#include "QueryParser.h"
#include "Tokenizer.h"
#include "EntitiesTable.h"
#include "QueryParserErrorUtility.h"

#include <algorithm>  // for std::find
#include <iterator>  // for std::begin, std::end
#include <iostream>

// =========================================================================================================================

QueryParser::QueryParser(std::shared_ptr<TokenizerInterface> tokenizer, std::shared_ptr<QueryInterface> query) : tokenizer(tokenizer), query(query)
{

}

/*
    Sets the current token to the next token obtained from the Tokenizer.
*/
void QueryParser::getNextToken()
{
    currToken = std::move(tokenizer->readNext());
}

/*
    This function returns the current token if we are able consume it; the current token
    has the correct type as what we are looking to consume.
    If not, we return null.
*/
std::shared_ptr<Token> QueryParser::accept(TokenTypes type)
{
    if (!currToken) return std::shared_ptr<Token>();  // End of query reached
    if (currToken->getType() == type) {
        std::shared_ptr<Token> prevToken = std::move(currToken);
        getNextToken();
        return prevToken;
    }
    // Check if the current token is a keyword token (if so, it can be treated as identifier if we are accepting identifiers)
    if (type == TokenTypes::Identifier && Tokenizer::canTreatAsIdent(currToken->getType(), currToken->getValue())) {
        std::shared_ptr<Token> prevToken = std::move(currToken);
        getNextToken();
        return prevToken;
    }
    // Check if the current token has symbol in it (e.g. Follows* , Parent* or all relationship transitive tokens)
    // We will have to break this token up, taking the identifier and storing the symbol '*' into the tokenizer buffer
    if (type == TokenTypes::Identifier && Tokenizer::hasSymbolInToken(currToken->getType(), currToken->getValue())) {
        std::shared_ptr<Token> prevToken = Tokenizer::convertTokenWithSymbolToIdentifierToken(currToken->getType(), tokenizer);
        getNextToken();
        return prevToken;
    }
    return std::shared_ptr<Token>();
}

/*
    This function returns the current token if we are able consume it; the current token
    has the correct type as what we are looking to consume.
    If not, we will thrown a Syntactic error as we are expecting a token of the specified type.
*/
std::shared_ptr<Token> QueryParser::expect(TokenTypes type)
{
    std::shared_ptr<Token> token = std::move(accept(type));
    if (token) {
        return token;
    }
    // Could not match the expected token type: either no tokens left or currToken does not match
    if (currToken) {  // If there are still tokens left
        QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString());
    }
    else {  // No more tokens left
        QueryParserErrorUtility::unexpectedQueryEndSyntacticException();
    }
}

void QueryParser::selectClause()
{
    // Can have zero or more declarations
    while (declaration());

    expect(TokenTypes::Select);
    
    resultClause();

    // Can have any number of such-that and pattern clauses in any order
    while (suchThatClause() || withClause() || patternClause());
}

void QueryParser::resultClause()
{
    if (accept(TokenTypes::Boolean)) {
        query->setIsBooleanQuery();  // In order to determine whether to return "FALSE" or empty result list
        return;
    }
    if (tuple()) {
        return;
    }
    QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), RES_CLAUSE_STR);
}

bool QueryParser::tuple()
{
    if (elem()) {
        return true;
    }
    else if (accept(TokenTypes::LeftAngleBracket)) {
        if (!elem())  // must have at least one elem in tuple
            QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), TUPLE_STR);
        while (accept(TokenTypes::Comma)) {
            if (!elem())  // must have at least one elem after each comma
                QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), TUPLE_STR);
        }
        expect(TokenTypes::RightAngleBracket);
        return true;
    }
    else {
        return false;
    }
}

bool QueryParser::elem()
{
    std::shared_ptr<Token> token = std::move(accept(TokenTypes::Identifier));
    if (token) {

        // Check for undeclared synonym
        QueryParserErrorUtility::semanticCheckUndeclaredSynonym(synonyms, RES_CLAUSE_STR, token->getValue());

        if (accept(TokenTypes::Dot)) {
            std::shared_ptr<Token> attrNameToken = std::move(expect(TokenTypes::AttrName));

            // Check for semantically incorrect attribute names for certain synonyms (e.g. constant.procName is invalid)
            QueryParserErrorUtility::semanticCheckInvalidAttrForSynonym(synonyms, attrNameToken->getValue(), token->getValue());

            auto queryInput = std::make_shared<Declaration>(synonyms[token->getValue()], token->getValue());

            // We call setIsAttribute on Declarations that represent secondary attribute names used for a synonym
            // A secondary attribute is defined as the attribute that is not implied by the synonym alone
            // For example, for stmt s, stmt# is implied because when we "Select s" that is what we return
            // The only attributes that require setIsAttribute to be called are print.varName, read.varName and call.procName
            // The primary attributes for these are print.stmt#, read.stmt# and call.stmt# respectively
            if (EntitiesTable::isSecondaryAttr(synonyms[token->getValue()], attrNameToken->getValue()))
                queryInput->setIsAttribute();

            query->addDeclarationToSelectClause(queryInput);
        }
        else {
            auto queryInput = std::make_shared<Declaration>(synonyms[token->getValue()], token->getValue());
            query->addDeclarationToSelectClause(queryInput);
        }

        return true;
    }
    return false;
}

bool QueryParser::declaration()
{
    std::shared_ptr<Token> designEntity = std::move(accept(TokenTypes::DesignEntity));
    if (designEntity) {
        std::shared_ptr<Token> synToken = std::move(expect(TokenTypes::Identifier));

        // Throw an error if synonyms are redeclared as a different entity type
        QueryParserErrorUtility::semanticCheckRedeclaredSynonym(synonyms, synToken->getValue(), designEntity->getEntityType());

        synonyms[synToken->getValue()] = designEntity->getEntityType();
        // std::cout << synToken->getValue() << "\n";
        while (accept(TokenTypes::Comma)) {
            synToken = std::move(expect(TokenTypes::Identifier));

            // Throw an error if synonyms are redeclared as a different entity type
            QueryParserErrorUtility::semanticCheckRedeclaredSynonym(synonyms, synToken->getValue(), designEntity->getEntityType());

            synonyms[synToken->getValue()] = designEntity->getEntityType();
        }
        expect(TokenTypes::Semicolon);
        return true;
    }
    return false;
}

bool QueryParser::suchThatClause()
{
    if (accept(TokenTypes::Such)) {
        expect(TokenTypes::That);
        relRef();
        while (accept(TokenTypes::And)) {  // Can have any number of 'and' relRef
            relRef();
        }
        return true;
    }
    return false;
}

void QueryParser::relRef()
{
    if (modifies() || uses() || follows() || parent() || calls() || next() || affects() || nextBip() || affectsBip()) {
        return;
    }
    QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), RELREF_STR);
}

std::shared_ptr<QueryInput> QueryParser::stmtRef(std::set<EntityType> allowedDesignEntities, bool acceptsUnderscore)
{
    std::shared_ptr<Token> token = std::move(accept(TokenTypes::Identifier));
    if (token) {
        // Check for undeclared synonym
        QueryParserErrorUtility::semanticCheckUndeclaredSynonym(synonyms, STMTREF_STR, token->getValue());

        // Check that synonym has entity that is allowed
        QueryParserErrorUtility::semanticCheckValidSynonymEntityType(synonyms, token->getValue(), allowedDesignEntities);

        return std::make_shared<Declaration>(synonyms[token->getValue()], token->getValue());
    }
    token = std::move(accept(TokenTypes::Underscore));
    if (token) {
        QueryParserErrorUtility::semanticCheckWildcardAllowed(acceptsUnderscore, token->getValue(), STMTREF_STR);
        return std::make_shared<Any>(token->getValue());
    }
    token = std::move(accept(TokenTypes::Integer));
    if (token) {
        return std::make_shared<StmtNum>(token->getValue());
    }
    QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), STMTREF_STR);
}

std::shared_ptr<QueryInput> QueryParser::entRef(std::set<EntityType> allowedDesignEntities, bool acceptsUnderscore)
{
    std::shared_ptr<Token> token = std::move(accept(TokenTypes::Identifier));
    if (token) {
        // Check for undeclared synonym
        QueryParserErrorUtility::semanticCheckUndeclaredSynonym(synonyms, STMTREF_STR, token->getValue());

        // Check that synonym has entity that is allowed
        QueryParserErrorUtility::semanticCheckValidSynonymEntityType(synonyms, token->getValue(), allowedDesignEntities);

        return std::make_shared<Declaration>(synonyms[token->getValue()], token->getValue());
    }
    token = std::move(accept(TokenTypes::Underscore));
    if (token) {
        QueryParserErrorUtility::semanticCheckWildcardAllowed(acceptsUnderscore, token->getValue(), ENTREF_STR);
        return std::make_shared<Any>(token->getValue());
    }
    else if (accept(TokenTypes::DoubleQuote)) {
        token = std::move(expect(TokenTypes::Identifier));
        expect(TokenTypes::DoubleQuote);
        return std::make_shared<Ident>(token->getValue());
    }
    else {
        QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), ENTREF_STR);
    }
}

bool QueryParser::modifies()
{
    if (accept(TokenTypes::Modifies)) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput;
        if (currToken->getType() == TokenTypes::DoubleQuote) {
            leftQueryInput = entRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::MODIFIES), false);
        }
        else {
            leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::MODIFIES), false);
        }
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = entRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::MODIFIES), true);
        expect(TokenTypes::RightParen);
        query->addRelationshipClause(RelationshipType::MODIFIES, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::uses()
{
    if (accept(TokenTypes::Uses)) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput;
        if (currToken->getType() == TokenTypes::DoubleQuote) {
            leftQueryInput = entRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::USES), false);
        }
        else {
            leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::USES), false);
        }
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = entRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::USES), true);
        expect(TokenTypes::RightParen);
        query->addRelationshipClause(RelationshipType::USES, leftQueryInput, rightQueryInput);
        return true;
    }
    return false;
}

bool QueryParser::parent()
{
    RelationshipType relType;
    std::string relRef;
    bool relFound = false;
    if (accept(TokenTypes::Parent)) {
        relType = RelationshipType::PARENT;
        relRef = PARENT_STR;
        relFound = true;
    } 
    else if (accept(TokenTypes::ParentT)) {
        relType = RelationshipType::PARENT_T;
        relRef = PARENT_T_STR;
        relFound = true;
    }

    if (relFound) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::PARENT), true);
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = stmtRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::PARENT), true);
        expect(TokenTypes::RightParen);
        // Semantic checks for Parent
        // Cannot have same synonym on both sides
        QueryParserErrorUtility::semanticCheckSameSynonymBothSides(leftQueryInput, rightQueryInput, relRef);

        // Cannot have a statement after being a parent of a statement before
        QueryParserErrorUtility::semanticCheckLeftStmtNumGtrEqualsRightStmtNum(leftQueryInput, rightQueryInput, relRef);

        query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
        return true;
    }
    else {
        return false;
    }
}

bool QueryParser::follows()
{
    RelationshipType relType;
    std::string relRef;
    bool relFound = false;
    if (accept(TokenTypes::Follows)) {
        relType = RelationshipType::FOLLOWS;
        relRef = FOLLOWS_STR;
        relFound = true;
    }
    else if (accept(TokenTypes::FollowsT)) {
        relType = RelationshipType::FOLLOWS_T;
        relRef = FOLLOWS_T_STR;
        relFound = true;
    }

    if (relFound) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::FOLLOWS), true);
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = stmtRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::FOLLOWS), true);
        expect(TokenTypes::RightParen);
        // Semantic checks for Follows
        // Cannot have same synonym on both sides
        QueryParserErrorUtility::semanticCheckSameSynonymBothSides(leftQueryInput, rightQueryInput, relRef);

        // Cannot have a statement coming after following one that comes before
        QueryParserErrorUtility::semanticCheckLeftStmtNumGtrEqualsRightStmtNum(leftQueryInput, rightQueryInput, relRef);

        query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
        return true;
    }
    else {
        return false;
    }
}

bool QueryParser::calls()
{
    RelationshipType relType;
    std::string relRef;
    bool relFound = false;
    if (accept(TokenTypes::Calls)) {
        relType = RelationshipType::CALLS;
        relRef = CALLS_STR;
        relFound = true;
    }
    else if (accept(TokenTypes::CallsT)) {
        relType = RelationshipType::CALLS_T;
        relRef = CALLS_T_STR;
        relFound = true;
    }

    if (relFound) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = entRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::CALLS), true);
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = entRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::CALLS), true);
        expect(TokenTypes::RightParen);
        // Semantic checks for Calls
        // Cannot have same synonym on both sides
        QueryParserErrorUtility::semanticCheckSameSynonymBothSides(leftQueryInput, rightQueryInput, relRef);

        query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
        return true;
    }
    else {
        return false;
    }
}

bool QueryParser::next()
{
    RelationshipType relType;
    std::string relRef;
    bool relFound = false;
    if (accept(TokenTypes::Next)) {
        relType = RelationshipType::NEXT;
        relRef = NEXT_STR;
        relFound = true;
    }
    else if (accept(TokenTypes::NextT)) {
        relType = RelationshipType::NEXT_T;
        relRef = NEXT_T_STR;
        relFound = true;
    }

    if (relFound) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::NEXT), true);
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = stmtRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::NEXT), true);
        expect(TokenTypes::RightParen);

        if (relType == RelationshipType::NEXT) {
            // Semantic checks for Next
            // Cannot have same synonym on both sides, cannot have Next(s, s) but Next*(s, s) is okay
            QueryParserErrorUtility::semanticCheckSameSynonymBothSides(leftQueryInput, rightQueryInput, relRef);

            // Cannot have Next(7, 7), but Next*(7, 7) is okay    
            QueryParserErrorUtility::semanticCheckLeftStmtNumEqualsRightStmtNum(leftQueryInput, rightQueryInput, relRef);
        }

        query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
        return true;
    }
    else {
        return false;
    }
}

bool QueryParser::nextBip()
{
    RelationshipType relType;
    std::string relRef;
    bool relFound = false;
    if (accept(TokenTypes::NextBip)) {
        relType = RelationshipType::NEXTBIP;
        relRef = NEXTBIP_STR;
        relFound = true;
    }
    else if (accept(TokenTypes::NextBipT)) {
        relType = RelationshipType::NEXTBIP_T;
        relRef = NEXTBIP_T_STR;
        relFound = true;
    }

    if (relFound) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::NEXTBIP), true);
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = stmtRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::NEXTBIP), true);
        expect(TokenTypes::RightParen);

        if (relType == RelationshipType::NEXTBIP) {
            // Semantic checks for NextBip
            // Cannot have same synonym on both sides
            QueryParserErrorUtility::semanticCheckSameSynonymBothSides(leftQueryInput, rightQueryInput, relRef);

            // Cannot have NextBip(7, 7), but NextBip*(7, 7) is okay
            QueryParserErrorUtility::semanticCheckLeftStmtNumEqualsRightStmtNum(leftQueryInput, rightQueryInput, relRef);
        }

        query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
        return true;
    }
    else {
        return false;
    }
}

bool QueryParser::affects()
{
    RelationshipType relType;
    std::string relRef;
    bool relFound = false;
    if (accept(TokenTypes::Affects)) {
        relType = RelationshipType::AFFECTS;
        relRef = AFFECTS_STR;
        relFound = true;
    }
    else if (accept(TokenTypes::AffectsT)) {
        relType = RelationshipType::AFFECTS_T;
        relRef = AFFECTS_T_STR;
        relFound = true;
    }

    if (relFound) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::AFFECTS), true);
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = stmtRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::AFFECTS), true);
        expect(TokenTypes::RightParen);

        query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
        return true;
    }
    else {
        return false;
    }
}

bool QueryParser::affectsBip()
{
    RelationshipType relType;
    std::string relRef;
    bool relFound = false;
    if (accept(TokenTypes::AffectsBip)) {
        relType = RelationshipType::AFFECTSBIP;
        relRef = AFFECTSBIP_STR;
        relFound = true;
    }
    else if (accept(TokenTypes::AffectsBipT)) {
        relType = RelationshipType::AFFECTSBIP_T;
        relRef = AFFECTSBIP_T_STR;
        relFound = true;
    }

    if (relFound) {
        expect(TokenTypes::LeftParen);
        std::shared_ptr<QueryInput> leftQueryInput = stmtRef(EntitiesTable::getRelAllowedLeftEntities(RelationshipType::AFFECTSBIP), true);
        expect(TokenTypes::Comma);
        std::shared_ptr<QueryInput> rightQueryInput = stmtRef(EntitiesTable::getRelAllowedRightEntities(RelationshipType::AFFECTSBIP), true);
        expect(TokenTypes::RightParen);

        query->addRelationshipClause(relType, leftQueryInput, rightQueryInput);
        return true;
    }
    else {
        return false;
    }
}

bool QueryParser::withClause()
{
    if (accept(TokenTypes::With)) {
        attrCompare();
        while (accept(TokenTypes::And)) {  // Can have any number of 'and' attrCompare
            attrCompare();
        }
        return true;
    }
    return false;
}

void QueryParser::attrCompare()
{
    std::shared_ptr<QueryInput> leftRef = ref();
    expect(TokenTypes::Equals);
    std::shared_ptr<QueryInput> rightRef = ref();

    // Check that the ref on both sides have the same type so that they can be compared
    QueryParserErrorUtility::semanticCheckValidAttrCompare(leftRef, rightRef);

    // Remove meaningless queries
    if (!QueryParserErrorUtility::isMeaninglessAttrCompare(leftRef, rightRef))
        query->addWithClause(leftRef, rightRef);
}

std::shared_ptr<QueryInput> QueryParser::ref()
{
    std::shared_ptr<Token> token = std::move(accept(TokenTypes::Integer));
    if (token) {
        auto queryInput = std::make_shared<StmtNum>(token->getValue());
        return queryInput;
    }
    token = std::move(accept(TokenTypes::Identifier));
    if (token) {
        // Check for undeclared synonym
        QueryParserErrorUtility::semanticCheckUndeclaredSynonym(synonyms, REF_STR, token->getValue());
        std::shared_ptr<Declaration> queryInput;
        if (accept(TokenTypes::Dot)) {
            std::shared_ptr<Token> attrNameToken = std::move(expect(TokenTypes::AttrName));

            // Check for semantically incorrect attribute names for certain synonyms (e.g. constant.procName is invalid)
            QueryParserErrorUtility::semanticCheckInvalidAttrForSynonym(synonyms, attrNameToken->getValue(), token->getValue());

            queryInput = std::make_shared<Declaration>(synonyms[token->getValue()], token->getValue());

            if (EntitiesTable::isSecondaryAttr(synonyms[token->getValue()], attrNameToken->getValue()))
                queryInput->setIsAttribute();
        }
        else {
            // Synonym in with clause must be of type prog_line
            QueryParserErrorUtility::semanticCheckWithClauseSynonym(synonyms[token->getValue()], token->getValue());

            queryInput = std::make_shared<Declaration>(synonyms[token->getValue()], token->getValue());
        }
        return queryInput;
    }
    if (accept(TokenTypes::DoubleQuote)) {
        token = std::move(expect(TokenTypes::Identifier));
        expect(TokenTypes::DoubleQuote);

        auto queryInput = std::make_shared<Ident>(token->getValue());

        return queryInput;
    }
    // Ref could not be parsed correctly
    QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), REF_STR);
}

bool QueryParser::patternClause()
{
    if (accept(TokenTypes::Pattern)) {

        bool patternFound = false;

        while (!patternFound || accept(TokenTypes::And)) {
            std::shared_ptr<Token> synToken = std::move(expect(TokenTypes::Identifier));

            // Semantic check if synonym has been declared before being used
            QueryParserErrorUtility::semanticCheckUndeclaredSynonym(synonyms, PATTERN_CLAUSE_STR, synToken->getValue());

            // Find the Entity Type of the synonym
            auto it = synonyms.find(synToken->getValue());
            EntityType synonymType = it->second;

            // Semantic check if synonym does not have valid entity type in context of pattern clause
            QueryParserErrorUtility::semanticCheckPatternClauseSynonym(synonymType, synToken->getValue());

            // Compulsory to match at least one pattern
            if (synonymType == EntityType::ASSIGN) {
                patternAssign(synToken->getValue());
            }
            else if (synonymType == EntityType::WHILE) {
                patternWhile(synToken->getValue());
            }
            else {  // synonymType == EntityType::IF
                patternIf(synToken->getValue());
            }

            patternFound = true;
        }
        return true;
    }
    return false;
}

void QueryParser::patternAssign(std::string synonymValue)
{
    auto synonym = std::make_shared<Declaration>(synonyms[synonymValue], synonymValue);
    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> queryInput = entRef(EntitiesTable::getPatternAllowedLeftEntities(PatternType::PATTERN_ASSIGN), true);
    expect(TokenTypes::Comma);
    std::shared_ptr<Expression> expression = expressionSpec();
    expect(TokenTypes::RightParen);
    query->addAssignPatternClause(synonym, queryInput, expression);
}

void QueryParser::patternWhile(std::string synonymValue)
{
    auto synonym = std::make_shared<Declaration>(synonyms[synonymValue], synonymValue);
    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> queryInput = entRef(EntitiesTable::getPatternAllowedLeftEntities(PatternType::PATTERN_WHILE), true);
    expect(TokenTypes::Comma);
    expect(TokenTypes::Underscore);
    expect(TokenTypes::RightParen);
    query->addContainerPatternClause(synonym, queryInput);
}

void QueryParser::patternIf(std::string synonymValue)
{
    auto synonym = std::make_shared<Declaration>(synonyms[synonymValue], synonymValue);
    expect(TokenTypes::LeftParen);
    std::shared_ptr<QueryInput> queryInput = entRef(EntitiesTable::getPatternAllowedLeftEntities(PatternType::PATTERN_IF), true);
    expect(TokenTypes::Comma);
    expect(TokenTypes::Underscore);
    expect(TokenTypes::Comma);
    expect(TokenTypes::Underscore);
    expect(TokenTypes::RightParen);
    query->addContainerPatternClause(synonym, queryInput);
}

std::shared_ptr<Expression> QueryParser::expressionSpec()
{
    if (accept(TokenTypes::Underscore)) {
        // Parse _"exp"_ expressionSpec
        if (accept(TokenTypes::DoubleQuote)) {
            Expression result("", ExpressionType::PARTIAL);
            expression(result);
            expect(TokenTypes::DoubleQuote);
            expect(TokenTypes::Underscore);
            return std::make_shared<Expression>(result);
        }
        // Parse _ expressionSpec
        return std::make_shared<Expression>("_", ExpressionType::EMPTY);
    }
    // Parse "exp" expressionSpec
    expect(TokenTypes::DoubleQuote);
    Expression result("", ExpressionType::EXACT);
    expression(result);
    expect(TokenTypes::DoubleQuote);
    return std::make_shared<Expression>(result);
}

void QueryParser::expression(Expression& result)
{
    term(result);
    std::shared_ptr<Token> token = std::move(accept(TokenTypes::ExprSymbol));
    while (token) {
        Expression subResult("", result.getType());
        term(subResult);
        result = result.combineExpression(token->getValue(), subResult, result.getType());
        // Try to accept another ExprSymbol to loop again
        token = std::move(accept(TokenTypes::ExprSymbol));
    }
}

void QueryParser::term(Expression& result)
{
    factor(result);
    std::shared_ptr<Token> token = std::move(accept(TokenTypes::TermSymbol));
    while (token) {
        Expression subResult("", result.getType());
        factor(subResult);
        result = result.combineExpression(token->getValue(), subResult, result.getType());
        // Try to accept another TermSymbol to loop again
        token = std::move(accept(TokenTypes::TermSymbol));
    }
}

void QueryParser::factor(Expression& result)
{
    std::shared_ptr<Token> token = std::move(accept(TokenTypes::Identifier));
    if (token) {
        result = Expression(token->getValue(), result.getType());
        return;
    }
    token = std::move(accept(TokenTypes::Integer));
    if (token) {
        result = Expression(token->getValue(), result.getType());
        return;
    }
    if (accept(TokenTypes::LeftParen)) {
        expression(result);
        expect(TokenTypes::RightParen);
        return;
    }
    // Factor could not be parsed correctly
    QueryParserErrorUtility::unexpectedTokenSyntacticException(currToken->toString(), FACTOR_STR);
}

void QueryParser::parse()
{
    getNextToken();
    selectClause();
    // Ensure that no more tokens are left, syntax error if tokens remain
    if (currToken)
        QueryParserErrorUtility::tokensRemainingAfterParsingCompleted(currToken->getValue());
}

// Getters for testing

std::unordered_map<std::string, EntityType> QueryParser::getSynonyms()
{
    return synonyms;
}

std::shared_ptr<QueryInterface> QueryParser::getQuery()
{
    return query;
}