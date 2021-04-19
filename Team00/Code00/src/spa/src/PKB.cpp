#include <stdio.h>
#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include <memory>
#include <unordered_map>

#include "PKB.h"
#include "EntityType.h"
#include "RelationshipType.h"
#include "QueryInputType.h"
#include "Declaration.h"

using namespace std;

PKB::PKB(const int& n) : number(n) {
	for (int i = 1; i <= n; i++) {
		this->entities[EntityType::STMT].insert(to_string(i));
		this->entities[EntityType::PROGLINE].insert(to_string(i));
	}
}

void PKB::init() {
	// init proc-var table
	unordered_set<string> varTable = this->entities[EntityType::VAR];
	for (string name : this->entities[EntityType::PROC]) {
		if (varTable.find(name) != varTable.end()) { // identical names
			this->procVarName[name].insert(name);
		}
	}
	// init const-stmt# table
	for (string val : this->entities[EntityType::CONST]) {
		if (types.find(val) != types.end()) { // identical values
			EntityType t = types[val];
			this->constStmtNum[t][val].insert(val);
			this->constStmtNum[EntityType::STMT][val].insert(val);
			this->constStmtNum[EntityType::PROGLINE][val].insert(val);
		}
	}
}

bool PKB::setStatementType(const int& index, const EntityType& type) {
	if (type == EntityType::STMT || type == EntityType::VAR ||
		type == EntityType::CONST || type == EntityType::PROC) {
		return false;
	}
	else if (index > this->number || index <= 0) {
		return false;
	}
	else {
		string indexString = to_string(index);
		if (this->types.find(indexString) != types.end()) { // key exists
			return false;
		}
		else {
			this->types[indexString] = type;
			this->entities[type].insert(indexString);
			return true;
		}
	}
}

bool PKB::insertRelationship(const RelationshipType& type, const string& e1, const string& e2) {
	int t = type;
	this->relations[t][e1].insert(e2);
	this->relationsBy[t][e2].insert(e1);
	this->relationKeys[t].insert(e1);
	this->relationByKeys[t].insert(e2);
	return true;
}

bool PKB::insertParent(const int& parent, const int& child) {
	if (parent <= 0 || parent > this->number) {
		return false;
	}
	else if (child <= 0 || child > this->number) {
		return false;
	}
	else if (child <= parent) {
		return false;
	}
	else {
		int t = RelationshipType::PARENT;
		string parentString = to_string(parent);
		string childString = to_string(child);
		if (this->relationByKeys[t].find(childString) !=
			this->relationByKeys[t].end()) { // key exists
			return false;
		}
		else {
			return this->insertRelationship(
				RelationshipType::PARENT, parentString, childString);
		}
	}
}

bool PKB::insertParentStar(const int& parent, const int& child) {
	if (parent <= 0 || parent > this->number) {
		return false;
	}
	else if (child <= 0 || child > this->number) {
		return false;
	}
	else if (child <= parent) {
		return false;
	}
	else {
		string parentString = to_string(parent);
		string childString = to_string(child);
		return this->insertRelationship(
			RelationshipType::PARENT_T, parentString, childString);
	}
}


bool PKB::insertFollow(const int& former, const int& latter) {
	if (former <= 0 || former > this->number) {
		return false;
	}
	else if (latter <= 0 || latter > this->number) {
		return false;
	}
	else if (former >= latter) {
		return false;
	}
	else {
		int t = RelationshipType::FOLLOWS;
		string formerString = to_string(former);
		string latterString = to_string(latter);
		if (this->relationKeys[t].find(formerString) !=
			this->relationKeys[t].end()) { // key exists
			return false;
		}
		else {
			return this->insertRelationship(
				RelationshipType::FOLLOWS, formerString, latterString);
		}
	}
}


bool PKB::insertFollowStar(const int& former, const int& latter) {
	if (former <= 0 || former > this->number) {
		return false;
	}
	else if (latter <= 0 || latter > this->number) {
		return false;
	}
	else if (former >= latter) {
		return false;
	}
	else {
		string formerString = to_string(former);
		string latterString = to_string(latter);
		return this->insertRelationship(
			RelationshipType::FOLLOWS_T, formerString, latterString);
	}
}

