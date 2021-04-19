using namespace std;

#include "Parser.h"
#include "ExpressionType.h"
#include "ParserHelper.h"

Parser::Parser(DesignExtractor& extractor) : designExtractor(extractor) {
	this->numberOfStatements = 0;
	variables.clear();
	literals.clear();
}

void Parser::addLiterals(string literal) {
	if (literals.find(literal) != literals.end()) {
		literals.insert(literal);
	}
}

void Parser::addParentRelationship(int parent, int child) {
	if (parent != -1 && child != -1) {
		designExtractor.insertParent(parent, child);
	}
}

void Parser::addVariable(string variableName) {
	if (variables.find(variableName) == variables.end()) {
		variables.insert(variableName);
	}
}

void Parser::startAProcedure(string procName) {
	rangeProcedure[procName].first = this->expectNextStatementIndex();
	currentProcName = procName;
	procedures.insert(procName);
}

void Parser::endAProcedure(string procName) {
	rangeProcedure[procName].second = this->numberOfStatements;
	this->designExtractor.setProcedure(
		procName, 
		rangeProcedure[procName].first, 
		rangeProcedure[procName].second
	);
}

int Parser::startNewStatement(int parentStatement, EntityType type) {
	int newIndex = ++numberOfStatements;
	this->designExtractor.increaseNumberOfStatement(type);
	this->addParentRelationship(parentStatement, newIndex);
	return newIndex;
}

void Parser::addUses(int statementId, const string& variable) {
	this->addVariable(variable);
	this->designExtractor.insertUses(statementId, variable);
}

void Parser::addModifies(int statementId, const string& variable) {
	this->addVariable(variable);
	this->designExtractor.insertModifies(statementId, variable);
}

void Parser::addFollowRelationship(int before, int after) {
	this->designExtractor.insertFollow(before, after);
}

void Parser::addExpression(int statementId, Expression expression) {
	this->designExtractor.insertExpression(statementId, expression);
}

void Parser::addCallingRelationship(int statementId, string callee) {
	this->callingRelationships[currentProcName].emplace_back(callee);
	this->designExtractor.insertStatementCall(statementId, callee);
}

ParseResult combineResult(ParseResult first, ParseResult second, SIMPLEToken operand) {
	ParseResult answer = FAILED;
	if (operand.getValue() == "(") {
		if (first != NULL_RESULT) {
			return FAILED;
		}
		if (second == CONDITION_N0) {
			return CONDITION_N1;
		}
		if (second == EXPRESSION) {
			return EXPRESSION;
		}
		return FAILED;
	}

	if (operand.getValue() == "!") {
		if (first != NULL_RESULT) {
			return FAILED;
		}
		if (second != CONDITION_N1) {
			return FAILED;
		}
		return CONDITION_N0;
	}

	if (operand.getTokenType() == TokenType::COND_SYMBOL) {
		if (first != CONDITION_N1) {
			return FAILED;
		}
		if (second != CONDITION_N1) {
			return FAILED;
		}
		return CONDITION_N0;
	}

	if (operand.getTokenType() == TokenType::REL_SYMBOL) {
		if (first != EXPRESSION) {
			return FAILED;
		}
		if (second != EXPRESSION) {
			return FAILED;
		}
		return CONDITION_N0;
	}

	if (operand.getTokenType() == TokenType::EXPR_SYMBOL) {
		if (first != EXPRESSION) {
			return FAILED;
		}
		if (second != EXPRESSION) {
			return FAILED;
		}
		return EXPRESSION;
	}

	if (operand.getTokenType() == TokenType::TERM_SYMBOL) {
		if (first != EXPRESSION) {
			return FAILED;
		}
		if (second != EXPRESSION) {
			return FAILED;
		}
		return EXPRESSION;
	}

	return FAILED;
}

ParseError consumeToken(const TokenType& type, SIMPLETokenStream& stream, SIMPLEToken& token) {
	///firstly, check whether the stream is empty
	if (stream.isEmpty()) {
		return ParseError(SIMPLEToken(), "Expect more tokens");
	}

	///next, consume one token from the stream
	token = stream.getToken();
	if (token.getTokenType() != type) {
		return ParseError(token, "Invalid token");
	}
	else {
		return ParseError();
	}
}

ParseError consumeTerminal(const string& value, SIMPLETokenStream& stream) {
	//firstly, check whether the stream is empty
	if (stream.isEmpty()) {
		return ParseError(SIMPLEToken(), "Expect more tokens");
	}

	///next, consume one token from the stream
	auto token = stream.getToken();
	if (token.getValue() != value) {
		return ParseError(token, "Invalid token");
	}
	else {
		return ParseError();
	}
}

