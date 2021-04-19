#include "QueryInput.h"

using namespace std;

QueryInputType QueryInput::getQueryInputType() {
	return this->aQueryInputType;
}

string QueryInput::getValue() {
	return this->aValue;
}

QueryInput::~QueryInput() {}