bool PKB::insertAffect(const int& former, const int& latter) {
	if (former <= 0 || former > this->number) {
		return false;
	}
	else if (latter <= 0 || latter > this->number) {
		return false;
	}
	else {
		string formerString = to_string(former);
		string latterString = to_string(latter);
		if (types[formerString] != EntityType::ASSIGN || 
			types[latterString] != EntityType::ASSIGN) {
			return false;
		}
		return this->insertRelationship(
			RelationshipType::AFFECTS, formerString, latterString);
	}
}

bool PKB::insertAffectStar(const int& former, const int& latter) {
	if (former <= 0 || former > this->number) {
		return false;
	}
	else if (latter <= 0 || latter > this->number) {
		return false;
	}
	else {
		string formerString = to_string(former);
		string latterString = to_string(latter);
		if (types[formerString] != EntityType::ASSIGN ||
			types[latterString] != EntityType::ASSIGN) {
			return false;
		}
		return this->insertRelationship(
			RelationshipType::AFFECTS_T, formerString, latterString);
	}
}

bool PKB::insertAffectBip(const int& former, const int& latter) {
	if (former <= 0 || former > this->number) {
		return false;
	}
	else if (latter <= 0 || latter > this->number) {
		return false;
	}
	else {
		string formerString = to_string(former);
		string latterString = to_string(latter);
		if (types[formerString] != EntityType::ASSIGN ||
			types[latterString] != EntityType::ASSIGN) {
			return false;
		}
		return this->insertRelationship(
			RelationshipType::AFFECTSBIP, formerString, latterString);
	}
}

bool PKB::insertAffectBipStar(const int& former, const int& latter) {
	if (former <= 0 || former > this->number) {
		return false;
	}
	else if (latter <= 0 || latter > this->number) {
		return false;
	}
	else {
		string formerString = to_string(former);
		string latterString = to_string(latter);
		if (types[formerString] != EntityType::ASSIGN ||
			types[latterString] != EntityType::ASSIGN) {
			return false;
		}
		return this->insertRelationship(
			RelationshipType::AFFECTSBIP_T, formerString, latterString);
	}
}

bool PKB::insertNext(const int& former, const int& latter) {
	if (former <= 0 || former > this->number) {
		return false;
	}
	else if (latter <= 0 || latter > this->number) {
		return false;
	}
	else {
		int t = RelationshipType::NEXT;
		string formerString = to_string(former);
		string latterString = to_string(latter);
		return this->insertRelationship(
			RelationshipType::NEXT, formerString, latterString);
	}
}

bool PKB::insertNextStar(const int& former, const int& latter) {
	if (former <= 0 || former > this->number) {
		return false;
	}
	else if (latter <= 0 || latter > this->number) {
		return false;
	}
	else {
		int t = RelationshipType::NEXT_T;
		string formerString = to_string(former);
		string latterString = to_string(latter);
		return this->insertRelationship(
			RelationshipType::NEXT_T, formerString, latterString);
	}
}

bool PKB::insertNextBip(const int& former, const int& latter) {
	if (former <= 0 || former > this->number) {
		return false;
	}
	else if (latter <= 0 || latter > this->number) {
		return false;
	}
	else {
		int t = RelationshipType::NEXTBIP;
		string formerString = to_string(former);
		string latterString = to_string(latter);
		return this->insertRelationship(
			RelationshipType::NEXTBIP, formerString, latterString);
	}
}

bool PKB::insertNextBipStar(const int& former, const int& latter) {
	if (former <= 0 || former > this->number) {
		return false;
	}
	else if (latter <= 0 || latter > this->number) {
		return false;
	}
	else {
		int t = RelationshipType::NEXTBIP_T;
		string formerString = to_string(former);
		string latterString = to_string(latter);
		return this->insertRelationship(
			RelationshipType::NEXTBIP_T, formerString, latterString);
	}
}

