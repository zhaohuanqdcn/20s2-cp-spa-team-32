#ifndef __EXPRESSIONPARSER__H__
#define __EXPRESSIONPARSER__H__

#include <vector>

using namespace std;

ParseError parseExpression(string input, Expression& result) {
    SIMPLETokenStream stream{{input}};
    return parseExpression(stream, result);
}


ParseError parseFactor(SIMPLETokenStream &stream, Expression& result, int userStatement) {
	if (stream.isEmpty()) {
		return ParseError(SIMPLEToken(), "unexpected end of file");
	}
	SIMPLEToken nextToken = stream.getToken();

	if (nextToken.getTokenType() == TokenType::name) {
		result = Expression(nextToken.getValue());
		return ParseError();
	}

	if (nextToken.getTokenType() == TokenType::integer) {
		result = Expression(nextToken.getValue());
		return ParseError();
	}

	if (nextToken.getValue() == "(") {
		auto error = parseExpression(stream, result, userStatement);
		error = error.combineWith(consumeTerminal(")", stream));	
		return error;
	}
	return ParseError(nextToken, "unexpected token");
}

ParseError parseExpression(SIMPLETokenStream& stream, Expression &result) {
    auto error = parseTerm(stream, result);
	while (!stream.isEmpty()) {
		SIMPLEToken nextToken = stream.lookAheadSingle();
		if (nextToken.getTokenType() == TokenType::exprSymbol) {
			Expression subResult("");
			error = error.combineWith(consumeToken(TokenType::exprSymbol, stream, nextToken));
			error = error.combineWith(parseTerm(stream, subResult, userStatement));
			result = result.combineExpression(nextToken.getValue(), subResult);
		} else {
			break;
		}
	}
	return error;
}


ParseError parseTerm(SIMPLETokenStream &stream, Expression& result) {
	auto error = parseFactor(stream, result);

	while (!stream.isEmpty()) {
		SIMPLEToken nextToken = stream.lookAheadSingle();
		if (nextToken.getTokenType() == TokenType::termSymbol) {
			Expression subResult("");
			error = error.combineWith(consumeToken(TokenType::termSymbol, stream, nextToken));
			error = error.combineWith(parseFactor(stream, subResult, userStatement));
			result = result.combineExpression(nextToken.getValue(), subResult);
			//TODO
		} else {
			return error;
		}
	}

	return error;
}

#endif  //!__EXPRESSIONPARSER__H__