#include "ResultsProjector.h"

string ResultsProjector::TRUE = "TRUE";
string ResultsProjector::FALSE = "FALSE";
string ResultsProjector::SPACE = " ";

// ================ General functions =================================================================================================================
void ResultsProjector::projectResults(shared_ptr<ResultsTable> evaluatedResults, shared_ptr<SelectClause> selectClause, shared_ptr<PKBInterface> PKB, list<string>& results) {
	vector<shared_ptr<Declaration>> declarations = selectClause->getDeclarations();

	// Select BOOLEAN
	if (declarations.size() == 0) {
		if (evaluatedResults->isNoResult()) {
			results.push_back(FALSE);
		}
		else {
			results.push_back(TRUE);
		}
		return;
	}

	// No result for select single synonym/tuple
	if (evaluatedResults->isNoResult()) {
		return;
	}

	// select single synonym
	if (declarations.size() == 1) {
		shared_ptr<Declaration> declaration = declarations.at(0);
		projectSingleSynonym(evaluatedResults, declaration, PKB, results);
	}
	else { // select tuple
		projectTuple(evaluatedResults, declarations, PKB, results);
	}
}

unordered_set<string> ResultsProjector::getAttributeValuesFromStmtNum(unordered_set<string> stmtNum, shared_ptr<PKBInterface> PKB) {
	unordered_set<string> values;

	for (unordered_set<string>::iterator it = stmtNum.begin(); it != stmtNum.end(); it++) {
		string stmtNum = *it;
		values.insert(PKB->getNameFromStmtNum(stmtNum));
	}

	return values;
}

// ================ Project single synonym functions =================================================================================================

void ResultsProjector::projectSingleSynonym(shared_ptr<ResultsTable> evaluatedResults, shared_ptr<Declaration> declaration,
	shared_ptr<PKBInterface> PKB, list<string>& results) {
	unordered_set<string> synonyms = evaluatedResults->getSynonyms();
	string synonym = declaration->getValue();
	unordered_set<string> setResults; // set used to remove duplicates

	// if selected synonym is in resultsTable
	if (synonyms.find(synonym) != synonyms.end()) {
		vector<vector<string>> resultValues = evaluatedResults->getTableValues();
		unordered_map<string, int> synonymIndexMap = evaluatedResults->getSynonymIndexMap();
		int synonymIndex = synonymIndexMap.find(synonym)->second;

		// Extract synonym values from table
		for (vector<vector<string>>::iterator it = resultValues.begin(); it != resultValues.end(); it++) {
			string resultValue = it->at(synonymIndex);
			setResults.insert(resultValue);
		}

		// If it is an attribute, need to get the procName/varName from its stmtnums
		if (declaration->getIsAttribute()) {
			setResults = getAttributeValuesFromStmtNum(setResults, PKB);
		}
	}
	else { // else, call results from pkb
		setResults = PKB->getEntities(declaration->getEntityType());

		// synonym has no results, return empty results list
		if (setResults.size() == 0) {
			return;
		}

		// If it is an attribute, need to get the procName/varName from its stmtnums
		if (declaration->getIsAttribute()) {
			setResults = getAttributeValuesFromStmtNum(setResults, PKB);
		}
	}

	for (unordered_set<string>::iterator it = setResults.begin(); it != setResults.end(); it++) {
		results.push_back(*it);
	}
}


// ================ Project tuple functions ==========================================================================================================

void ResultsProjector::projectTuple(shared_ptr<ResultsTable> evaluatedResults, vector<shared_ptr<Declaration>> declarations,
	shared_ptr<PKBInterface> PKB, list<string>& results) {
	unordered_set<string> resultsSynonyms = evaluatedResults->getSynonyms();
	vector<shared_ptr<Declaration>> selectedSynonymsInResult = getSelectedSynonymsInResults(declarations, resultsSynonyms);
	vector<shared_ptr<Declaration>> selectedSynonymsNotInResult = getSelectedSynonymsNotInResults(declarations, resultsSynonyms);

	// All tuple synonyms are in resultsTable
	if (selectedSynonymsNotInResult.size() == 0) {
		projectTupleFromResults(declarations, evaluatedResults, PKB, results);
		return;
	}

	// All tuple synonyms are not in resultsTable 
	if (selectedSynonymsInResult.size() == 0) {
		shared_ptr<ResultsTable> emptyResultsTable = make_shared<ResultsTable>();
		shared_ptr<ResultsTable> resultsTable = getResultsTableOfTuple(declarations, emptyResultsTable, PKB);

		if (resultsTable->isNoResult()) {
			return;
		}

		projectTupleFromResults(declarations, resultsTable, PKB, results);
		return;
	}

	// Tuple has mix of synonyms in resultsTable and not in resultsTable
	if (selectedSynonymsNotInResult.size() != 0 && selectedSynonymsInResult.size() != 0) {
		shared_ptr<ResultsTable> resultsTable = getResultsTableOfTuple(selectedSynonymsNotInResult, evaluatedResults, PKB);

		if (resultsTable->isNoResult()) {
			return;
		}

		projectTupleFromResults(declarations, resultsTable, PKB, results);
		return;
	}
}