bool PKB::insertUses(const int& index, const string& variable) {
	if (index <= 0 || index > this->number) {
		return false;
	}
	else {
		string indexString = to_string(index);
		this->insertVariable(variable);
		return this->insertRelationship(RelationshipType::USES, indexString, variable);
	}
}

bool PKB::insertProcUses(const string& procedure, const string& variable) {
	this->procUses[procedure].insert(variable);
	this->usedByProc[variable].insert(procedure);
	this->procUsesKeys.insert(procedure);
	this->usedByProcKeys.insert(variable);
	return true;
}

bool PKB::insertModifies(const int& index, const string& variable) {
	if (index <= 0 || index > this->number) {
		return false;
	}
	else {
		string indexString = to_string(index);
		this->insertVariable(variable);
		return this->insertRelationship(RelationshipType::MODIFIES, indexString, variable);
	}
}

bool PKB::insertProcModifies(const string& procedure, const string& variable) {
	this->procModifies[procedure].insert(variable);
	this->modifiedByProc[variable].insert(procedure);
	this->procModifiesKeys.insert(procedure);
	this->modByProcKeys.insert(variable);
	return true;
}

bool PKB::insertCalls(const string& caller, const string& callee) {
	this->insertProcedure(caller);
	this->insertProcedure(callee);
	return this->insertRelationship(RelationshipType::CALLS, caller, callee);
}

bool PKB::insertCallsStar(const string& caller, const string& callee) {
	this->insertProcedure(caller);
	this->insertProcedure(callee);
	return this->insertRelationship(RelationshipType::CALLS_T, caller, callee);
}

bool PKB::insertExpression(const int& index, const Expression& expression) {
	if (index <= 0 || index > this->number) {
		return false;
	}
	else {
		this->expressions[expression].insert(to_string(index));
		return true;
	}
}

bool PKB::setControlVariable(const int& index, const string& variable) {
	if (index <= 0 || index > this->number) {
		return false;
	}
	else {
		string indexString = to_string(index);
		int t = this->types[indexString] == EntityType::IF ? 0
			: this->types[indexString] == EntityType::WHILE ? 1 : -1;
		if (t == -1) {
			return false;
		}
		else {
			this->contPattern[t][indexString].insert(variable);
			this->contPatternBy[t][variable].insert(indexString);
			this->contPatternKeys[t].insert(indexString);
			return true;
		}
	}
}

bool PKB::insertVariable(const string& variable) {
	this->entities[EntityType::VAR].insert(variable);
	return true;
}

bool PKB::insertConst(const string& number) {
	this->entities[EntityType::CONST].insert(number);
	return true;
}

bool PKB::insertProcedure(const string& proc) {
	this->entities[EntityType::PROC].insert(proc);
	return true;
}

bool PKB::insertUsedName(const int& index, const string& name) {
	if (index <= 0 || index > this->number) {
		return false;
	}
	string indexString = to_string(index);
	EntityType type = this->types[indexString];
	int t = type == EntityType::CALL ? 0 : type == EntityType::PRINT ? 1 : type == EntityType::READ ? 2 : -1;
	if (t == -1) {
		return false;
	}
	this->nameUsed[indexString] = name;
	this->nameUsedBy[t][name].insert(indexString);
	return true;
}

unordered_set<string> PKB::getEntities(const EntityType& type) {
	return this->entities[type];
}

// retrieval results for such that clauses

