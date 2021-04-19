#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "PKBInterface.h"
#include "EntityType.h"
#include "RelationshipType.h"
#include "QueryInput.h"
#include "Declaration.h"
#include "Expression.h"
#include "Ident.h"

using namespace std;

struct EnumClassHash {
	template <typename T>
	std::size_t operator()(T t) const
	{
		return static_cast<std::size_t>(t);
	}
};

struct KeyHasher {
	std::size_t operator()(const Expression& e) const {
		using std::size_t;
		using std::hash;
		using std::string;

		return ((hash<string>()(e.getValue())
			^ (hash<ExpressionType>()(e.getType()) << 1)) >> 1);
	}
};

class PKB : public PKBInterface {

public:
	/**
	* Constructor for PKB
	*
	* @param number the number of statements in the program
	*/
	PKB(const int& n);

	/**
	* Initializes PKB after being populated
	*/
	void init();

	/**
	* Sets the type of an indexed statement in PKB
	*
	* @param index
	* @param type
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool setStatementType(const int& index, const EntityType& type);

	/**
	* Inserts a parent relationship into PKB
	*
	* @param parent the index of the parent statement
	* @param child the index of the child statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertParent(const int& parent, const int& child);

	bool insertParentStar(const int& parent, const int& child);

	/**
	* Inserts a follow relationship into PKB
	*
	* @param former the index of the statement being followed
	* @param latter the index of the statement following
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertFollow(const int& former, const int& latter);

	bool insertFollowStar(const int& former, const int& latter);

	/**
	* Inserts a next relationship into PKB
	*
	* @param former the index of the statement before
	* @param latter the index of the statement after
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertNext(const int& former, const int& latter);

	bool insertNextStar(const int& former, const int& latter);

	/**
	* Inserts a next_bip relationship into PKB
	*
	* @param former the index of the statement before
	* @param latter the index of the statement after
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertNextBip(const int& former, const int& latter);

	bool insertNextBipStar(const int& former, const int& latter);

	/**
	* Inserts an affect relationship into PKB
	*
	* @param former the index of the statement affecting
	* @param latter the index of the statement affected
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertAffect(const int& former, const int& latter);

	bool insertAffectStar(const int& former, const int& latter);

	/**
	* Inserts an affect_bip relationship into PKB
	*
	* @param former the index of the statement before
	* @param latter the index of the statement after
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertAffectBip(const int& former, const int& latter);

	bool insertAffectBipStar(const int& former, const int& latter);

	/**
	* Insert a set of variables used by a statement into PKB
	*
	* @param index the index of the given statement
	* @param variables the name of the variable being used by the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertUses(const int& index, const string& variable);

	bool insertProcUses(const string& procedure, const string& variable);

	/**
	* Inserts a set of variables modified by a statement into PKB
	*
	* @param index the index of the given statement
	* @param variables the name of the variable being modified by the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertModifies(const int& index, const string& variable);

	bool insertProcModifies(const string& procedure, const string& variable);

	/**
	* Inserts a calls relationship into PKB
	*
	* @param caller name of the procedure that calls
	* @param callee name of the procedure being called
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertCalls(const string& caller, const string& callee);

	bool insertCallsStar(const string& caller, const string& callee);

	/**
	* Inserts an expression of an assign statement into PKB
	*
	* @param index the index of the given statement
	* @param expression hashed value of the RHS of the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool insertExpression(const int& index, const Expression& expression);

	/**
	* Inserts a control variable of a container statement into PKB.
	* The specified statement should have been set as IF or WHILE beforehand.
	*
	* @param index the index of the given statement
	* @param varaible control variable of the statement
	*
	* @return a boolean indicating whether the operation is successful
	*/
	bool setControlVariable(const int& index, const string& variable);

	/**
	* Inserts a variable name into PKB
	*/
	bool insertVariable(const string& variable);

	bool insertConst(const string& number);

	bool insertProcedure(const string& proc);

	/**
	* Inserts a name used in a statement, varName for READ/PRINT and procName for CALL
	*/
	bool insertUsedName(const int& index, const string& name);

	/**
	* Retrieves indices of all statements of some type as string
	*
	* @param type
	*
	* @return unordered_set of statement indices as string
	*/
	unordered_set<string> getEntities(const EntityType& type);

