#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <unordered_set>

using namespace std;

class ResultsTable {
private:
	unordered_map<string, int> aSynonymIndex;
	vector<vector<string>> aValues;
	bool noResult = false;

public:
	ResultsTable();
	unordered_map<string, int> getSynonymIndexMap();
	vector<vector<string>> getTableValues();
	unordered_set<string> getSynonyms();
	void setTable(unordered_map<string, int> synonymIndex, vector<vector<string>> values);
	bool isNoResult();
	void setIsNoResult();

	bool isTableEmpty();
	int getTableSize();
	void populateWithMap(unordered_map<string, unordered_set<string>> PKBResult, vector<string> synonyms);
	void populateWithSet(unordered_set<string> PKBResult, vector<string> synonyms);
};