ParseError Parser::parseProgram(SIMPLETokenStream &stream) {
	while (!stream.isEmpty()) {
		auto error = parseProcedure(stream);
		if (error.hasError()) {
			return error;
		}
	}
	if (checkCallOfNonProcedure(this->callingRelationships, this->procedures)) {
		return ParseError("There is a call of non-exist procedure");
	}
	if (checkCyclicCalls(this->callingRelationships)) {
		return ParseError("There is a cyclic call");
	}

	this->designExtractor.setCalls(callingRelationships);
	return ParseError();
}

ParseError Parser::parseProcedure(SIMPLETokenStream& stream) {
	SIMPLEToken procedureName;
	auto error = consumeTerminal("procedure", stream);
	error = error.combineWith(consumeToken(TokenType::NAME, stream, procedureName));
	
	///check duplication of procedure names
	if (procedures.find(procedureName.getValue()) != procedures.end())  {
		return ParseError("Two procedures have the same name");
	}

	this->startAProcedure(procedureName.getValue());
	error = error.combineWith(consumeTerminal("{", stream));
	error = error.combineWith(parseStatementList(stream));
	error = error.combineWith(consumeTerminal("}", stream));
	this->endAProcedure(procedureName.getValue());
	return error;
}

inline int Parser::expectNextStatementIndex() {
	return numberOfStatements + 1;
}

ParseError Parser::parseAssignmentStatement(SIMPLETokenStream& stream, int parentStatementIndex = -1) {
	int thisStatementIndex = this->startNewStatement(parentStatementIndex, EntityType::ASSIGN);

	SIMPLEToken leftHandSide;
	Expression result("", ExpressionType::EMPTY);
	auto error = consumeToken(TokenType::NAME, stream, leftHandSide);
	error = error.combineWith(consumeTerminal("=", stream));
	error = error.combineWith(parseExpression(stream, result, thisStatementIndex));
	error = error.combineWith(consumeTerminal(";", stream));

	result.setType(ExpressionType::EXACT);
	this->addExpression(thisStatementIndex, result);
	if (error.hasError()) {
		return error;
	}

	this->addModifies(thisStatementIndex, leftHandSide.getValue());
	return ParseError();
}

ParseError Parser::parseCallStatement(SIMPLETokenStream &stream, int parentStatementIndex) {
	int thisStatementIndex = this->startNewStatement(parentStatementIndex, EntityType::CALL);

	SIMPLEToken calleeName;
	auto error = consumeTerminal("call", stream);
	error = error.combineWith(consumeToken(TokenType::NAME, stream, calleeName));
	error = error.combineWith(consumeTerminal(";", stream));

	addCallingRelationship(thisStatementIndex, calleeName.getValue());

	return error;
}

ParseError Parser::parseReadStatement(SIMPLETokenStream &stream, int parentStatementIndex = -1) {
	int thisStatementIndex = this->startNewStatement(parentStatementIndex, EntityType::READ);

	SIMPLEToken variable;
	auto error = consumeTerminal("read", stream);
	error = error.combineWith(consumeToken(TokenType::NAME, stream, variable));
	error = error.combineWith(consumeTerminal(";", stream));

	if (error.hasError()) {
		return error;
	}

	this->addModifies(thisStatementIndex, variable.getValue());
	return ParseError();
}

ParseError Parser::parsePrintStatement(SIMPLETokenStream& stream, int parentStatementIndex = -1) {
	int thisStatementIndex = this->startNewStatement(parentStatementIndex, EntityType::PRINT);
	SIMPLEToken variable;
	auto error = consumeTerminal("print", stream);
	error = error.combineWith(consumeToken(TokenType::NAME, stream, variable));
	error = error.combineWith(consumeTerminal(";", stream));;

	if (error.hasError()) {
		return error;
	}

	this->addUses(thisStatementIndex, variable.getValue());
	return ParseError();
}

ParseError Parser::parseWhileStatement(SIMPLETokenStream& stream, int parentStatementIndex = -1) {
	int thisStatementIndex = this->startNewStatement(parentStatementIndex, EntityType::WHILE);

	auto error = consumeTerminal("while", stream);
	error = error.combineWith(consumeTerminal("(", stream));
	error = error.combineWith(parseConditionExpression(stream, thisStatementIndex));
	error = error.combineWith(consumeTerminal(")", stream));
	error = error.combineWith(consumeTerminal("{", stream));
	error = error.combineWith(parseStatementList(stream, thisStatementIndex));
	error = error.combineWith(consumeTerminal("}", stream));


	if (error.hasError()) {
		return error;
	}


	return ParseError();
}