	/**
	* Retrieves a boolean result of a given relationship query
	*
	* @param type
	* @param input1 the first input of relationship
	* @param input2 the second input of relationship
	*
	* @return a boolean indicating whether the relationship is true.
	* if there is any Declaration in inputs, returns false
	*/
	bool getBooleanResultOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2);

	/**
	* Retrieves results of a given relationship query
	*
	* @param type
	* @param input1 the first input of relationship
	* @param input2 the second input of relationship
	*
	* @return unordered_set of results there is exactly one Declaration
	*/
	unordered_set<string> getSetResultsOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2);

	/**
	* Retrieves results of a given relationship query
	*
	* @param type
	* @param input1 the first input of relationship
	* @param input2 the second input of relationship
	*
	* @return unordered_set of results there are exactly two Declarations
	*
	*/
	unordered_map<string, unordered_set<string>> getMapResultsOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2);


	/**
	* Retrieves results of an assign pattern query
	*
	* @param type the type of statements being queried
	* @param input the LHS of a pattern clause
	* @param expression the RHS of a pattern clause
	*
	* @return a unordered_set of indices if input is Any or Ident
	*
	*/
	unordered_set<string> getSetResultsOfAssignPattern(
		shared_ptr<QueryInput> input, Expression& expression);

	/**
	* Retrieves results of a pattern query
	*
	* @param type the type of statements being queried
	* @param input the LHS of a pattern clause
	* @param expression the RHS of a pattern clause
	*
	* @return a map of results (stmt_index, set<input_value>) if input is a Declaration
	*/
	unordered_map<string, unordered_set<string>> getMapResultsOfAssignPattern(
		shared_ptr<QueryInput> input, Expression& expression);

	unordered_set<string> getSetResultsOfContainerPattern(const EntityType& type, shared_ptr<QueryInput> input);

	unordered_map<string, unordered_set<string>> getMapResultsOfContainerPattern(const EntityType& type, shared_ptr<QueryInput> input);

	/**
	* Retrieves results of a with query
	*
	* @param left the first parameter of with clause
	* @param right the second parameter of with clause
	*
	* @return a map of results (left_value, unordered_set<right_value>)
	*/
	unordered_map<string, unordered_set<string>> getDeclarationsMatchResults(
		shared_ptr<Declaration> left, shared_ptr<Declaration> right);

	unordered_map<string, unordered_set<string>> getDeclarationMatchAttributeResults(
		shared_ptr<Declaration> declaration, EntityType type);

	unordered_map<string, unordered_set<string>> getAttributesMatchResults(
		EntityType leftEntityType, EntityType rightEntityType);

	unordered_set<string> getAttributeMatchNameResults(
		EntityType type, shared_ptr<Ident> name);

	/**
	* Retrieves the name directly used by a statement
	*
	* @param stmtNum the index of specified statement
	*
	* @return a string of variable name for PRINT/READ or procedure name for CALL
	*
	*/
	string getNameFromStmtNum(string stmtNum);


private:

	const int number; // number of statement

	unordered_map<string, EntityType> types; // map from index to type

	unordered_map<EntityType, unordered_set<string>, EnumClassHash> entities; // map from type to indices

	unordered_map<string, unordered_set<string>> relations[17]; // relationship maps

	unordered_map<string, unordered_set<string>> relationsBy[17]; // by-relationship maps

	unordered_set<string> relationKeys[17]; // key unordered_set for relationships

	unordered_set<string> relationByKeys[17]; // key unordered_set for by-relationships 

	// procedure uses / modifies

	unordered_map<string, unordered_set<string>> procUses, procModifies;

	unordered_map<string, unordered_set<string>> usedByProc, modifiedByProc;

	unordered_set<string> procUsesKeys, procModifiesKeys;

	unordered_set<string> usedByProcKeys, modByProcKeys;

	// container pattern

	unordered_map<string, unordered_set<string>> contPattern[2]; // map from index -> var

	unordered_map<string, unordered_set<string>> contPatternBy[2]; // map from var -> index

	unordered_set<string> contPatternKeys[2]; // key unordered_set for contPattern

	// assign pattern

	unordered_map<Expression, unordered_set<string>, KeyHasher> expressions; // map from expression to indices

	// with clause

	unordered_map<string, unordered_set<string>> procVarName; // identical procedure and variable names

	unordered_map<EntityType, unordered_map<string, unordered_set<string>>, EnumClassHash> 
		constStmtNum; // identical constant values and stmt#
	
	unordered_map<string, string> nameUsed; // varName for PRINT/READ and procName for CALL 
	
	unordered_map<string, unordered_set<string>> nameUsedBy[3]; // set of stmts using a name 

	// utility methods

	bool insertRelationship(const RelationshipType& type, const string& e1, const string& e2);

	void filterSetOfType(const EntityType& type, unordered_set<string>* res);

	void filterMapOfType(
		const EntityType& t1, const EntityType& t2,
		unordered_map<string, unordered_set<string>>* res);
};
