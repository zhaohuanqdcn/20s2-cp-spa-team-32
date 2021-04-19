#include "DisjointClausesSet.h"

DisjointClausesSet::DisjointClausesSet(vector<shared_ptr<OptionalClause>> clauses) {
	this->allClauses = clauses;
	this->parent = unordered_map<string, string>();
	this->disjointClauses = unordered_map<string, shared_ptr<ClauseList>>();
	this->noDeclarationClauses = vector<shared_ptr<OptionalClause>>();
	unordered_set<string> synonyms = this->extractSynonyms();
	this->init(synonyms);
}

// get all unique synonyms in clauses
unordered_set<string> DisjointClausesSet::extractSynonyms() {
	unordered_set<string> synonyms;
	for (vector<shared_ptr<OptionalClause>>::iterator it = this->allClauses.begin(); it != this->allClauses.end(); it++) {
		shared_ptr<QueryInput> leftQueryInput = (*it)->getLeftInput();
		shared_ptr<QueryInput> rightQueryInput = (*it)->getRightInput();

		if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
			synonyms.insert(leftQueryInput->getValue());
		}

		if (rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
			synonyms.insert(rightQueryInput->getValue());
		}
	}

	return synonyms;
}

// initialize data structures with unique synonyms
void DisjointClausesSet::init(unordered_set<string> synonyms) {
	for (unordered_set<string>::iterator it = synonyms.begin(); it != synonyms.end(); it++) {
		string synonym = *it;
		this->parent.insert({ synonym, synonym }); // initially each synonym is its own disjoint set
		this->disjointClauses.insert({ synonym, make_shared<ClauseList>() });
	}
}

void DisjointClausesSet::addClause(shared_ptr<OptionalClause> clause) {
	shared_ptr<QueryInput> leftQueryInput = clause->getLeftInput();
	shared_ptr<QueryInput> rightQueryInput = clause->getRightInput();
	
	if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION &&
		rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
		string leftSynonym = leftQueryInput->getValue();
		string rightSynonym = rightQueryInput->getValue();

		string leftSynonymRoot = this->findRoot(leftSynonym);
		string rightSynonymRoot = this->findRoot(rightSynonym);

		// add clause to left synonym root set first
		shared_ptr<ClauseList> leftSynonymClauses = this->disjointClauses.find(leftSynonymRoot)->second;
		leftSynonymClauses->appendNode(make_shared<ClauseNode>(clause));

		// now consider to join right synonym root set to left synonym root set
		shared_ptr<ClauseList> rightSynonymClauses = this->disjointClauses.find(rightSynonymRoot)->second;
		shared_ptr<ClauseNode> rightSynonymRootHead = rightSynonymClauses->getHead();
		if ((leftSynonymRoot == rightSynonymRoot) || 
			(rightSynonymRootHead != NULL && rightSynonymRootHead->getClause()->getLeftInput()->getValue() != leftSynonym &&
				rightSynonymRootHead->getClause()->getLeftInput()->getValue() != rightSynonym &&
				rightSynonymRootHead->getClause()->getRightInput()->getValue() != leftSynonym &&
				rightSynonymRootHead->getClause()->getRightInput()->getValue() != rightSynonym)) {
			// if the root of left and right synonyms are the same OR
			// if any of the left and right synonyms do not match any of the left and right synonyms of the 
			// first clause of the right synonym root's node list,
			// then dont need to join right synonym root set to left synonym root set
			return;
		}

		// Join right synonym set to left synonym set
		this->parent.find(rightSynonymRoot)->second = leftSynonymRoot;
		leftSynonymClauses->appendNode(rightSynonymClauses->getHead()); // join the sets
		rightSynonymClauses->clear();
		return;
	}

	// Clause has no declaraton - would not being to any other clause set
	if (leftQueryInput->getQueryInputType() != QueryInputType::DECLARATION &&
		rightQueryInput->getQueryInputType() != QueryInputType::DECLARATION) {
		this->noDeclarationClauses.push_back(clause);
		return;
	}

	// only one queryInput is a declaration
	shared_ptr<QueryInput> declarationQueryInput;
	if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
		declarationQueryInput = leftQueryInput;
	} else {
		declarationQueryInput = rightQueryInput;
	}
	string synonym = declarationQueryInput->getValue();
	string synonymRoot = this->findRoot(synonym);

	this->disjointClauses.find(synonymRoot)->second->appendNode(make_shared<ClauseNode>(clause));
}


vector<vector<shared_ptr<OptionalClause>>> DisjointClausesSet::getClauses() {
	vector<vector<shared_ptr<OptionalClause>>> results;

	for (vector<shared_ptr<OptionalClause>>::iterator it = this->allClauses.begin(); it != this->allClauses.end(); it++) {
		this->addClause(*it);
	}

	/*if (this->noDeclarationClauses.size() > 0) {
		results.push_back(this->noDeclarationClauses);
	}*/

	for (unordered_map<string, shared_ptr<ClauseList>>::iterator it = this->disjointClauses.begin();
		it != this->disjointClauses.end(); it++) {
		shared_ptr<ClauseList> disjointSet = it->second;
		vector<shared_ptr<OptionalClause>> clauses = disjointSet->getClauses();
		if (clauses.size() > 0) {
			results.push_back(clauses);
		}
	}

	return results;
}

string DisjointClausesSet::findRoot(string synonym) {
	string parentSynonym = this->parent.find(synonym)->second;
	if (parentSynonym == synonym) {
		return synonym;
	}

	return this->findRoot(parentSynonym);
}