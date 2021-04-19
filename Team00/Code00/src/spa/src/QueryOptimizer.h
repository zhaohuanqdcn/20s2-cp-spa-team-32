#pragma once

#include <vector>
#include <algorithm>
#include "OptionalClause.h"
#include "DisjointClausesSet.h"
#include "ResultsTable.h"

class QueryOptimizer {
public:
	static vector<vector<shared_ptr<OptionalClause>>> sortClausesIntoGroups(vector<shared_ptr<OptionalClause>> clauses);

	static bool sortClauseBy(shared_ptr<OptionalClause> clause1, shared_ptr<OptionalClause> clause2);
	static vector<shared_ptr<OptionalClause>> sortClausesByResultSize(vector<shared_ptr<OptionalClause>> clauses);

	static bool sortTablesBy(shared_ptr<ResultsTable> table1, shared_ptr<ResultsTable> table2);
	static vector<shared_ptr<ResultsTable>> sortTablesBySize(vector<shared_ptr<ResultsTable>> tables);
};