vector<shared_ptr<Declaration>> ResultsProjector::getSelectedSynonymsInResults(vector<shared_ptr<Declaration>> selectedDeclarations, unordered_set<string> resultSynonyms) {
	vector<shared_ptr<Declaration>> declarations;
	for (vector<shared_ptr<Declaration>>::iterator it = selectedDeclarations.begin(); it != selectedDeclarations.end(); it++) {
		shared_ptr<Declaration> declaration = *it;
		string selectedSynonym = declaration->getValue();
		if (resultSynonyms.find(selectedSynonym) != resultSynonyms.end()) {
			declarations.push_back(declaration);
		}
	}
	return declarations;
}

vector<shared_ptr<Declaration>> ResultsProjector::getSelectedSynonymsNotInResults(vector<shared_ptr<Declaration>> selectedDeclarations, unordered_set<string> resultSynonyms) {
	vector<shared_ptr<Declaration>> declarations;
	for (vector<shared_ptr<Declaration>>::iterator it = selectedDeclarations.begin(); it != selectedDeclarations.end(); it++) {
		shared_ptr<Declaration> declaration = *it;
		string selectedSynonym = declaration->getValue();
		if (resultSynonyms.find(selectedSynonym) == resultSynonyms.end()) {
			declarations.push_back(declaration);
		}
	}
	return declarations;
}

// builds the final result list using the final evaluated results WIP
void ResultsProjector::projectTupleFromResults(vector<shared_ptr<Declaration>> declarations, shared_ptr<ResultsTable> evaluatedResults,
	shared_ptr<PKBInterface> PKB, list<string>& results) {
	vector<vector<string>> resultValues = evaluatedResults->getTableValues();
	unordered_map<string, int> synonymIndexMap = evaluatedResults->getSynonymIndexMap();
	unordered_set<string> setResults;

	for (vector<vector<string>>::iterator it = resultValues.begin(); it != resultValues.end(); it++) {
		vector<string> row = *it;
		string resultString;

		for (size_t i = 0; i < declarations.size(); i++) {
			shared_ptr<Declaration> declaration = declarations.at(i);
			string selectedSynonym = declaration->getValue();
			int synonymIndex = synonymIndexMap.find(selectedSynonym)->second;
			string synonymValue = row.at(synonymIndex);

			// If declaration is an attribute, need to retrieve attribute value procName/varName from PKB
			if (declaration->getIsAttribute()) {
				synonymValue = PKB->getNameFromStmtNum(synonymValue);
			}

			resultString += synonymValue;

			if (i != declarations.size() - 1) {
				resultString += SPACE;
			}
		}

		setResults.insert(resultString);
	}

	for (auto setResult : setResults) {
		results.push_back(setResult);
	}
}

shared_ptr<ResultsTable> ResultsProjector::getResultsTableOfTuple(vector<shared_ptr<Declaration>> newDeclarations, shared_ptr<ResultsTable> initialResults,
	shared_ptr<PKBInterface> PKB) {
	shared_ptr<ResultsTable> currentResults = initialResults;
	unordered_set<string> evaluatedSynonyms;

	for (vector<shared_ptr<Declaration>>::iterator it = newDeclarations.begin(); it != newDeclarations.end(); it++) {
		shared_ptr<Declaration> declaration = *it;
		string synonym = declaration->getValue();
		EntityType declarationEntityType = declaration->getEntityType();

		// To handle cases where tuple has repeated synonyms
		if (evaluatedSynonyms.find(synonym) != evaluatedSynonyms.end()) {
			continue;
		}
		evaluatedSynonyms.insert(synonym);

		unordered_set<string> PKBResults = PKB->getEntities(declarationEntityType);

		if (PKBResults.size() == 0) {
			currentResults->setIsNoResult();
			return currentResults;
		}

		if (currentResults->isTableEmpty()) {
			currentResults->populateWithSet(PKBResults, { synonym });
		}
		else {
			currentResults = ResultUtil::getCartesianProductFromSet(PKBResults, { synonym }, currentResults);
		}
	}
	return currentResults;
}