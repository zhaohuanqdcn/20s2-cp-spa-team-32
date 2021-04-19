#include "ResultUtil.h"
#include <map>

unordered_set<string> ResultUtil::getCommonSynonyms(vector<string> PKBResultSynonyms, unordered_set<string> currentResultSynonyms) {
	unordered_set<string> commonSynonyms;

	for (vector<string>::iterator it = PKBResultSynonyms.begin(); it != PKBResultSynonyms.end(); it++) {
		if (currentResultSynonyms.find(*it) != currentResultSynonyms.end()) {
			commonSynonyms.insert(*it);
		}
	}

	return commonSynonyms;

}

unordered_set<string> ResultUtil::getCommonSynonyms(unordered_set<string> groupResultSynonyms, unordered_set<string> currentResultSynonyms) {
	unordered_set<string> commonSynonyms;

	for (unordered_set<string>::iterator it = groupResultSynonyms.begin(); it != groupResultSynonyms.end(); it++) {
		if (currentResultSynonyms.find(*it) != currentResultSynonyms.end()) {
			commonSynonyms.insert(*it);
		}
	}

	return commonSynonyms;
}

// used to merge PKB Map results with ResultsTable object that have no common synonyms
// synonyms assumed to be size 2
// all inputs assumed to be non empty
shared_ptr<ResultsTable> ResultUtil::getCartesianProductFromMap(unordered_map <string, unordered_set<string>> PKBResults, vector<string> synonyms,
	shared_ptr<ResultsTable> currentResults) {
	unordered_map<string, int> synonymIndex = currentResults->getSynonymIndexMap();
	vector<vector<string>> tableValues = currentResults->getTableValues();
	vector<vector<string>> newTableValues;

	string leftSynonym = synonyms.at(0);
	string rightSynonym = synonyms.at(1);
	synonymIndex.insert({ leftSynonym, synonymIndex.size() });
	synonymIndex.insert({ rightSynonym, synonymIndex.size() });

	for (unordered_map <string, unordered_set<string>>::iterator pkbResultIt = PKBResults.begin();
		pkbResultIt != PKBResults.end(); pkbResultIt++) {
		string leftSynonymValue = pkbResultIt->first;
		unordered_set<string> rightSynonymValues = pkbResultIt->second;

		for (unordered_set<string>::iterator synonymIt = rightSynonymValues.begin(); synonymIt != rightSynonymValues.end();
			synonymIt++) {
			string rightSynonymValue = *synonymIt;

			for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {
				vector<string> rowCopy(*it);
				rowCopy.push_back(leftSynonymValue);
				rowCopy.push_back(rightSynonymValue);
				newTableValues.push_back(rowCopy);
			}
		}
	}

	currentResults->setTable(synonymIndex, newTableValues);

	return currentResults;
}

// used to merge PKB Set results with ResultsTable object that have no common synonyms
// all inputs assumed to be non empty
shared_ptr<ResultsTable> ResultUtil::getCartesianProductFromSet(unordered_set<string> PKBResults, vector<string> synonyms,
	shared_ptr<ResultsTable> currentResults) {
	unordered_map<string, int> synonymIndex = currentResults->getSynonymIndexMap();
	for (size_t i = 0; i < synonyms.size(); i++) {
		synonymIndex.insert({ synonyms.at(i), synonymIndex.size() });
	}
	
	vector<vector<string>> tableValues = currentResults->getTableValues();
	vector<vector<string>> newTableValues;
	for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {

		for (unordered_set<string>::iterator pkbResultIt = PKBResults.begin(); pkbResultIt != PKBResults.end();
			pkbResultIt++) {
			vector<string> rowCopy(*it);
			string valueToBeAdded = *pkbResultIt;
			for (size_t i = 0; i < synonyms.size(); i++) {
				rowCopy.push_back(valueToBeAdded);
			}
			newTableValues.push_back(rowCopy);
		}

	}

	currentResults->setTable(synonymIndex, newTableValues);

	return currentResults;
}


