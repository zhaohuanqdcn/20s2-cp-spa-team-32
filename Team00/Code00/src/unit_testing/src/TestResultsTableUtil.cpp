#include "TestResultsTableUtil.h"
#include <iostream>

void TestResultsTableUtil::checkTable(vector<vector<string>> actualTable, vector<vector<string>> expectedTable) {
	REQUIRE(actualTable.size() == expectedTable.size());

	for (int i = 0; i < actualTable.size(); i++) {
		vector<string> actualRow = actualTable.at(i);
		vector<string> expectedRow = expectedTable.at(i);
		sort(actualRow.begin(), actualRow.end());
		sort(expectedRow.begin(), expectedRow.end());

		REQUIRE(actualRow.size() == expectedRow.size());
		// cerr << "---------------------------" << endl;
		// for (auto x: actualRow) cerr << x << " "; cerr << endl;
		// for (auto y: expectedRow) cerr << y << " "; cerr << endl;
		// cerr << "---------------------------" << endl;	

		for (int j = 0; j < actualRow.size(); j++) {
			REQUIRE(actualRow.at(j) == expectedRow.at(j));
		}
	}
}

void TestResultsTableUtil::checkMap(unordered_map<string, int> actualMap, unordered_map<string, int> expectedMap) {
	REQUIRE(actualMap.size() == expectedMap.size());

	for (unordered_map<string, int>::iterator it = expectedMap.begin(); it != expectedMap.end(); it++) {
		string expectedKey = it->first;
		int expectedValue = it->second;

		unordered_map<string, int>::iterator actualIt = actualMap.find(expectedKey);
		REQUIRE(actualIt != actualMap.end());
		REQUIRE(actualIt->second == expectedValue);
		actualMap.erase(expectedKey);
	}

	REQUIRE(actualMap.size() == 0);
}

void TestResultsTableUtil::checkSet(unordered_set<string> actualSet, unordered_set<string> expectedSet) {
	REQUIRE(actualSet.size() == expectedSet.size());

	unordered_set<string>::iterator actualSetIt = actualSet.begin();
	unordered_set<string>::iterator expectedSetIt = expectedSet.begin();

	while (actualSetIt != actualSet.end() || expectedSetIt != expectedSet.end()) {
		REQUIRE(*actualSetIt == *expectedSetIt);
		actualSetIt++;
		expectedSetIt++;
	}
}

void  TestResultsTableUtil::checkList(list<string> actualList, list<string> expectedList) {
	vector<string> A;
	for (auto x : actualList) A.emplace_back(x);
	vector<string> B;
	for (auto x : expectedList) B.emplace_back(x);
	sort(A.begin(), A.end());
	sort(B.begin(), B.end());

	for (int i = 0; i < A.size(); i++) {
		REQUIRE(A[i] == B[i]);
	}
}

void TestResultsTableUtil::checkClauseGroups(vector<vector<shared_ptr<OptionalClause>>> actualGroups, vector<vector<shared_ptr<OptionalClause>>> expectedGroups) {
	REQUIRE(actualGroups.size() == expectedGroups.size());
	for (size_t i = 0; i < actualGroups.size(); i++) {
		vector<shared_ptr<OptionalClause>> actualGroup = actualGroups.at(i);
		vector<shared_ptr<OptionalClause>> expectedGroup = expectedGroups.at(i);

		checkClauseList(actualGroup, expectedGroup);
	}
}

void TestResultsTableUtil::checkClauseList(vector<shared_ptr<OptionalClause>> actualList, vector<shared_ptr<OptionalClause>> expectedList) {
	REQUIRE(actualList.size() == expectedList.size());
	for (size_t j = 0; j < actualList.size(); j++) {
		shared_ptr<OptionalClause> actualClause = actualList.at(j);
		shared_ptr<OptionalClause> expectedClause = expectedList.at(j);

		REQUIRE(actualClause->getClauseType() == expectedClause->getClauseType());
		shared_ptr<QueryInput> actualLeftQueryInput = actualClause->getLeftInput();
		shared_ptr<QueryInput> actualRightQueryInput = actualClause->getRightInput();
		shared_ptr<QueryInput> expectedLeftQueryInput = expectedClause->getLeftInput();
		shared_ptr<QueryInput> expectedRightQueryInput = expectedClause->getRightInput();

		checkQueryInput(actualLeftQueryInput, expectedLeftQueryInput);
		checkQueryInput(actualRightQueryInput, expectedRightQueryInput);

		if (actualClause->getClauseType() == ClauseType::RELATIONSHIP) {
			shared_ptr<RelationshipClause> actualRSClause = dynamic_pointer_cast<RelationshipClause>(actualClause);
			shared_ptr<RelationshipClause> expectedRSClause = dynamic_pointer_cast<RelationshipClause>(expectedClause);

			REQUIRE(actualRSClause->getRelationshipType() == expectedRSClause->getRelationshipType());
		}

		if (actualClause->getClauseType() == ClauseType::PATTERN) {
			shared_ptr<PatternClause> actualPatternClause = dynamic_pointer_cast<PatternClause>(actualClause);
			shared_ptr<PatternClause> expectedPatternClause = dynamic_pointer_cast<PatternClause>(expectedClause);
			shared_ptr<Expression> actualExpression = actualPatternClause->getExpression();
			shared_ptr<Expression> expectedExpression = expectedPatternClause->getExpression();

			bool nullCheck = (actualExpression == NULL && expectedExpression == NULL) || (actualExpression != NULL && expectedExpression != NULL);
			REQUIRE(nullCheck);
			if (actualExpression != NULL && expectedExpression != NULL) {
				REQUIRE(actualPatternClause->getExpression()->getType() == expectedPatternClause->getExpression()->getType());
				REQUIRE(actualPatternClause->getExpression()->getValue() == expectedPatternClause->getExpression()->getValue());
			}

		}

	}
}

void TestResultsTableUtil::checkQueryInput(shared_ptr<QueryInput> actualQueryInput, shared_ptr<QueryInput> expectedQueryInput) {
	REQUIRE(actualQueryInput->getValue() == expectedQueryInput->getValue());
	REQUIRE(actualQueryInput->getQueryInputType() == expectedQueryInput->getQueryInputType());
	if (actualQueryInput->getQueryInputType() == QueryInputType::DECLARATION) {
		shared_ptr<Declaration> actualDeclaration = dynamic_pointer_cast<Declaration>(actualQueryInput);
		shared_ptr<Declaration> expectedDeclaration = dynamic_pointer_cast<Declaration>(expectedQueryInput);
		REQUIRE(actualDeclaration->getEntityType() == expectedDeclaration->getEntityType());
		REQUIRE(actualDeclaration->getIsAttribute() == expectedDeclaration->getIsAttribute());
	}
}	