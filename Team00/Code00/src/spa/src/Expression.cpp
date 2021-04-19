#include "Expression.h"
#include <iostream>

using namespace std;

Expression::Expression(string value, ExpressionType t) : aValue(value), type(t) {}

Expression::Expression(string value) : aValue(value) {
	if (value == "") {
		type = ExpressionType::PARTIAL;
	} else {
		type = ExpressionType::EXACT;
	}
} // remove after merge with QPP

Expression Expression::combineExpression(
	string operand, const Expression& other, ExpressionType t) {
	//cout << "combining " <<  "(" + this->getValue() + operand + other.getValue() + ")" << endl;
	ExpressionType resultType;
	if (this->type == ExpressionType::EXACT || t == ExpressionType::EXACT) {
		resultType = ExpressionType::EXACT; 
	} else {
		resultType = ExpressionType::PARTIAL;
	}

	return Expression("(" + this->getValue() + operand + other.getValue() + ")", resultType);
}

void Expression::setType(ExpressionType type) {
	this->type = type;
}

string Expression::getValue() const {
	return this->aValue;
}

ExpressionType Expression::getType() const {
	return this->type;
}