// used to merge PKB Map results with ResultsTable object that have some common synonyms
// Synonyms is assumed to be of size 2
shared_ptr<ResultsTable> ResultUtil::getNaturalJoinFromMap(unordered_map <string, unordered_set<string>> PKBResults, vector<string> synonyms,
	shared_ptr<ResultsTable> currentResults, unordered_set<string> commonSynonyms) {

	string leftSynonym = synonyms.at(0);
	string rightSynonym = synonyms.at(1);

	// both are common synonyms
	if (commonSynonyms.find(leftSynonym) != commonSynonyms.end() &&
		commonSynonyms.find(rightSynonym) != commonSynonyms.end()) {
		currentResults = getNaturalJoinTwoSynonymsCommon(PKBResults, synonyms, currentResults);
	}
	else { // exactly one synonym is common
		string commonSynonym;
		bool isLeftSynonymCommon = (commonSynonyms.find(leftSynonym) != commonSynonyms.end());
		currentResults = getNaturalJoinOneSynonymCommon(PKBResults, synonyms, isLeftSynonymCommon,
			currentResults);
	}

	return currentResults;
}

// used to merge PKB Set results with ResultsTable object that have some common synonym
// vector synonyms assumed to have size range of 1 to 2
shared_ptr<ResultsTable> ResultUtil::getNaturalJoinFromSet(unordered_set<string> PKBResults, vector<string> synonyms,
	shared_ptr<ResultsTable> currentResults, unordered_set<string> commonSynonyms) {
	unordered_map<string, int> synonymIndex = currentResults->getSynonymIndexMap();
	vector<vector<string>> tableValues = currentResults->getTableValues();
	vector<vector<string>> newTableValues;

	// Only one synonym involved - it must be the common synonym
	if (synonyms.size() == 1) {
		int index = synonymIndex.find(synonyms.at(0))->second;

		for (unordered_set<string>::iterator pkbResultIt = PKBResults.begin(); pkbResultIt != PKBResults.end();
			pkbResultIt++) {
			string valueFromPKB = *pkbResultIt;

			for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {
				vector<string> row = *it;
				if (row.at(index) == valueFromPKB) {
					newTableValues.push_back(row);
				}
			}
		}

		currentResults->setTable(synonymIndex, newTableValues);
		return currentResults;
	}

	// Two synonyms involved - need to find out 1 or 2 common synonyms
	string leftSynonym = synonyms.at(0);
	string rightSynonym = synonyms.at(1);

	// both are common synonyms - no need to add new synonym to resultsTable
	if (commonSynonyms.find(leftSynonym) != commonSynonyms.end() &&
		commonSynonyms.find(rightSynonym) != commonSynonyms.end()) {
		int leftSynonymIndex = synonymIndex.find(leftSynonym)->second;
		int rightSynonymIndex = synonymIndex.find(rightSynonym)->second;

		for (unordered_set<string>::iterator pkbResultsIt = PKBResults.begin(); pkbResultsIt != PKBResults.end(); pkbResultsIt++) {
			string valueToBeAdded = *pkbResultsIt;

			for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {
				vector<string> row = *it;
				if (row.at(leftSynonymIndex) == valueToBeAdded && row.at(rightSynonymIndex) == valueToBeAdded) {
					vector<string> rowCopy(row);
					newTableValues.push_back(rowCopy);
				}
			}
		}
	}
	else { // exactly one synonym is common
		string commonSynonym;
		string uncommonSynonym;
		if (commonSynonyms.find(leftSynonym) != commonSynonyms.end()) {
			commonSynonym = leftSynonym;
			uncommonSynonym = rightSynonym;
		}
		else {
			commonSynonym = rightSynonym;
			uncommonSynonym = leftSynonym;
		}

		int commonSynonymIndex = synonymIndex.find(commonSynonym)->second;
		synonymIndex.insert({ uncommonSynonym, synonymIndex.size() });

		for (unordered_set<string>::iterator pkbResultsIt = PKBResults.begin(); pkbResultsIt != PKBResults.end(); pkbResultsIt++) {
			string valueToBeAdded = *pkbResultsIt;

			for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {
				vector<string> row = *it;
				if (row.at(commonSynonymIndex) == valueToBeAdded) {
					vector<string> rowCopy(row);
					rowCopy.push_back(valueToBeAdded);
					newTableValues.push_back(rowCopy);
				}
			}
		}

	}

	currentResults->setTable(synonymIndex, newTableValues);
	return currentResults;
}