bool PKB::getBooleanResultOfRS(const RelationshipType& type,
	shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	QueryInputType t1 = input1->getQueryInputType();
	QueryInputType t2 = input2->getQueryInputType();
	if (t1 == QueryInputType::ANY && t2 == QueryInputType::ANY) { // eg. parent(_, _). can't be uses/modifies
		return !this->relations[type].empty();
	}
	else if (t1 == QueryInputType::ANY) { // eg. follows(_, 3). can't be uses/modifies
		return !this->relationsBy[type][input2->getValue()].empty();
	}
	else if (t1 != QueryInputType::ANY) {
		unordered_set<string> results;
		if (t1 == QueryInputType::IDENT) { // input1 is a procedure
			if (type == RelationshipType::USES) { // procedure uses
				results = this->procUses[input1->getValue()];
			}
			else if (type == RelationshipType::MODIFIES) { // procedure modifies
				results = this->procModifies[input1->getValue()];
			}
			else { // call or call*
				results = this->relations[type][input1->getValue()];
			}
		}
		else { // does not involve procedures
			results = this->relations[type][input1->getValue()];
		}
		if (t2 == QueryInputType::ANY) { // eg. parent(2, _); uses(1, _)
			return !results.empty();
		}
		else if (t2 != QueryInputType::ANY) { // eg. follows*(1, 3); uses("main", "v")
			return (results.find(input2->getValue()) != results.end());
		}
	}
	else { // otherwise
		return false;
	}
}

unordered_set<string> PKB::getSetResultsOfRS(const RelationshipType& type,
	shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	QueryInputType t1 = input1->getQueryInputType();
	QueryInputType t2 = input2->getQueryInputType();
	unordered_set<string> ans;
	if (t1 == QueryInputType::DECLARATION) {
		shared_ptr<Declaration> d = dynamic_pointer_cast<Declaration>(input1);
		if (d->getEntityType() == EntityType::PROC) {
			if (type == RelationshipType::USES) {
				if (t2 == QueryInputType::IDENT) { // eg. uses(p, "x")
					return this->usedByProc[input2->getValue()];
				}
				else if (t2 == QueryInputType::ANY) { // eg. uses(p, _)
					return this->procUsesKeys;
				}
				else {
					return ans;
				}
			}
			else if (type == RelationshipType::MODIFIES) {
				if (t2 == QueryInputType::IDENT) { // eg. modifies(p, "x")
					return this->modifiedByProc[input2->getValue()];
				}
				else if (t2 == QueryInputType::ANY) { // eg. modifies(p, _)
					return this->procModifiesKeys;
				}
				else {
					return ans;
				}
			} // else: calls / calls*
		}
		switch (t2) {
		case QueryInputType::STMT_NUM: { // eg. parent*(s, 3)
			ans = this->relationsBy[type][input2->getValue()];
			break;
		}
		case QueryInputType::IDENT: { // eg. modifies(s, "x")
			ans = this->relationsBy[type][input2->getValue()];
			break;
		}
		case QueryInputType::ANY: { // eg. follows*(s, _)
			ans = this->relationKeys[type];
			break;
		}
		}
		filterSetOfType(d->getEntityType(), &(ans));
		return ans;
	}
	else if (t2 == QueryInputType::DECLARATION) {
		switch (t1) {
		case QueryInputType::STMT_NUM: { // eg. parent*(3, s)
			ans = this->relations[type][input1->getValue()];
			break;
		}
		case QueryInputType::IDENT: {
			if (type == RelationshipType::USES) { // eg. uses("main", v)
				ans = this->procUses[input1->getValue()];
			}
			else if (type == RelationshipType::MODIFIES) { // eg. modifies("main", v)
				ans = this->procModifies[input1->getValue()];
			}
			else { // eg. calls*("main", p)
				ans = this->relations[type][input1->getValue()];
			}
			break;
		}
		case QueryInputType::ANY: { // eg. follows*(_, s)
			ans = this->relationByKeys[type];
			break;
		}
		}
		shared_ptr<Declaration> d = dynamic_pointer_cast<Declaration>(input2);
		filterSetOfType(d->getEntityType(), &(ans));
		return ans;
	}
	else { // otherwise
		return ans;
	}
}

