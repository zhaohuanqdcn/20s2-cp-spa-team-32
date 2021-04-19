#ifndef __PARSER__H__
#define __PARSER__H__

using namespace std;

#include "SimpleParseError.h"
#include "SIMPLETokenStream.h"
#include "DesignExtractor.h"

enum ParseResult {
    EXPRESSION,
    CONDITION_N0,
    CONDITION_N1,
    NULL_RESULT,
    FAILED
};

ParseResult combineResult(ParseResult first, ParseResult second, SIMPLEToken operand);

class Parser {
private:
    DesignExtractor& designExtractor;

    ///total number of statement has been parsed
    int numberOfStatements;

    ///current procedure name
    string currentProcName;

    ///set of all variables
    set<string> variables;

    ///set of all literals
    set<string> literals;

    ///set of all procedures
    set<string> procedures;
    
    ///range of procedures
    map<string, pair<int, int> > rangeProcedure;

    ///all calling relationships.
    unordered_map<string, vector<string> > callingRelationships;

    void addCallingRelationship(int, string callee);

    ///set the first statement index of the procName
    void startAProcedure(string procName);

    ///set the last statement index of the procName
    void endAProcedure(string procName);

    ///add one literals
    void addLiterals(string literals);

    ///add a new variable to the table
    void addVariable(string variableName);

    ///add to DesignExtractor
    void addModifies(int statement, const string& variable);

    ///add to DesignExtractor    
    void addUses(int statement, const string& variable);

    ///add to DesignExtractor
    void addParentRelationship(int parent, int child);

    ///add expression
    void addExpression(int statement, Expression expression);

    void addFollowRelationship(int before, int after);

    ///increment number of statement
    int startNewStatement(int parentStatement, EntityType type);

    ///return the next statement's index
    int expectNextStatementIndex();

    /*
        According to SIMPLE grammar rules.
    */
    ParseError parseAssignmentStatement(SIMPLETokenStream& stream, int parentStatementIndex);

    ParseError parseWhileStatement(SIMPLETokenStream& stream, int parentStatementIndex);

    ParseError parseIfStatement(SIMPLETokenStream& stream, int parentStatementIndex);

    ParseError parseStatementList(SIMPLETokenStream& stream, int parentStatementIndex = -1);

    ParseError parsePrintStatement(SIMPLETokenStream& stream, int parentStatementIndex);

    ParseError parseCallStatement(SIMPLETokenStream &stream, int parentStatementIndex);

    ParseError parseFactor(SIMPLETokenStream &stream, Expression& result, int userStatement);

    ParseError parseTerm(SIMPLETokenStream& stream, Expression& result, int userStatement);

    /*
        This one use operator precedence parsing, hence the level is needed. 
    */
    ParseError parseConditionOrExpression(int level, SIMPLETokenStream& stream, 
        ParseResult& result, ParseResult currentResult, int userStatement);
public:
    /**
    * Constructor for Parser
    */
    Parser(DesignExtractor& extractor);

    /**
     * Parses a Token Stream and adds information to design extractor
     * returns UnexpectedToken error only
     */
    ParseError parseExpression(SIMPLETokenStream& stream, Expression& result, int userStatement);

    ParseError parseConditionExpression(SIMPLETokenStream& stream, int userStatement);

    ParseError parseProcedure(SIMPLETokenStream& stream);

    ParseError parseReadStatement(SIMPLETokenStream& stream, int parentStatementIndex);

    ParseError parseProgram(SIMPLETokenStream& stream);
};

#endif ///__PARSER__H__