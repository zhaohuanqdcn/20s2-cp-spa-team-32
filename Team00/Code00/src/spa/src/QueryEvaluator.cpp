#include "QueryEvaluator.h"

QueryEvaluator::QueryEvaluator(shared_ptr<QueryInterface> query, shared_ptr<PKBInterface> pkb) {
	this->aQuery = query;
	this->aPKB = pkb;
}

shared_ptr<ResultsTable> QueryEvaluator::evaluate() {
	vector<shared_ptr<OptionalClause>> clauses = aQuery->getOptionalClauses();

	// return this if any of the clauses has empty results
	shared_ptr<ResultsTable> noResults = make_shared<ResultsTable>(); 
	noResults->setIsNoResult();

	// evaluate the results for each clause first, results from PKB will be stored in each clause object
	for (vector<shared_ptr<OptionalClause>>::iterator iterator = clauses.begin(); iterator != clauses.end(); iterator++) {
		shared_ptr<OptionalClause> clause = *iterator;
		ClauseType clauseType = clause->getClauseType();

		switch (clauseType) {
		case ClauseType::RELATIONSHIP: 
			if (!evaluateRelationshipClause(clause)) {
				return noResults;
			}
			break;
		
		case ClauseType::PATTERN:
			if (!evaluatePatternClause(clause)) {
				return noResults;
			}
			break;
	
		case ClauseType::WITH:
			if (!evaluateWithClause(clause)) {
				return noResults;
			}
			break;

		default:
			break;
		}
	}

	// Optimization steps:
	// sort clauses by non-decreasing result size, and then 
	// seperate them into groups in which a clause has at least one synonym that has been in a previous clause
	clauses = QueryOptimizer::sortClausesByResultSize(clauses);
	vector<vector<shared_ptr<OptionalClause>>> clauseGroups = QueryOptimizer::sortClausesIntoGroups(clauses);

	
	// Merge the results of the clauses within each group
	vector<shared_ptr<ResultsTable>> groupResults = {};
	for (vector<vector<shared_ptr<OptionalClause>>>::iterator it = clauseGroups.begin(); it != clauseGroups.end(); it++) {
		vector<shared_ptr<OptionalClause>> clauseGroup = *it;
		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
		resultsTable = mergeClauses(clauseGroup, resultsTable);

		if (resultsTable->isNoResult()) {
			return noResults;
		}

		groupResults.push_back(resultsTable);
	}

	// Optimization step - sort groups result table by non-decreasing size before merging them
	groupResults = QueryOptimizer::sortTablesBySize(groupResults);

	// Merge the results of all clause group to get final results
	shared_ptr<ResultsTable> currentResults = make_shared<ResultsTable>();
	for (vector<shared_ptr<ResultsTable>>::iterator it = groupResults.begin(); it != groupResults.end(); it++) {
		shared_ptr<ResultsTable> groupResult = *it;
		currentResults = mergeResultTables(groupResult, currentResults);

		if (currentResults->isNoResult()) {
			return noResults;
		}
	}

	return currentResults;
}

bool QueryEvaluator::evaluateRelationshipClause(shared_ptr<OptionalClause> clause) {
	shared_ptr<RelationshipClause> relationshipClause = dynamic_pointer_cast<RelationshipClause>(clause);

	shared_ptr<QueryInput> leftQueryInput = relationshipClause->getLeftInput();
	shared_ptr<QueryInput> rightQueryInput = relationshipClause->getRightInput();
	RelationshipType relationshipType = relationshipClause->getRelationshipType();

	// None of query inputs are declarations
	if (leftQueryInput->getQueryInputType() != QueryInputType::DECLARATION &&
		rightQueryInput->getQueryInputType() != QueryInputType::DECLARATION) {

		bool hasResults = aPKB->getBooleanResultOfRS(relationshipType, leftQueryInput, rightQueryInput);
		clause->setBoolResult(hasResults);
		return hasResults;
	}

	// Both query inputs are declarations
	if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION &&
		rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {

		unordered_map<string, unordered_set<string>> PKBResults = aPKB->getMapResultsOfRS(relationshipType, leftQueryInput, rightQueryInput);
		clause->addMapResult(PKBResults);
		return (PKBResults.size() != 0);
	}

	// only one query input can be declaration
	unordered_set<string> PKBResults = aPKB->getSetResultsOfRS(relationshipClause->getRelationshipType(),
		leftQueryInput, rightQueryInput);
	clause->addSetResult(PKBResults);
	return (PKBResults.size() != 0);
}