unordered_map<string, unordered_set<string>> PKB::getMapResultsOfRS(const RelationshipType& type,
	shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2) {
	QueryInputType t1 = input1->getQueryInputType();
	QueryInputType t2 = input2->getQueryInputType();
	unordered_map<string, unordered_set<string>> ans;
	shared_ptr<Declaration> d1 = dynamic_pointer_cast<Declaration>(input1);
	shared_ptr<Declaration> d2 = dynamic_pointer_cast<Declaration>(input2);
	if (d1->getEntityType() == EntityType::PROC &&
		d2->getEntityType() == EntityType::VAR) {
		if (type == RelationshipType::USES) { // eg. uses(p, v)
			return this->procUses;
		}
		else if (type == RelationshipType::MODIFIES) { // eg. modifies(p, v)
			return this->procModifies;
		}
		else { // otherwise
			return ans;
		}
	}
	// eg. follows*(s1, s2)
	ans = this->relations[type];
	if (type != AFFECTS && type != AFFECTSBIP
		&& type != AFFECTS_T && type != AFFECTSBIP_T) {
		filterMapOfType(d1->getEntityType(), d2->getEntityType(), &ans);
	}
	string n1 = d1->getValue();
	string n2 = d2->getValue();
	if (n1 == n2) { // eg. affects(s, s)
		unordered_map<string, unordered_set<string>> res;
		for (auto pair : ans) {
			if (pair.second.find(pair.first) != pair.second.end()) {
				res[pair.first].insert(pair.first);
			}
		}
		return res;
	}
	else {
		return ans;
	}
}

// retrieval methods for pattern clauses

unordered_set<string> PKB::getSetResultsOfAssignPattern(
	shared_ptr<QueryInput> input, Expression& exp) {
	unordered_set<string> res = this->expressions[exp];
	unordered_set<string> ans;
	if (exp.getType() == ExpressionType::EMPTY) { // any expression
		res = this->getEntities(EntityType::ASSIGN);
	}
	switch (input->getQueryInputType()) {
	case QueryInputType::ANY: { // eg. pattern a(_, _"x"_)
		ans = res;
		break;
	}
	case QueryInputType::IDENT: { // eg. pattern a("x", _"x"_)
		unordered_set<string> mod = this->relationsBy[MODIFIES][input->getValue()];
		for (string x : res) {
			if (mod.find(x) != mod.end()) {
				ans.insert(x);
			}
		}
		break;
	}
	default: { } // STMT_NUM
	}

	return ans;
}

unordered_map<string, unordered_set<string>> PKB::getMapResultsOfAssignPattern(
	shared_ptr<QueryInput> input, Expression& exp) {
	unordered_set<string> res = this->expressions[exp];
	unordered_map<string, unordered_set<string>> ans;
	if (exp.getType() == ExpressionType::EMPTY) { // any expression
		res = this->getEntities(EntityType::ASSIGN);
	}
	// eg. pattern a(v, _"x"_)
	for (string s : res) {
		ans[s] = this->relations[MODIFIES][s];
	}
	return ans;
}

unordered_set<string> PKB::getSetResultsOfContainerPattern(
	const EntityType& type, shared_ptr<QueryInput> input) {
	int t = type == EntityType::IF ? 0 : type == EntityType::WHILE ? 1 : -1;
	if (t == -1) {
		return unordered_set<string>();
	}
	switch (input->getQueryInputType()) {
	case QueryInputType::ANY: { // eg. pattern w(_, _)
		return this->contPatternKeys[t];
	}
	case QueryInputType::IDENT: { // eg. pattern w("x", _)
		return this->contPatternBy[t][input->getValue()];
	}
	default: { } // STMT_NUM
	}
}

unordered_map<string, unordered_set<string>> PKB::getMapResultsOfContainerPattern(
	const EntityType& type, shared_ptr<QueryInput> input) {
	int t = type == EntityType::IF ? 0 : type == EntityType::WHILE ? 1 : -1;
	if (t == -1) {
		return unordered_map<string, unordered_set<string>>();
	}
	else {
		return this->contPattern[t];
	}
}

// retrieve methods for with clauses

unordered_map<string, unordered_set<string>> PKB::getDeclarationsMatchResults(
	shared_ptr<Declaration> left, shared_ptr<Declaration> right) {
	EntityType t1 = left->getEntityType();
	EntityType t2 = right->getEntityType();
	if ((t1 == EntityType::PROC && t2 == EntityType::VAR) || // eg. p.procName = v.varName
		(t2 == EntityType::PROC && t1 == EntityType::VAR)) { // eg. v.varName = p.procName
		return this->procVarName;
	}
	else if (t1 == EntityType::CONST) { // eg. c.value = s.stmt#
		return this->constStmtNum[t2];
	}
	else if (t2 == EntityType::CONST) { // eg. s.stmt# = c.value 
		return this->constStmtNum[t1];
	}
	else { // otherwise
		return unordered_map<string, unordered_set<string>>();
	}
}

