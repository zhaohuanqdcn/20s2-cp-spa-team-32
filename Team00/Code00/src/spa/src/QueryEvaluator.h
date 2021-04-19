#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include "QueryInterface.h"
#include "QueryInputType.h"
#include "PKBInterface.h"
#include "ResultUtil.h"
#include "ResultsTable.h"
#include "QueryOptimizer.h"

class QueryEvaluator {
private:
	shared_ptr<QueryInterface> aQuery;
	shared_ptr<PKBInterface> aPKB;

	bool evaluateRelationshipClause(shared_ptr<OptionalClause> clause);

	bool evaluatePatternClause(shared_ptr<OptionalClause> clause);

	bool evaluateWithClause(shared_ptr<OptionalClause> clause);

	bool evaluateTwoDeclarationsWithClause(shared_ptr<Declaration> leftDeclaration, shared_ptr<Declaration> rightDeclaration,
		shared_ptr<OptionalClause> clause);

	bool evaluateOneDeclarationWithClause(shared_ptr<Declaration> declaration, shared_ptr<QueryInput> queryInput,
		shared_ptr<OptionalClause> clause);

	shared_ptr<ResultsTable> mergeClauses(vector<shared_ptr<OptionalClause>> clauses, shared_ptr<ResultsTable> resultsTable);

	shared_ptr<ResultsTable> mergeRelationshipClause(shared_ptr<RelationshipClause> relationshipClause, shared_ptr<ResultsTable> results);

	shared_ptr<ResultsTable> mergePatternClause(shared_ptr<PatternClause> patternClause, shared_ptr<ResultsTable> results);

	shared_ptr<ResultsTable> mergeWithClause(shared_ptr<WithClause> withClause, shared_ptr<ResultsTable> results);
	
	shared_ptr<ResultsTable> mergeTwoDeclarationsWithClause(shared_ptr<WithClause> withClause, shared_ptr<ResultsTable> results);
	
	shared_ptr<ResultsTable> mergeOneDeclarationWithClause(shared_ptr<WithClause> withClause, shared_ptr<ResultsTable> results);

	shared_ptr<ResultsTable> mergeMapResults(unordered_map <string, unordered_set<string>> PKBResults, vector<string> synonyms,
		shared_ptr<ResultsTable> currentResults);

	shared_ptr<ResultsTable> mergeSetResults(unordered_set<string> PKBResults, vector<string> synonyms,
		shared_ptr<ResultsTable> currentResults);

	shared_ptr<ResultsTable> mergeResultTables(shared_ptr<ResultsTable> groupResult, shared_ptr<ResultsTable> currentResults);
	
public:
	
	QueryEvaluator(shared_ptr<QueryInterface> query, shared_ptr<PKBInterface> pkb);

	shared_ptr<ResultsTable> evaluate();
};