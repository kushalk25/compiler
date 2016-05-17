#ifndef __PARSETREE_H__
#define __PARSETREE_H__
#include <set>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <list>
#include <sstream>
#include <map>
#include <stack>
extern std::set<std::string> terms;
extern bool error;
extern int offset;
extern int loopCounter;


struct Rule {
	std::string rName;
	int rNum;
};

struct Symbol{
	std::string type;
	std::string name;
	int offset;

	Symbol();
	Symbol(std::string t, std::string n);
	void printSymbol();

};

typedef std::map<std::string, Symbol> SymbolTable;
typedef std::map<std::string, SymbolTable*> MegaTable;
 extern MegaTable *tables;

 extern std::map<std::string, Symbol> *symbolTable;

struct Procedure;

 extern std::map<std::string, Procedure> pros;
struct Procedure{
	std::string name;
	int order; 
	int params;
	Procedure(std::string name, int o, int p);
	bool isOrdered(std::string sig);
};
void storeIdVal(std::string sig, std::string id);
void storeNumVal(std::string sig, std::string num);
std::string compAdd(std::string lType, std::string rType);
std::string compSub(std::string lType, std::string rType);
std::string compMDP(std::string lType, std::string rType);
std::string compAmp(std::string childType);
std::string compStar(std::string childType);
 extern int proCount;

void pushRegs();
void popRegs();

struct TreeNode{

	std::string lhs;
	std::string rhs;
	TreeNode **children;
	int size;


	TreeNode();
	TreeNode(std::string lhs, std::string rhs, int size);
	TreeNode(std::string lhs, std::string rhs, int size, TreeNode **c);
	void makeSymbolTable(std::string sig);
	std::string checkWellTyped(std::string sig);

	bool isInt();
	bool isIntStar();
	std::string getType();
	std::string getSigMain();
	std::string getSig();
	// takes in a procedure node
	std::string getParams();
	int getNumParams();
	void getSymbols(std::string sig);

	void storeProcedure(std::string procedureName);
	void searchUndeclaredSymbols(std::string sig);
	void checkArgslist();
	int getArgsNum();
	void testTraverse(int s);
	void postOrderTraversal();


	// a9 additions:

	void assembleMips(std::string sig);
	void assembleMain(std::string sig);
	void computeExpr(std::string sig);
	void computeTerm(std::string sig);
	void computeFactor(std::string sig);
	void storeMainParams(std::string sig);
	void computeChildrenExpr(std::string sig);
	void computeChildrenTerm(std::string sig);
	void assembleStatements(std::string sig);
	void computeStatement(std::string sig);
	void assembleDcls(std::string sig);
	std::string lValGetId(std::string sig);
	void computeTest(std::string sig);
	void computeFactorAmp(std::string sig);
	void computeAdd(std::string sig);
	void computeSub(std::string sig);
	void callProcedure(std::string sig);
	void assembleProcedures(std::string sig);
	void assembleProc(std::string sig);
	void findMain(std::string sig);
	void pushArgs(std::string sig);
	void popArgs(std::string sig);
	void storeParams(std::string sig);

};
void printSymbolTables();
void printTables();
#endif