bool QueryEvaluator::evaluatePatternClause(shared_ptr<OptionalClause> clause) {
	shared_ptr<PatternClause> patternClause = dynamic_pointer_cast<PatternClause>(clause);

	shared_ptr<Declaration> synonym = patternClause->getSynonym(); // assign/if/while
	shared_ptr<QueryInput> queryInput = patternClause->getQueryInput(); // LHS parameter of pattern clause

	// 2 declarations in pattern clause (including the assign/while/if declaration)
	if (queryInput->getQueryInputType() == QueryInputType::DECLARATION) { 

		unordered_map<string, unordered_set<string>> PKBResults;

		switch (synonym->getEntityType()) {
		case EntityType::ASSIGN:
			PKBResults = aPKB->getMapResultsOfAssignPattern(queryInput, *(patternClause->getExpression()));
			break;

		case EntityType::WHILE:
		case EntityType::IF:
			PKBResults = aPKB->getMapResultsOfContainerPattern(synonym->getEntityType(), queryInput);
			break;

		default:
			break;
		}

		clause->addMapResult(PKBResults);
		return (PKBResults.size() != 0);
	}
	else { // else first argument of clause is not a declaration: clause has only 1 declaration (assign/while/if)
		unordered_set<string> PKBResults;
		switch (synonym->getEntityType()) {
		case EntityType::ASSIGN:
			PKBResults = aPKB->getSetResultsOfAssignPattern(queryInput, *(patternClause->getExpression()));
			break;

		case EntityType::WHILE:
		case EntityType::IF:
			PKBResults = aPKB->getSetResultsOfContainerPattern(synonym->getEntityType(), queryInput);
			break;

		default:
			break;
		}

		clause->addSetResult(PKBResults);
		return (PKBResults.size() != 0);
	}
}

bool QueryEvaluator::evaluateWithClause(shared_ptr<OptionalClause> clause) {
	shared_ptr<OptionalClause> withClause = dynamic_pointer_cast<WithClause>(clause);

	// Assumed that at least one query input is a declaration/attribute (meaningless to have no synonym in input)
	shared_ptr<QueryInput> leftQueryInput = withClause->getLeftInput();
	shared_ptr<QueryInput> rightQueryInput = withClause->getRightInput();

	// Both query inputs are either a declaration or an attribute of a declaration
	if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION &&
		rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {

		shared_ptr<Declaration> leftDeclaration = dynamic_pointer_cast<Declaration>(leftQueryInput);
		shared_ptr<Declaration> rightDeclaration = dynamic_pointer_cast<Declaration>(rightQueryInput);

		return evaluateTwoDeclarationsWithClause(leftDeclaration, rightDeclaration, clause);
	}

	// One declaration/attribute and one stmtNum/ident
	shared_ptr<QueryInput> queryInput;
	shared_ptr<Declaration> declaration;

	if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
		declaration = dynamic_pointer_cast<Declaration>(leftQueryInput);
		queryInput = rightQueryInput;
	}
	else {
		declaration = dynamic_pointer_cast<Declaration>(rightQueryInput);
		queryInput = leftQueryInput;
	}

	return evaluateOneDeclarationWithClause(declaration, queryInput, clause);

}

bool QueryEvaluator::evaluateTwoDeclarationsWithClause(shared_ptr<Declaration> leftDeclaration, shared_ptr<Declaration> rightDeclaration, 
	shared_ptr<OptionalClause> clause) {

	// Both are not attributes, so both are simply declarations
	if (!leftDeclaration->getIsAttribute() && !rightDeclaration->getIsAttribute()) {
		EntityType leftEntityType = leftDeclaration->getEntityType();
		EntityType rightEntityType = rightDeclaration->getEntityType();

		if ((leftEntityType == EntityType::PROC && rightEntityType == EntityType::VAR) ||
			(leftEntityType == EntityType::VAR && rightEntityType == EntityType::PROC) ||
			(leftEntityType == EntityType::CONST && rightEntityType != EntityType::CONST) || 
			(leftEntityType != EntityType::CONST && rightEntityType == EntityType::CONST)) {

			unordered_map<string, unordered_set<string>> PKBResults = aPKB->getDeclarationsMatchResults(leftDeclaration, rightDeclaration);
			clause->addMapResult(PKBResults);
			return (PKBResults.size() != 0);
		}
		else { // else, both declarations are of the same type: v = v, p = p, s/n = a/w/c/pn/rd/s/n

			EntityType restrictiveEntityType;
			if (leftEntityType == EntityType::STMT && rightEntityType != EntityType::STMT) {
				restrictiveEntityType = rightEntityType;
			}
			else {
				restrictiveEntityType = leftEntityType;
			}

			unordered_set<string> PKBResults = aPKB->getEntities(restrictiveEntityType);
			clause->addSetResult(PKBResults);
			return (PKBResults.size() != 0);
		}
	}

	// Both are attributes
	if (leftDeclaration->getIsAttribute() && rightDeclaration->getIsAttribute()) {
		unordered_map<string, unordered_set<string>> PKBResults = aPKB->getAttributesMatchResults(leftDeclaration->getEntityType(), rightDeclaration->getEntityType());
		clause->addMapResult(PKBResults);
		return (PKBResults.size() != 0);
	}

	// One attribute and one declaration
	shared_ptr<Declaration> attribute;
	shared_ptr<Declaration> declaration;
	if (leftDeclaration->getIsAttribute()) {
		attribute = leftDeclaration;
		declaration = rightDeclaration;
	}
	else {
		attribute = rightDeclaration;
		declaration = leftDeclaration;
	}

	unordered_map<string, unordered_set<string>> PKBResults = aPKB->getDeclarationMatchAttributeResults(declaration, attribute->getEntityType());
	clause->addMapResult(PKBResults);
	return (PKBResults.size() != 0);
}

