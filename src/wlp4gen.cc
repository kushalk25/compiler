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

int main(){
	
	// bool error and other variables are set in parseTree.h
	error = false;

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

	// compilation happens here
	try{
		root->makeSymbolTable("noPrint");
		root->searchUndeclaredSymbols("noPrint");	
		root->checkArgslist();
		string tempString = root->checkWellTyped("");
	 	printTables();
		printSymbolTables();
		root->assembleMips("");
	} catch(string e){
		cerr << e << endl;
	} catch(...){
		cerr << "ERROR: catch all case";
	}
}
