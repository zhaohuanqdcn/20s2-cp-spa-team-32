#include "ResultsTable.h"

ResultsTable::ResultsTable() {
	this->aSynonymIndex = unordered_map<string, int>();
	this->aValues = vector<vector<string>>();
}

unordered_map<string, int> ResultsTable::getSynonymIndexMap() {
	return this->aSynonymIndex;
}

vector<vector<string>> ResultsTable::getTableValues() {
	return this->aValues;
}

unordered_set<string> ResultsTable::getSynonyms() {
	unordered_set<string> synonyms;
	for (unordered_map<string, int>::iterator it = this->aSynonymIndex.begin(); it != this->aSynonymIndex.end(); it++) {
		synonyms.insert(it->first);
	}
	return synonyms;
}
bool ResultsTable::isNoResult() {
	return this->noResult;
}


void ResultsTable::setIsNoResult() {
	this->noResult = true;
}

void ResultsTable::setTable(unordered_map<string, int> synonymIndex, vector<vector<string>> values) {
	this->aSynonymIndex = synonymIndex;
	this->aValues = values;
}

bool ResultsTable::isTableEmpty() {
	return (this->aValues.size() == 0);
}

int ResultsTable::getTableSize() {
	return this->aValues.size();
}

void ResultsTable::populateWithMap(unordered_map<string, unordered_set<string>> PKBResult, vector<string> synonyms) {
	string leftSynonym = synonyms.at(0);
	string rightSynonym = synonyms.at(1);
	this->aSynonymIndex.clear();
	this->aSynonymIndex.insert({ leftSynonym, 0 });
	this->aSynonymIndex.insert({ rightSynonym, 1 });
	for (unordered_map<string, unordered_set<string>>::iterator it = PKBResult.begin(); it != PKBResult.end(); it++) {
		string leftSynonymValue = it->first;
		unordered_set<string> values = it->second;

		for (unordered_set<string>::iterator setIt = values.begin(); setIt != values.end(); setIt++) {
			string rightSynonymValue = *setIt;
			vector<string> row = { leftSynonymValue, rightSynonymValue };
			this->aValues.push_back(row);
		}
	}
}

void ResultsTable::populateWithSet(unordered_set<string> PKBResult, vector<string> synonyms) {
	this->aSynonymIndex.clear();
	for (size_t i = 0; i < synonyms.size(); i++) {
		this->aSynonymIndex.insert({ synonyms.at(i), i });
	}
	for (unordered_set<string>::iterator it = PKBResult.begin(); it != PKBResult.end(); it++) {
		string value = *it;
		vector<string> row = {};
		for (size_t i = 0; i < synonyms.size(); i++) {
			row.push_back(value);
		}
		this->aValues.push_back(row);
	}
}