unordered_map<string, unordered_set<string>> PKB::getDeclarationMatchAttributeResults(
	shared_ptr<Declaration> declaration, EntityType type) {
	EntityType td = declaration->getEntityType();
	unordered_map<string, unordered_set<string>> ans;
	int t = type == EntityType::CALL ? 0 : type == EntityType::PRINT ? 1 : type == EntityType::READ ? 2 : -1;
	if (t == -1) {
		return ans;
	}
	else if (td != EntityType::PROC && td != EntityType::VAR) {
		return ans;
	}
	else if (t != 0 && td == EntityType::VAR) { // eg. v = print.varName
		return this->nameUsedBy[t];
	}
	else if (t == 0 && td == EntityType::PROC) { // eg. p = call.procName
		return this->nameUsedBy[t];
	}
	else { // eg. v = call.procName or p = read.varName
		for (string name : this->entities[td]) {
			if (!this->nameUsedBy[t][name].empty()) {
				ans[name] = this->nameUsedBy[t][name];
			}
		}
		return ans;
	}
}

unordered_map<string, unordered_set<string>> PKB::getAttributesMatchResults(
	EntityType leftEntityType, EntityType rightEntityType) {
	int t = rightEntityType == EntityType::CALL ? 0 
		: rightEntityType == EntityType::PRINT ? 1 
		: rightEntityType == EntityType::READ ? 2 : -1;
	unordered_map<string, unordered_set<string>> ans;
	if (t == -1) {
		return ans;
	}
	for (string s : this->entities[leftEntityType]) {
		string name = this->nameUsed[s];
		if (!this->nameUsedBy[t][name].empty()) {
			ans[s] = this->nameUsedBy[t][name];
		}
	}
	return ans;
}

unordered_set<string> PKB::getAttributeMatchNameResults(EntityType type, shared_ptr<Ident> name) {
	int t = type == EntityType::CALL ? 0 : type == EntityType::PRINT ? 1 : type == EntityType::READ ? 2 : -1;
	if (t == -1) {
		return unordered_set<string>();
	}
	else {
		return this->nameUsedBy[t][name->getValue()];
	}
}

string PKB::getNameFromStmtNum(string stmtNum) {
	return this->nameUsed[stmtNum];
}

// filtering result

void PKB::filterSetOfType(const EntityType& type, unordered_set<string>* res) {
	if (type == EntityType::VAR || type == EntityType::PROGLINE ||
		type == EntityType::STMT || type == EntityType::PROC) {
		return;
	}
	unordered_set<string> ans = *res;
	for (auto& x : *res) {
		if (this->types[x] != type) {
			ans.erase(x);
		}
	}
	*res = ans;
}

void PKB::filterMapOfType(
	const EntityType& t1, const EntityType& t2,
	unordered_map<string, unordered_set<string>>* res) {
	if (t1 == EntityType::STMT || t1 == EntityType::PROC || t1 == EntityType::PROGLINE) {
		for (auto& x : *res) filterSetOfType(t2, &((*res)[x.first]));
	}
	else if (t2 == EntityType::VAR || t2 == EntityType::CONST ||
		t2 == EntityType::STMT || t2 == EntityType::PROC) {
		unordered_map<string, unordered_set<string>> ans = *res;
		for (auto& x : *res) {
			if (this->types[x.first] != t1) {
				ans.erase(x.first);
			}
		}
		*res = ans;
	}
	else {
		unordered_map<string, unordered_set<string>> ans = *res;
		for (auto& x : *res) {
			if (this->types[x.first] != t1) {
				ans.erase(x.first);
			}
			else {
				filterSetOfType(t2, &ans[x.first]);
			}
		}
		*res = ans;
	}

}