shared_ptr<ResultsTable> ResultUtil::getNaturalJoinTwoSynonymsCommon(unordered_map <string, unordered_set<string>> PKBResults, vector<string> synonyms,
	shared_ptr<ResultsTable> currentResults) {
	unordered_map<string, int> synonymIndex = currentResults->getSynonymIndexMap();
	string leftSynonym = synonyms.at(0);
	string rightSynonym = synonyms.at(1);
	int leftSynonymIndex = synonymIndex.find(leftSynonym)->second;
	int rightSynonymIndex = synonymIndex.find(rightSynonym)->second;

	vector<vector<string>> tableValues = currentResults->getTableValues();
	vector<vector<string>> newTableValues;

	for (unordered_map <string, unordered_set<string>>::iterator pkbResultsIt = PKBResults.begin(); pkbResultsIt != PKBResults.end();
		pkbResultsIt++) {
		string leftSynonymValue = pkbResultsIt->first;
		unordered_set<string> rightSynonymValues = pkbResultsIt->second;

		for (unordered_set<string>::iterator rowIt = rightSynonymValues.begin(); rowIt != rightSynonymValues.end(); rowIt++) {
			string rightSynonymValue = *rowIt;
			for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {
				vector<string> row = *it;
				if (row.at(leftSynonymIndex) == leftSynonymValue && row.at(rightSynonymIndex) == rightSynonymValue) {
					vector<string> rowCopy(row);
					newTableValues.push_back(rowCopy);
				}
			}
		}
	}

	currentResults->setTable(synonymIndex, newTableValues);
	return currentResults;
}

shared_ptr<ResultsTable> ResultUtil::getNaturalJoinOneSynonymCommon(unordered_map <string, unordered_set<string>> PKBResults, vector<string> synonyms,
	bool isLeftSynonymCommon, shared_ptr<ResultsTable> currentResults) {
	unordered_map<string, int> synonymIndex = currentResults->getSynonymIndexMap();
	string leftSynonym = synonyms.at(0);
	string rightSynonym = synonyms.at(1);
	string commonSynonym;
	string uncommonSynonym;
	if (isLeftSynonymCommon) {
		commonSynonym = leftSynonym;
		uncommonSynonym = rightSynonym;
	}
	else {
		commonSynonym = rightSynonym;
		uncommonSynonym = leftSynonym;
	}
	int commonSynonymIndex = synonymIndex.find(commonSynonym)->second;
	synonymIndex.insert({ uncommonSynonym, synonymIndex.size() });

	vector<vector<string>> tableValues = currentResults->getTableValues();
	vector<vector<string>> newTableValues;

	if (isLeftSynonymCommon) {
		for (unordered_map <string, unordered_set<string>>::iterator pkbResultIt = PKBResults.begin();
			pkbResultIt != PKBResults.end(); pkbResultIt++) {
			string leftSynonymValue = pkbResultIt->first;
			unordered_set<string> rightSynonymValues = pkbResultIt->second;
			
			for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {
				vector<string> row = *it;
				if (row.at(commonSynonymIndex) == leftSynonymValue) {
					for (unordered_set<string>::iterator setIt = rightSynonymValues.begin(); setIt != rightSynonymValues.end(); setIt++) {
						vector<string> rowCopy(row);
						rowCopy.push_back(*setIt);
						newTableValues.push_back(rowCopy);
					}
				}
			}
		}
	}
	else {
		for (unordered_map <string, unordered_set<string>>::iterator pkbResultIt = PKBResults.begin();
			pkbResultIt != PKBResults.end(); pkbResultIt++) {
			string leftSynonymValue = pkbResultIt->first;
			unordered_set<string> rightSynonymValues = pkbResultIt->second;

			for (unordered_set<string>::iterator setIt = rightSynonymValues.begin(); setIt != rightSynonymValues.end(); setIt++) {
				string rightSynonymValue = *setIt;

				for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {
					vector<string> row = *it;
					if (row.at(commonSynonymIndex) == rightSynonymValue) {
						vector<string> rowCopy(row);
						rowCopy.push_back(leftSynonymValue);
						newTableValues.push_back(rowCopy);
					}
				}
			}
		}
	}

	currentResults->setTable(synonymIndex, newTableValues);
	return currentResults;
}

