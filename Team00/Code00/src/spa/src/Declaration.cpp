#include "Declaration.h"

using namespace std;

Declaration::Declaration(EntityType entityType, string value) {
	this->aEntityType = entityType;
	this->aValue = value;
	this->isAttribute = false;
	this->aQueryInputType = QueryInputType::DECLARATION;
}

EntityType Declaration::getEntityType() {
	return this->aEntityType;
}

void Declaration::setIsAttribute() {
	this->isAttribute = true;
}

bool Declaration::getIsAttribute() {
	return this->isAttribute;
}