bool QueryEvaluator::evaluateOneDeclarationWithClause(shared_ptr<Declaration> declaration, shared_ptr<QueryInput> queryInput,
	shared_ptr<OptionalClause> clause) {

	// One attribute and one ident
	if (declaration->getIsAttribute()) {
		unordered_set<string> PKBResults = aPKB->getAttributeMatchNameResults(declaration->getEntityType(), dynamic_pointer_cast<Ident>(queryInput));
		clause->addSetResult(PKBResults);
		return (PKBResults.size() != 0);
	}
	else { // One declaration and one stmtNum/Ident
		unordered_set<string> PKBResults = aPKB->getEntities(declaration->getEntityType());
		string expectedValue = queryInput->getValue();
		if (PKBResults.find(expectedValue) == PKBResults.end()) {
			return false;
		}

		clause->addSetResult({ expectedValue });
		return true;
	}
}

shared_ptr<ResultsTable> QueryEvaluator::mergeClauses(vector<shared_ptr<OptionalClause>> clauses, shared_ptr<ResultsTable> resultsTable) {
	shared_ptr<ResultsTable> currentResults = resultsTable;

	for (vector<shared_ptr<OptionalClause>>::iterator iterator = clauses.begin(); iterator != clauses.end(); iterator++) {
		shared_ptr<OptionalClause> clause = *iterator;
		ClauseType clauseType = clause->getClauseType();

		switch (clauseType) {
		case ClauseType::RELATIONSHIP:
			currentResults = mergeRelationshipClause(dynamic_pointer_cast<RelationshipClause>(clause), currentResults);
			break;

		case ClauseType::PATTERN:
			currentResults = mergePatternClause(dynamic_pointer_cast<PatternClause>(clause), currentResults);
			break;

		case ClauseType::WITH:
			currentResults = mergeWithClause(dynamic_pointer_cast<WithClause>(clause), currentResults);
			break;

		default:
			break;
		}

		// if the clause does not have any results, no need to continue evaluating
		if (currentResults->isNoResult()) {
			return currentResults;
		}
	}

	return currentResults;
}

shared_ptr<ResultsTable> QueryEvaluator::mergeRelationshipClause(shared_ptr<RelationshipClause> relationshipClause, shared_ptr<ResultsTable> results) {
	shared_ptr<ResultsTable> currentResults = results;

	shared_ptr<QueryInput> leftQueryInput = relationshipClause->getLeftInput();
	shared_ptr<QueryInput> rightQueryInput = relationshipClause->getRightInput();
	ClauseResultType clauseResultType = relationshipClause->getClauseResultType();

	switch (clauseResultType) {
	case (ClauseResultType::MAP): {
		string leftSynonym = leftQueryInput->getValue();
		string rightSynonym = rightQueryInput->getValue();
		unordered_map<string, unordered_set<string>> clauseResults = relationshipClause->getMapResult();
		currentResults = mergeMapResults(clauseResults, { leftSynonym, rightSynonym }, currentResults);

		// intermediate results table may become empty after merging
		if (currentResults->isTableEmpty()) {
			currentResults->setIsNoResult();
		}

		return currentResults;
	}
		
	case (ClauseResultType::SET): {
		string synonym;
		if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
			synonym = leftQueryInput->getValue();
		}
		else {
			synonym = rightQueryInput->getValue();
		}
		unordered_set<string> clauseResults = relationshipClause->getSetResult();

		currentResults = mergeSetResults(clauseResults, { synonym }, currentResults);

		// intermediate results table may become empty after merging
		if (currentResults->isTableEmpty()) {
			currentResults->setIsNoResult();
		}

		return currentResults;
	}

	case (ClauseResultType::BOOL): {
		if (!relationshipClause->getBoolResult()) {
			currentResults->setIsNoResult();
		}
		return currentResults;
	}

	default:
		return currentResults;
	}
}

