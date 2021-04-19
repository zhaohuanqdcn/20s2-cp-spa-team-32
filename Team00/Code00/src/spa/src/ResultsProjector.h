#pragma once

#include <string>
#include <list>
#include <memory>
#include "SelectClause.h"
#include "ResultsTable.h"
#include "PKBInterface.h"
#include "ResultUtil.h"

class ResultsProjector {
private:
	static string TRUE;
	static string FALSE;
	static string SPACE;

	static unordered_set<string> getAttributeValuesFromStmtNum(unordered_set<string> stmtNum, shared_ptr<PKBInterface> PKB);

	static void projectSingleSynonym(shared_ptr<ResultsTable> evaluatedResults, shared_ptr<Declaration> declaration,
		shared_ptr<PKBInterface> PKB, list<string>& results);

	static void projectTuple(shared_ptr<ResultsTable> evaluatedResults, vector<shared_ptr<Declaration>> declarations,
		shared_ptr<PKBInterface> PKB, list<string>& results);

	static void projectTupleFromResults(vector<shared_ptr<Declaration>> declarations, shared_ptr<ResultsTable> evaluatedResults,
		shared_ptr<PKBInterface> PKB, list<string>& results);

	static shared_ptr<ResultsTable> getResultsTableOfTuple(vector<shared_ptr<Declaration>> newDeclarations, shared_ptr<ResultsTable> initialResults,
		shared_ptr<PKBInterface> PKB);

	static vector<shared_ptr<Declaration>> getSelectedSynonymsInResults(vector<shared_ptr<Declaration>> selectedDeclarations,
		unordered_set<string> resultSynonyms);

	static vector<shared_ptr<Declaration>> getSelectedSynonymsNotInResults(vector<shared_ptr<Declaration>> selectedDeclarations,
		unordered_set<string> resultSynonyms);

public:
	static void projectResults(shared_ptr<ResultsTable> evaluatedResults, shared_ptr<SelectClause> selectClause, shared_ptr<PKBInterface> PKB,
		list<string>& results);
};