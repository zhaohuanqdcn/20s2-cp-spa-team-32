#include "QueryOptimizer.h"

vector<vector<shared_ptr<OptionalClause>>> QueryOptimizer::sortClausesIntoGroups(vector<shared_ptr<OptionalClause>> clauses) {
	DisjointClausesSet disjointClausesSet = DisjointClausesSet(clauses);
	return disjointClausesSet.getClauses();
}
 
bool QueryOptimizer::sortClauseBy(shared_ptr<OptionalClause> clause1, shared_ptr<OptionalClause> clause2) {
	return clause1->getResultSize() < clause2->getResultSize();
}

vector<shared_ptr<OptionalClause>> QueryOptimizer::sortClausesByResultSize(vector<shared_ptr<OptionalClause>> clauses) {
	sort((&clauses)->begin(), (&clauses)->end(), sortClauseBy);
	return clauses;
}

bool QueryOptimizer::sortTablesBy(shared_ptr<ResultsTable> table1, shared_ptr<ResultsTable> table2) {
	return table1->getTableSize() < table2->getTableSize();
}
vector<shared_ptr<ResultsTable>> QueryOptimizer::sortTablesBySize(vector<shared_ptr<ResultsTable>> tables) {
	sort((&tables)->begin(), (&tables)->end(), sortTablesBy);
	return tables;
}