shared_ptr<ResultsTable> QueryEvaluator::mergePatternClause(shared_ptr<PatternClause> patternClause, shared_ptr<ResultsTable> results) {
	shared_ptr<ResultsTable> currentResults = results;

	shared_ptr<Declaration> synonym = patternClause->getSynonym(); // assign/if/while
	shared_ptr<QueryInput> queryInput = patternClause->getQueryInput(); // LHS parameter of pattern clause
	ClauseResultType clauseResultType = patternClause->getClauseResultType();

	switch (clauseResultType) {
	case (ClauseResultType::MAP): {
		unordered_map<string, unordered_set<string>> clauseResults = patternClause->getMapResult();
		currentResults = mergeMapResults(clauseResults, { synonym->getValue(), queryInput->getValue() }, currentResults);

		// intermediate results table may become empty after merging
		if (currentResults->isTableEmpty()) {
			currentResults->setIsNoResult();
		}

		return currentResults;
	}

	case (ClauseResultType::SET): {
		unordered_set<string> clauseResults = patternClause->getSetResult();

		currentResults = mergeSetResults(clauseResults, { synonym->getValue() }, currentResults);

		// intermediate results table may become empty after merging
		if (currentResults->isTableEmpty()) {
			currentResults->setIsNoResult();
		}

		return currentResults;
	}

	default:
		return currentResults;
	}
}

shared_ptr<ResultsTable> QueryEvaluator::mergeWithClause(shared_ptr<WithClause> withClause, shared_ptr<ResultsTable> results) {
	shared_ptr<ResultsTable> currentResults = results;

	// Assumed that at least one query input is a declaration/attribute (meaningless to have no synonym in input)
	shared_ptr<QueryInput> leftQueryInput = withClause->getLeftInput();
	shared_ptr<QueryInput> rightQueryInput = withClause->getRightInput();

	// Both query inputs are either a declaration or an attribute of a declaration
	if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION &&
		rightQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {

		currentResults = mergeTwoDeclarationsWithClause(withClause, currentResults);
		return currentResults;
	}

	// One declaration/attribute and one stmtNum/ident
	currentResults = mergeOneDeclarationWithClause(withClause, currentResults);
	return currentResults;
}

shared_ptr<ResultsTable> QueryEvaluator::mergeTwoDeclarationsWithClause(shared_ptr<WithClause> withClause, shared_ptr<ResultsTable> results) {
	shared_ptr<ResultsTable> currentResults = results;
	
	shared_ptr<Declaration> leftDeclaration = dynamic_pointer_cast<Declaration>(withClause->getLeftInput());
	shared_ptr<Declaration> rightDeclaration = dynamic_pointer_cast<Declaration>(withClause->getRightInput());
	ClauseResultType clauseResultType = withClause->getClauseResultType();

	// Both are not attributes, so both are simply declarations
	if (!leftDeclaration->getIsAttribute() && !rightDeclaration->getIsAttribute()) {
		string leftSynonym = leftDeclaration->getValue();
		string rightSynonym = rightDeclaration->getValue();

		switch (clauseResultType) {
		case (ClauseResultType::MAP): {
			unordered_map<string, unordered_set<string>> clauseResults = withClause->getMapResult();
			currentResults = mergeMapResults(clauseResults, { leftSynonym, rightSynonym }, currentResults);

			// intermediate results table may become empty after merging
			if (currentResults->isTableEmpty()) {
				currentResults->setIsNoResult();
			}

			return currentResults;
		}

		case (ClauseResultType::SET): {
			unordered_set<string> clauseResults = withClause->getSetResult();
			currentResults = mergeSetResults(clauseResults, { leftSynonym, rightSynonym }, currentResults);

			// intermediate results table may become empty after merging
			if (currentResults->isTableEmpty()) {
				currentResults->setIsNoResult();
			}

			return currentResults;
		}

		default:
			return currentResults;
		}
	}

	// From here on, result type must be a map (refer to evaluateTwoDeclarationsWithClause function above)
	unordered_map<string, unordered_set<string>> clauseResults = withClause->getMapResult();

	// Both are attributes
	if (leftDeclaration->getIsAttribute() && rightDeclaration->getIsAttribute()) {
		string leftSynonym = leftDeclaration->getValue();
		string rightSynonym = rightDeclaration->getValue();
		currentResults = mergeMapResults(clauseResults, { leftSynonym, rightSynonym }, currentResults);

		// intermediate results table may become empty after merging
		if (currentResults->isTableEmpty()) {
			currentResults->setIsNoResult();
		}

		return currentResults;
	}

	// One attribute and one declaration
	shared_ptr<Declaration> attribute;
	shared_ptr<Declaration> declaration;
	if (leftDeclaration->getIsAttribute()) {
		attribute = leftDeclaration;
		declaration = rightDeclaration;
	}
	else {
		attribute = rightDeclaration;
		declaration = leftDeclaration;
	}

	string leftSynonym = declaration->getValue();
	string rightSynonym = attribute->getValue();
	currentResults = mergeMapResults(clauseResults, { leftSynonym, rightSynonym }, currentResults);

	// intermediate results table may become empty after merging
	if (currentResults->isTableEmpty()) {
		currentResults->setIsNoResult();
	}

	return currentResults;
}

