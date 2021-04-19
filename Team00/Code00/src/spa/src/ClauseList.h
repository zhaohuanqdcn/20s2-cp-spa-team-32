#pragma once

#include <memory>
#include <vector>
#include "ClauseNode.h"

class ClauseList {
private:
	shared_ptr<ClauseNode> head;
	shared_ptr<ClauseNode> tail;

public:
	ClauseList();
	void appendNode(shared_ptr<ClauseNode> node);
	void clear();
	shared_ptr<ClauseNode> getHead();
	vector<shared_ptr<OptionalClause>> getClauses();
};