ParseError Parser::parseIfStatement(SIMPLETokenStream& stream, int parentStatementIndex = -1) {
	int thisStatementIndex = this->startNewStatement(parentStatementIndex, EntityType::IF);

	auto error = consumeTerminal("if", stream);
	error = error.combineWith(consumeTerminal("(", stream));
	error = error.combineWith(parseConditionExpression(stream, thisStatementIndex));
	error = error.combineWith(consumeTerminal(")", stream));
	error = error.combineWith(consumeTerminal("then", stream));
	error = error.combineWith(consumeTerminal("{", stream));
	error = error.combineWith(parseStatementList(stream, thisStatementIndex));
	error = error.combineWith(consumeTerminal("}", stream));
	error = error.combineWith(consumeTerminal("else", stream));
	error = error.combineWith(consumeTerminal("{", stream));
	error = error.combineWith(parseStatementList(stream, thisStatementIndex));
	error = error.combineWith(consumeTerminal("}", stream));

	if (error.hasError()) {
		return error;
	}
	return ParseError();
}

ParseError Parser::parseStatementList(SIMPLETokenStream& stream, int parentStatementIndex) {
	if (stream.isEmpty()) {
		return ParseError(SIMPLEToken(), "Unexpected end of file");
	}

	vector<int> listIndices(0);
	while (!stream.isEmpty()) {
		/// need to look ahead 2 tokens to determine which branch to take
		vector<SIMPLEToken> nextTwos = stream.lookAhead(2);

		if (nextTwos.size() < 2) {
			break;
		} else
		if (nextTwos[0].getValue() == "while" && nextTwos[1].getValue() == "(") {
			int nextIndices = this->expectNextStatementIndex();
			listIndices.emplace_back(nextIndices);
			auto error = parseWhileStatement(stream, parentStatementIndex);
			if (error.hasError()) {
				return error;
			}
		} else
		if (nextTwos[0].getValue() == "if" && nextTwos[1].getValue() == "(") {
			int nextIndices = this->expectNextStatementIndex();
			listIndices.emplace_back(nextIndices);
			auto error = parseIfStatement(stream, parentStatementIndex);
			if (error.hasError()) {
				return error;
			}	
		} else 
		if (nextTwos[0].getTokenType() == TokenType::NAME && nextTwos[1].getValue() == "=") {
			int nextIndices = this->expectNextStatementIndex();
			listIndices.emplace_back(nextIndices);
			auto error = parseAssignmentStatement(stream, parentStatementIndex);
			if (error.hasError()) {
				return error;
			}	
		} else
		if (nextTwos[0].getValue() == "read" && nextTwos[1].getTokenType() == TokenType::NAME) {
			int nextIndices = this->expectNextStatementIndex();
			listIndices.emplace_back(nextIndices);
			auto error = parseReadStatement(stream, parentStatementIndex);
			if (error.hasError()) {
				return error;
			}	
		} else 
		if (nextTwos[0].getValue() == "print" && nextTwos[1].getTokenType() == TokenType::NAME) {
			int nextIndices = this->expectNextStatementIndex();
			listIndices.emplace_back(nextIndices);
			auto error = parsePrintStatement(stream, parentStatementIndex);
			if (error.hasError()) {
				return error;
			}	
		} else 
		if (nextTwos[0].getValue() == "call" && nextTwos[1].getTokenType() == TokenType::NAME) {
			int nextIndices = this->expectNextStatementIndex();
			listIndices.emplace_back(nextIndices);
			auto error = parseCallStatement(stream, parentStatementIndex);
			if (error.hasError()) {
				return error;
			}
		} else {
			break;
		}
	}
	
	if (listIndices.size() == 0) {
		return ParseError(stream.getLastConsumedToken(), "Statement list should not be empty");
	}

	for (int i = 0; i + 1 < listIndices.size(); i++) {
		this->addFollowRelationship(listIndices[i], listIndices[i + 1]);
	}

	return ParseError();
}

ParseError Parser::parseFactor(SIMPLETokenStream& stream, Expression& result, int userStatement) {
	if (stream.isEmpty()) {
		return ParseError(SIMPLEToken(), "Unexpected end of file");
	}
	SIMPLEToken nextToken = stream.getToken();

	if (nextToken.getTokenType() == TokenType::NAME) {
		this->addUses(userStatement, nextToken.getValue());
		result = Expression(nextToken.getValue(), ExpressionType::PARTIAL);
		this->addExpression(userStatement, result);
		return ParseError();
	}

	if (nextToken.getTokenType() == TokenType::INTEGER) {
		result = Expression(nextToken.getValue(), ExpressionType::PARTIAL);
		this->addExpression(userStatement, result);
		this->designExtractor.insertConstant(nextToken.getValue());
		return ParseError();
	}

	if (nextToken.getValue() == "(") {
		auto error = parseExpression(stream, result, userStatement);
		error = error.combineWith(consumeTerminal(")", stream));
		return error;
	}

	return ParseError(nextToken, "Unexpected token");
}

