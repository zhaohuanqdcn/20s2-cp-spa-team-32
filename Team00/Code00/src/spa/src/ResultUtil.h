#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <memory>
#include "ResultsTable.h"

using namespace std;

class ResultUtil {
public:
	
	static unordered_set<string> getCommonSynonyms(vector<string> PKBResultSynonyms, unordered_set<string> currentResultSynonyms);

	static unordered_set<string> getCommonSynonyms(unordered_set<string> groupResultSynonyms, unordered_set<string> currentResultSynonyms);

	static shared_ptr<ResultsTable> getCartesianProductFromMap(unordered_map <string, unordered_set<string>> PKBResults, vector<string> synonyms,
		shared_ptr<ResultsTable> currentResults);

	static shared_ptr<ResultsTable> getNaturalJoinFromMap(unordered_map <string, unordered_set<string>> PKBResults, vector<string> synonyms,
		shared_ptr<ResultsTable> currentResults, unordered_set<string> commonSynonyms);

	static shared_ptr<ResultsTable> getCartesianProductFromSet(unordered_set<string> PKBResults, vector<string> synonyms,
		shared_ptr<ResultsTable> currentResults);

	static shared_ptr<ResultsTable> getNaturalJoinFromSet(unordered_set<string> PKBResults, vector<string> synonyms,
		shared_ptr<ResultsTable> currentResults, unordered_set<string> commonSynonyms);

	static shared_ptr<ResultsTable> getCartesianProductOfTables(shared_ptr<ResultsTable> groupResult, shared_ptr<ResultsTable> currentResults);

	static shared_ptr<ResultsTable> getNaturalJoinOfTables(shared_ptr<ResultsTable> groupResult, shared_ptr<ResultsTable> currentResults, 
		unordered_set<string> commonSynonyms);

private:
	static shared_ptr<ResultsTable> getNaturalJoinTwoSynonymsCommon(unordered_map <string, unordered_set<string>> PKBResults, vector<string> synonyms,
		shared_ptr<ResultsTable> currentResults);

	static shared_ptr<ResultsTable> getNaturalJoinOneSynonymCommon(unordered_map <string, unordered_set<string>> PKBResults, vector<string> synonyms,
		bool isLeftSynonymCommon, shared_ptr<ResultsTable> currentResults);
};