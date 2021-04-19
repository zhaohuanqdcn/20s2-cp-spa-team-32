#include "ClauseNode.h"

ClauseNode::ClauseNode(shared_ptr<OptionalClause> clause) {
	this->nodeClause = clause;
	this->nextNode = NULL;
}
void ClauseNode::setNextNode(shared_ptr<ClauseNode> node) {
	this->nextNode = node;
}

shared_ptr<ClauseNode> ClauseNode::getNextNode() {
	return this->nextNode;
}


shared_ptr<OptionalClause> ClauseNode::getClause() {
	return this->nodeClause;
}