shared_ptr<ResultsTable> QueryEvaluator::mergeOneDeclarationWithClause(shared_ptr<WithClause> withClause, shared_ptr<ResultsTable> results) {
	shared_ptr<ResultsTable> currentResults = results;
	shared_ptr<QueryInput> leftQueryInput = withClause->getLeftInput();
	shared_ptr<QueryInput> rightQueryInput = withClause->getRightInput();

	// One declaration/attribute and one stmtNum/ident
	shared_ptr<QueryInput> queryInput;
	shared_ptr<Declaration> declaration;

	if (leftQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
		declaration = dynamic_pointer_cast<Declaration>(leftQueryInput);
		queryInput = rightQueryInput;
	}
	else {
		declaration = dynamic_pointer_cast<Declaration>(rightQueryInput);
		queryInput = leftQueryInput;
	}

	
	// In this function, the result type must be a set (refer to evaluateOneDeclarationWithClause function above)
	unordered_set<string> clauseresults = withClause->getSetResult();
	currentResults = mergeSetResults(clauseresults, { declaration->getValue() }, currentResults);

	// intermediate results table may become empty after merging
	if (currentResults->isTableEmpty()) {
		currentResults->setIsNoResult();
	}

	return currentResults;
}



// PKBResult is assumed to be non empty here - QE must check if results from PKB are empty before merging
// only case when currentResult is empty is when mergining the first PKBResult
shared_ptr<ResultsTable> QueryEvaluator::mergeMapResults(unordered_map <string, unordered_set<string>> PKBResults, vector<string> synonyms, shared_ptr<ResultsTable> currentResults) {
	if (currentResults->isTableEmpty()) {
		currentResults->populateWithMap(PKBResults, synonyms);
		return currentResults;
	}

	unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(synonyms, currentResults->getSynonyms());
	if (commonSynonyms.size() == 0) {
		return ResultUtil::getCartesianProductFromMap(PKBResults, synonyms, currentResults);
	}
	else {
		return ResultUtil::getNaturalJoinFromMap(PKBResults, synonyms, currentResults, commonSynonyms);
	}
}

shared_ptr<ResultsTable> QueryEvaluator::mergeSetResults(unordered_set<string> PKBResults, vector<string> synonyms,
	shared_ptr<ResultsTable> currentResults) {
	if (currentResults->isTableEmpty()) {
		currentResults->populateWithSet(PKBResults, synonyms);
		return currentResults;
	}

	unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(synonyms, currentResults->getSynonyms());
	if (commonSynonyms.size() == 0) {
		return ResultUtil::getCartesianProductFromSet(PKBResults, synonyms, currentResults);
	}
	else {
		return ResultUtil::getNaturalJoinFromSet(PKBResults, synonyms, currentResults, commonSynonyms);
	}
}

shared_ptr<ResultsTable> QueryEvaluator::mergeResultTables(shared_ptr<ResultsTable> groupResult, shared_ptr<ResultsTable> currentResults) {
	if (currentResults->isTableEmpty()) {
		return groupResult;
	}

	unordered_set<string> commonSynonyms = ResultUtil::getCommonSynonyms(groupResult->getSynonyms(), currentResults->getSynonyms());
	if (commonSynonyms.size() == 0) {
		return ResultUtil::getCartesianProductOfTables(groupResult, currentResults);
	}
	else {
		return ResultUtil::getNaturalJoinOfTables(groupResult, currentResults, commonSynonyms);
	}
}