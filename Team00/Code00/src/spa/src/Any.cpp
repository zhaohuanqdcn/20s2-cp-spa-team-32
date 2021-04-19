#include "Any.h"

using namespace std;


Any::Any() {
	this->aValue = "_";
	this->aQueryInputType = QueryInputType::ANY;
}

Any::Any(string value) {
	this->aValue = value;
	this->aQueryInputType = QueryInputType::ANY;
}