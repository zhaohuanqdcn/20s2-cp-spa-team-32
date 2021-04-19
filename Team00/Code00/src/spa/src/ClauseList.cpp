#include "ClauseList.h"

ClauseList::ClauseList() {
	this->head = NULL;
	this->tail = NULL;
}

void ClauseList::appendNode(shared_ptr<ClauseNode> node) {

	// list is empty
	if (this->head == NULL && this->tail == NULL) {
		this->head = node;
		this->tail = node;
	}
	else { // else list has at least one node
		this->tail->setNextNode(node);

		shared_ptr<ClauseNode> current = this->tail;
		while (current->getNextNode() != NULL) {
			current = current->getNextNode();
		}
		this->tail = current;
	}
}

void ClauseList::clear() {
	this->head = NULL;
	this->tail = NULL;
}

shared_ptr<ClauseNode> ClauseList::getHead() {
	return this->head;
}

vector<shared_ptr<OptionalClause>> ClauseList::getClauses() {
	vector<shared_ptr<OptionalClause>> clauses;
	if (this->head != NULL) {
		shared_ptr<ClauseNode> current = this->head;
		while (current != NULL) {
			clauses.push_back(current->getClause());
			current = current->getNextNode();
		}
	}
	return clauses;
}