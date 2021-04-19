#pragma once
#include <vector>
#include <string>
#include <list>
#include <unordered_map>
#include "catch.hpp"

using namespace std;

class TestResultsTableUtil {
public:
	static void checkTable(vector<vector<string>> actualTable, vector<vector<string>> expectedTable);

	static void checkMap(unordered_map<string, int> actualMap, unordered_map<string, int> expectedMap);

	static void checkList(list<string> actualList, list<string> expectedList);
};