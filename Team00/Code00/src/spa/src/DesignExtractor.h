#ifndef __DESIGN__EXTRACTOR__H__
#define __DESIGN__EXTRACTOR__H__

using namespace std;

#include "PKB.h"
#include "DesignExtractorHelper.h"

#include <set>

class DesignExtractor {
private:
	int numberOfStatement;

	vector<vector<int> > nexts;
	vector<vector<int> > parents;
	vector<vector<int> > follows;
	vector<EntityType> types;
	vector<vector<string>> uses;
	vector<vector<string>> modifies;
	vector<vector<int>> affects;
	unordered_map<string, vector<string>> calls;
	Ownership<string, int> procedures;
	Ownership<int, string> statementCalls;
	vector<vector<Expression> > expressions;
	set<string> constants;
	set<string> proceduresList;
	set<string> variables;

	///indirect relationships
	Indirect<string> callStar;
	Indirect<int> parentStar;
	Indirect<int> followStar;
	Indirect<int> nextStar;
	Indirect<int> affectStar;
	Indirect<int> nextBip;
	Indirect<int> nextBipStar;
	Indirect<int> affectsBip;
	Indirect<int> affectsBipStar;
	Indirect<int> CFGBip;
	Ownership<int, string> directUses;
	Ownership<int, string> indirectUses;
	Ownership<int, string> directModifies;
	Ownership<int, string> indirectModifies;
	Ownership<string, string> directProcedureUses;
	Ownership<string, string> indirectProcedureUses;
	Ownership<string, string> directProcedureModifies;
	Ownership<string, string> indirectProcedureModifies;
	Ownership<int, string> controlVariables;
	Ownership<string, int> endingProc;


	/// Return last statements of each block 
	vector<int> buildCFGBlock(int stmt, int& maxLineStmt);

	void buildCFG();

	void affectDFS(int startStmt, int curStmt, string var, vector<int>& visited);

	void buildDirectAffect();

	void buildNextBip();

	void affectsBipDFS(int startStmt, int curStmt, string var, vector<int>& visited);

	void buildAffectsBip();

	// Build Indirect relationships
	void buildIndirectRelationships();
public:
	DesignExtractor();

	void increaseNumberOfStatement(EntityType type);

	void insertParent(int parent, int child);

	void insertFollow(int before, int after);

	void insertUses(int id, const string& variable);

	void insertModifies(int id, const string& variable);

	void insertNext(int id1, int id2);

	void insertExpression(int id, const Expression& expression);

	void insertStatementCall(int id, const string &procName);

	void insertConstant(string c);

	void setProcedure(string name, int low, int high);

	void setCalls(Indirect<string>& edges);

	shared_ptr<PKB> extractToPKB();

	vector<string> getUses(int index) const;

	vector<string> getModifies(int index) const;

	vector<string> getIndirectUses(int index) const;

	vector<string> getIndirectModifies(int index) const;

	vector<int> getFollows(int index) const;

	vector<string> getCallStars(string procName) const;

	vector<int> getNext(int index) const;

	vector<int> getNextStar(int index) const;

	vector<int> getAffect(int index) const;

	vector<int> getAffectStar(int index) const;

	vector<int> getNextBip(int index) const;

	vector<int> getNextBipStar(int index) const;

	vector<int> getAffectBip(int index) const;

	vector<int> getAffectBipStar(int index) const;

	vector<int> getCFGBip(int index) const;

	vector<int> getEndingProcedure(string procName) const;

	vector<Expression> getExpression(int index) const;
};

#endif ///__DESIGN__EXTRACTOR__H__