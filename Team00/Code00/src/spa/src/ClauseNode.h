#pragma once

#include "OptionalClause.h"
#include <memory>

class ClauseNode {
private:
	shared_ptr<OptionalClause> nodeClause;
	shared_ptr<ClauseNode> nextNode;

public:
	ClauseNode(shared_ptr<OptionalClause> clause);
	void setNextNode(shared_ptr<ClauseNode> node);
	shared_ptr<ClauseNode> getNextNode();
	shared_ptr<OptionalClause> getClause();
};