shared_ptr<ResultsTable> ResultUtil::getCartesianProductOfTables(shared_ptr<ResultsTable> groupResult, shared_ptr<ResultsTable> currentResults) {
	unordered_set<string> groupResultSynonyms = groupResult->getSynonyms();
	unordered_map<string, int> currentResultsSynonymIndex = currentResults->getSynonymIndexMap();
	for (unordered_set<string>::iterator it = groupResultSynonyms.begin(); it != groupResultSynonyms.end(); it++) {
		string groupResultSynonym = *it;
		currentResultsSynonymIndex.insert({ groupResultSynonym, currentResultsSynonymIndex.size() });
	}

	unordered_map<string, int> groupResultSynonymIndex = groupResult->getSynonymIndexMap();
	vector<vector<string>> currentResultsTableValues = currentResults->getTableValues();
	vector<vector<string>> groupResultTableValues = groupResult->getTableValues();
	vector<vector<string>> newTableValues;
	for (vector<vector<string>>::iterator currentResultIt = currentResultsTableValues.begin();
		currentResultIt != currentResultsTableValues.end(); currentResultIt++) {
		vector<string> currentResultRow = (*currentResultIt);

		for (vector<vector<string>>::iterator groupResultIt = groupResultTableValues.begin(); groupResultIt != groupResultTableValues.end(); groupResultIt++) {
			vector<string>groupResultRow = (*groupResultIt);
			vector<string> rowCopy(currentResultRow);

			for (unordered_set<string>::iterator it = groupResultSynonyms.begin(); it != groupResultSynonyms.end(); it++) {
				string groupResultSynonym = *it;
				int groupResultIndex = groupResultSynonymIndex.find(groupResultSynonym)->second;
				string value = groupResultRow.at(groupResultIndex);
				rowCopy.push_back(value);
			}

			newTableValues.push_back(rowCopy);
		}

	}

	currentResults->setTable(currentResultsSynonymIndex, newTableValues);

	return currentResults;
}

shared_ptr<ResultsTable> ResultUtil::getNaturalJoinOfTables(shared_ptr<ResultsTable> groupResult, shared_ptr<ResultsTable> currentResults,
	unordered_set<string> commonSynonyms) {
	unordered_set<string> groupResultSynonyms = groupResult->getSynonyms();
	unordered_set<string> currentResultsSynonyms = currentResults->getSynonyms();
	unordered_map<string, int> currentResultsSynonymIndex = currentResults->getSynonymIndexMap();

	unordered_set<string> uncommonSynonyms = {};
	for (unordered_set<string>::iterator it = groupResultSynonyms.begin(); it != groupResultSynonyms.end(); it++) {
		string groupResultSynonym = *it;

		// synonym is not in current results table, aka uncommon synonym
		if (commonSynonyms.find(groupResultSynonym) == commonSynonyms.end()) {
			uncommonSynonyms.insert(groupResultSynonym);
			currentResultsSynonymIndex.insert({ groupResultSynonym, currentResultsSynonymIndex.size() });
		}
	}

	unordered_map<string, int> groupResultSynonymIndex = groupResult->getSynonymIndexMap();
	vector<vector<string>> currentResultsTableValues = currentResults->getTableValues();
	vector<vector<string>> groupResultTableValues = groupResult->getTableValues();
	vector<vector<string>> newTableValues;
	for (vector<vector<string>>::iterator currentResultIt = currentResultsTableValues.begin();
		currentResultIt != currentResultsTableValues.end(); currentResultIt++) {
		vector<string> currentResultRow = (*currentResultIt);

		for (vector<vector<string>>::iterator groupResultIt = groupResultTableValues.begin(); groupResultIt != groupResultTableValues.end(); groupResultIt++) {
			vector<string>groupResultRow = (*groupResultIt);

			// checking if the values of common synonyms match
			bool allCommonSynonymsMatch = true;
			for (unordered_set<string>::iterator it = commonSynonyms.begin(); it != commonSynonyms.end(); it++) {
				string commonSynonym = *it;
				int groupResultIndex = groupResultSynonymIndex.find(commonSynonym)->second;
				int currentResultIndex = currentResultsSynonymIndex.find(commonSynonym)->second;
				string groupResultValue = groupResultRow.at(groupResultIndex);
				string currentResultValue = currentResultRow.at(currentResultIndex);
				if (groupResultValue != currentResultValue) { 
					allCommonSynonymsMatch = false;
					break;
				}
			}

			if (!allCommonSynonymsMatch) { // if one of the common synonyms do not match, skip this row
				continue;
			}

			vector<string> rowCopy(currentResultRow);
			// since all common synonyms match, add uncommon synonyms to new row
			for (unordered_set<string>::iterator it = uncommonSynonyms.begin(); it != uncommonSynonyms.end(); it++) {
				string uncommonSynonym = *it;
				int groupResultIndex = groupResultSynonymIndex.find(uncommonSynonym)->second;
				string value = groupResultRow.at(groupResultIndex);
				rowCopy.push_back(value);
			}

			newTableValues.push_back(rowCopy);
		}

	}

	currentResults->setTable(currentResultsSynonymIndex, newTableValues);

	return currentResults;
}