ParseError Parser::parseTerm(SIMPLETokenStream& stream, Expression& result, int userStatement) {
	auto error = parseFactor(stream, result, userStatement);

	while (!stream.isEmpty()) {
		SIMPLEToken nextToken = stream.lookAheadSingle();
		if (nextToken.getTokenType() == TokenType::TERM_SYMBOL) {
			Expression subResult("", ExpressionType::EMPTY);
			error = error.combineWith(consumeToken(TokenType::TERM_SYMBOL, stream, nextToken));
			error = error.combineWith(parseFactor(stream, subResult, userStatement));
			result = result.combineExpression(nextToken.getValue(), subResult, ExpressionType::PARTIAL);
			this->addExpression(userStatement, result);
		}
		else {
			return error;
		}
	}

	this->addExpression(userStatement, result);
	return error;
}

ParseError Parser::parseExpression(SIMPLETokenStream& stream, Expression& result, int userStatement) {
	auto error = parseTerm(stream, result, userStatement);

	while (!stream.isEmpty()) {
		SIMPLEToken nextToken = stream.lookAheadSingle();
		if (nextToken.getTokenType() == TokenType::EXPR_SYMBOL) {
			Expression subResult("", ExpressionType::EMPTY);
			error = error.combineWith(consumeToken(TokenType::EXPR_SYMBOL, stream, nextToken));
			error = error.combineWith(parseTerm(stream, subResult, userStatement));
			result = result.combineExpression(nextToken.getValue(), subResult, ExpressionType::PARTIAL);
			this->addExpression(userStatement, result);
		}
		else {
			break;
		}
	}

	result.setType(ExpressionType::PARTIAL);
	this->addExpression(userStatement, result);
	return error;
}

ParseError Parser::parseConditionExpression(SIMPLETokenStream& stream, int userStatement) {
	ParseResult result = ParseResult::FAILED;
	auto error = parseConditionOrExpression(0, stream, result, NULL_RESULT, userStatement);
	if (result != ParseResult::CONDITION_N0) {
		return ParseError(SIMPLEToken(), "Wrong condition expression");
	}
	return error;
}

ParseError Parser::parseConditionOrExpression(int level, SIMPLETokenStream& stream, ParseResult& result, ParseResult currentResult, int userStatement) {
	if (stream.isEmpty()) {
		result = currentResult;
		return ParseError();
	}

	auto error = ParseError();
	result = FAILED;

	/// special case, need a single primary after this 
	if (currentResult == ParseResult::NULL_RESULT) {
		SIMPLEToken nextToken = stream.lookAheadSingle();

		bool receiveTerm = false;
		if (nextToken.getValue() == "(") {
			receiveTerm = true;
			auto error = consumeTerminal("(", stream);
			ParseResult subResult = FAILED;
			error = error.combineWith(parseConditionOrExpression(0, stream, subResult, NULL_RESULT, userStatement));
			error = error.combineWith(consumeTerminal(")", stream));
			switch (subResult) {
			case ParseResult::EXPRESSION:
				currentResult = EXPRESSION;
				break;
			case ParseResult::CONDITION_N0:
				currentResult = ParseResult::CONDITION_N1;
				break;
			default:
				return ParseError(nextToken, "Wrong expression");
			}
		}
		else {
			if (nextToken.getTokenType() == TokenType::NAME || nextToken.getTokenType() == TokenType::INTEGER) {
				receiveTerm = true;
				error = consumeToken(nextToken.getTokenType(), stream, nextToken);
				if (nextToken.getTokenType() == TokenType::NAME) {
					this->addUses(userStatement, nextToken.getValue());
				}
				if (nextToken.getTokenType() == TokenType::INTEGER) {
					this->designExtractor.insertConstant(nextToken.getValue());
				}
				currentResult = EXPRESSION;
			}
		}

		if (receiveTerm) {
			error = error.combineWith(parseConditionOrExpression(level, stream, result, currentResult, userStatement));
			return error;
		}
	}

	auto nextToken = stream.lookAheadSingle();

	int currentRank = static_cast<int>(nextToken.getTokenType());
	if (currentRank <= 0 || currentRank >= 6) {
		/// wrong token type, stop parsing, return result
		result = currentResult;
		return ParseError();
	}

	if (currentRank > level) { 
		/// higher precedence, proceed to parse
		ParseResult subResult = FAILED;
		auto error = consumeToken(nextToken.getTokenType(), stream, nextToken);
		error = error.combineWith(parseConditionOrExpression(currentRank, stream, subResult, NULL_RESULT, userStatement));
		currentResult = combineResult(currentResult, subResult, nextToken);

		if (currentResult == FAILED) {
			return ParseError(nextToken, "Unable to parse");
		}
		error = error.combineWith(parseConditionOrExpression(level, stream, result, currentResult, userStatement));
		return error;
	}
	else {
		result = currentResult;
		return error;
	}
}