#include "Ident.h"

using namespace std;

Ident::Ident(string value) {
	this->aQueryInputType = QueryInputType::IDENT;
	this->aValue = value;
}