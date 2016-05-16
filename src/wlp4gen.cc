#include "parseTree.h"
#include <algorithm>
#include <set>
#include <fstream>
#include <stack>
#include <vector>
#include <string>
#include <iostream>
#include <list>
#include <sstream>
#include <stack>
#include <map>

using namespace std;

//bool error;
//MegaTable *tables;
//std::map<std::string, Symbol> *symbolTable;
//std::map<std::string, Procedure> pros;
//int proCount;


int main(){
	
	error = false;
//counter < tables->size()-1) 
string temp[]= { "BOF", "BECOMES", "COMMA", "ELSE", "EOF", "EQ", "GE", "GT",
	          "ID", "IF", "INT", "LBRACE", "LE", "LPAREN", "LT", "MINUS",
                 "NE", "NUM", "PCT", "PLUS", "PRINTLN", "RBRACE", "RETURN",
                  "RPAREN", "SEMI", "SLASH", "STAR", "WAIN", "WHILE", "AMP",
                  "LBRACK", "RBRACK", "NEW", "DELETE", "NULL"};

	set<string> tempSet(temp, temp + (sizeof(temp) / sizeof(temp[0])));
	
	proCount = 0;

	terms = tempSet;

	symbolTable = new map<string, Symbol>;

	tables = new MegaTable;
	(*tables)["procedures"] = new map<string, Symbol>;
	
	TreeNode *root = new TreeNode();
//	root->printHi();
	//root->testTraverse(0);
	try{
		root->makeSymbolTable("noPrint");
		root->searchUndeclaredSymbols("noPrint");	
		root->checkArgslist();
	//	cout << "checking wellTyped() "<< endl;
		string tempString = root->checkWellTyped("");
	 	printTables();
		
		printSymbolTables();


		root->assembleMips("");

//		cerr << endl << "printing procedures " << endl;
//	for (map<string, Procedure>::iterator it = pros.begin(); it != pros.end(); ++it){
//			cerr << it->first << " " << it->second.order << endl;
//		}
	} catch(string e){
		cerr << e << endl;
	} catch(...){
		cerr << "ERROR: catch all case";
	}
}
