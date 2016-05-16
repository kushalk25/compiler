#include "parseTree.h"

using namespace std;
bool error = true;
set<string> terms = set<string>();
MegaTable *tables = new MegaTable;
map<string, Symbol> *symbolTable = new map<string, Symbol>;
map<string, Procedure> pros = map<string, Procedure>();
int proCount = 0;


Symbol::Symbol(string t, string n){
    this->type = t;
    this->name = n;
}
Symbol::Symbol(){
    this->type = "";
    this->name = "";
}
// will only be used for the else case
void Symbol::printSymbol(){
    if(this->type == "procedure"){
        cerr  << this->name << endl;
    } else {
        cerr << this->name << " " << this->type << endl;
    }
}

Procedure::Procedure(string name = "", int o=0, int p=0){
    this->name = name;
    this->order = o;
    this->params = p;
}
bool Procedure::isOrdered(string sig){
    string proName;
    istringstream iss(sig);
    iss >> proName;     

    return pros[proName].order >= this->order;
}

// the following functions are to compare types
string compAdd(string lType, string rType){
    if(lType == "int" && rType == "int"){
        return "int";
    }
    if(lType == "int*" && rType == "int"){
        return "int*";
    }
    if(lType == "int" && rType == "int*"){
        return "int*";
    }
    throw "ERROR: addition with " + lType +" and " + rType;
}
string compSub(string lType, string rType){
    if(lType == "int" && rType == "int"){
        return "int";
    }
    if(lType == "int*" && rType == "int"){
        return "int*";
    }
    if(lType == "int*" && rType == "int*"){
        return "int";
    }
    throw "ERROR: subtraction with " + lType+ " and " + rType;
}
string compMDP(string lType, string rType){

    if(lType == "int" && rType == "int"){
        return "int";
    }
    throw "ERROR: M/D/P with " + lType +" and " + rType;
}

string compAmp(string childType){
    if(childType == "int"){
        return "int*";
    } else {
        throw "ERROR: amp incorrect, given " +childType;
    }
}
string compStar(string childType){
    if(childType == "int*"){
        return "int";
    } else {
        throw "ERROR: star incorrect"+childType;
    }
}


TreeNode::TreeNode(){
    string input;
    getline(cin, input);
    istringstream iss(input); 
    iss >> this->lhs;
    int rhsSize = 0;
    string temp;
    // read rhs into rhs string and count spaces
	while(iss >> temp){
        rhsSize++;
        this->rhs = this->rhs + " " + temp;
    }
    //this gets rid of space in the begining
    if(this->rhs.length() > 0) this->rhs = this->rhs.substr(1);     

	// if non-terminal create childrens' array
    if(!terms.count(this->lhs)){
        this->size = rhsSize;
        this->children = new TreeNode *[this->size];
        for(int i = 0; i<this->size; i++){
            this->children[i] = new TreeNode();
        }
    } else {
        this->size = 0;
        this->children = NULL;
    }
}

    TreeNode::TreeNode(string lhs, string rhs, int size){

        this->lhs = lhs;
        this->rhs = rhs;
        this->size = size;
        children = new TreeNode *[size];
    }

    TreeNode::TreeNode(string lhs, string rhs, int size, TreeNode **c){

        this->lhs = lhs;
        this->rhs = rhs;
        this->size = size;
        children = new TreeNode *[size];
        
        for(int i = 0; i<size; i++){
            children[i] = c[i];
        }
        

    }

    void TreeNode::makeSymbolTable(string sig){
//      cerr << "making symbole table " << this->lhs << endl;
		// if we encounter a procedure make room for it in our MegaTable
    //    if(this->lhs == "procedure"){
        if(this->lhs == "procedure"){
//          cerr << "getting sig" << endl;  
            string procedureName = "F" + this->children[1]->rhs; 
            if(pros.count(procedureName)){
                throw "ERROR: duplicate declaration of " + procedureName;
            }       
            sig = this->getSig();       
            (*tables)[sig] = new map<string, Symbol>;
            this->storeProcedure(procedureName);
        }
        if(this->lhs == "main"){
            sig = "wain " + this->getSigMain();
//          cerr << "new table made with sig: " << sig << endl;
            (*tables)[sig] = new map<string, Symbol>;
        }

		// if we get a declaration, add it to the symbolTable
        if(this->lhs == "dcl"){
            this->getSymbols(sig);
        }else {
            for(int i = 0; i<this->size; i++){
                this->children[i]->makeSymbolTable(sig);
            }
        }
    }

	// given any node check if it AND its children are well-typed
    string TreeNode::checkWellTyped(string sig){
        string result;
        if(this->lhs == "procedure"){
            sig = this->getSig();       
            string returnType = this->children[9]->checkWellTyped(sig);
            if(returnType != "int"){
                throw "ERROR: returning " + returnType;
            }
        }
        if(this->lhs == "main"){
            sig = "wain " + this->getSigMain();
            string param2 = this->children[5]->children[0]->getType();
            if(param2 != "int"){
                throw "ERROR second param of wain is " + param2;
            }
            string returnType = this->children[11]->checkWellTyped(sig);
            if(returnType != "int"){
                throw "ERROR: returning " + returnType;
            }
        }

        if(this->lhs == "dcls" && this->size ==5){
            string dclType = this->children[1]->children[0]->getType();
            if(dclType == "int" && this->children[3]->lhs !="NUM"){
                throw "ERROR can't declare "+dclType+" as NUM";
            } else if(dclType == "int*" && this->children[3]->lhs !="NULL"){
                throw "ERROR can't declare "+dclType+" as NULL";
            }
        }

        if(this->lhs == "test"){
            string lType = this->children[0]->checkWellTyped(sig);
            string rType = this->children[2]->checkWellTyped(sig);
            if(lType != rType){
                throw "ERROR: types don't match for test: " + lType + " " +rType;
            }
        }
        if(this->lhs=="statement"){
            if(this->children[0]->lhs=="PRINTLN"){
                string childExpr = this->children[2]->checkWellTyped(sig);
                if(childExpr != "int"){
                    throw "ERROR: trying to println " + childExpr;
                }
            } else if(this->children[0]->lhs=="DELETE"){
                string childExpr = this->children[3]->checkWellTyped(sig);
                if(childExpr != "int*"){
                    throw "ERROR: trying to delete " + childExpr;
                }
            } else if(this->children[0]->lhs == "lvalue"){
                string leftVal = this->children[0]->checkWellTyped(sig);
                string childExpr = this->children[2]->checkWellTyped(sig);
                if(leftVal != childExpr){
                    throw "ERROR: assigning " +leftVal+ " "+childExpr;
                }
            }
        }

        if(this->lhs == "expr"){
            if(this->size == 1){
                return this->children[0]->checkWellTyped(sig);
            } else {
                string lType = this->children[0]->checkWellTyped(sig);
                string rType = this->children[2]->checkWellTyped(sig);
                if(this->children[1]->lhs=="PLUS") {
                    return compAdd(lType, rType);
                }
                if(this->children[1]->lhs=="MINUS") {
                    return compSub(lType, rType);
                }
            }
        } else if(this->lhs == "term"){
            if(this->size == 1){
                return this->children[0]->checkWellTyped(sig);
            } else {
                string lType = this->children[0]->checkWellTyped(sig);
                string rType = this->children[2]->checkWellTyped(sig);
                if(this->children[1]->lhs=="STAR") {
                    return compMDP(lType, rType);
                }
                if(this->children[1]->lhs=="SLASH") {
                    return compMDP(lType, rType);
                }
                if(this->children[1]->lhs=="PCT") {
                    return compMDP(lType, rType);
                }
            }
            
	} else if(this->lhs == "factor"){
            if(this->size == 1){
                if(this->children[0]->lhs=="ID") {
                    string name = this->children[0]->rhs;
                    string idType = (*(*tables)[sig])[name].type;
                    return idType;
                }
                if(this->children[0]->lhs=="NUM") {
                    return "int";
                }
                if(this->children[0]->lhs=="NULL") {
                    return "int*";
                }
            } else if(this->size == 3){ 
                if(this->children[0]->lhs == "ID"){
                    return "int";
                } else {
                    return this->children[1]->checkWellTyped(sig);
                }
            } else if(this->size == 4){ 
                if(this->children[0]->lhs == "ID"){
                    return "int";
                }
            } else if(this->size == 5){
                string childExpr = this->children[3]->checkWellTyped(sig);
                return compAmp(childExpr);
            }
            // for AMP and STAR
            if(this->children[0]->lhs == "AMP"){
                string childType = this->children[1]->checkWellTyped(sig);
                return compAmp(childType);
            }else if(this->children[0]->lhs == "STAR"){
                string childType = this->children[1]->checkWellTyped(sig);
                return compStar(childType);
            }
        } else if(this->lhs=="lvalue"){
            if(this->size == 1){    
                if(this->children[0]->lhs=="ID") {
                    string name = this->children[0]->rhs;
                    string idType = (*(*tables)[sig])[name].type;
                    return idType;
                }
            } else if(this->size == 2){
                string childType = this->children[1]->checkWellTyped(sig);
                return compStar(childType);
            } 
            return this->children[1]->checkWellTyped(sig);
        }
            for(int i = 0; i<this->size; i++){
                this->children[i]->checkWellTyped(sig);
            }
        return "";
    }



    bool TreeNode::isInt(){
        return ("int" == this->children[0]->getType());
    }
    bool TreeNode::isIntStar(){
        return ("int*" == this->children[0]->getType());
    }

    string TreeNode::getType(){
        string type = "";
        for(int i = 0; i < this->size; i++){
            type = type + this->children[i]->rhs;
        }
        return type;
    }

    string TreeNode::getSigMain(){
        string signature = this->children[3]->children[0]->getType();
        signature = signature + " " + this->children[5]->children[0]->getType();
        return signature;
    }

    string TreeNode::getSig(){
        // within procedure node, children[1] is the ID
        string signature = "F" + this->children[1]->rhs;

        if(this->children[3]->size > 0){
            // children[3]->children[0] is the pamalist of the params for the procedure
            signature = signature + " " + this->children[3]->children[0]->getParams();
        } 
        //signature = signature + " " + this->children[5]->children[0]->getType();
        return signature;
    }

    // takes in a procedure node
    string TreeNode::getParams(){

        
        //cerr << endl << "node info: " << this->lhs << " " << this->rhs << endl;

        string signature = this->children[0]->children[0]->getType();
        //cerr << "in first sig def: " << signature << endl; 
//      cerr << "searching paramlist " << endl;
        if(this->size > 2){
            //cerr << "in secondary sig def: " << this->children[2]->lhs << this->children[2]->rhs << endl;
            signature = signature + " " + this->children[2]->getParams();
            //cerr << "the sig is: " << signature << endl;  
        }
        return signature;

    }

	// given a paramlist return the number of params
    int TreeNode::getNumParams(){
        if(this->size > 2){
            return 1 + this->children[2]->getNumParams();
        }
        return 1;
    }

	// given a dcl node, this code will get the id and type of the node
	// and stores it into the symbol table with key sig
    void TreeNode::getSymbols(string sig){
        string type = this->children[0]->getType();
        string name = this->children[1]->rhs;
        if((*tables)[sig]->count(name)){
            throw "ERROR: duplicate declaration of " + name;
        } else {
            (*(*tables)[sig])[name] = Symbol(type, name);
        }
    }
	
	// given a procedure node, this will store it's corresponding procedure object
	// in the procedures map
    void TreeNode::storeProcedure(string procedureName){
        int numParams;
        if(this->children[3]->size == 0){
            numParams = 0;
        } else {
            numParams = this->children[3]->children[0]->getNumParams();
        }
        pros[procedureName] = Procedure(procedureName, proCount, numParams);
        proCount++;
    }	

	// goes through the tree and ouputs undeclared symbols
    void TreeNode::searchUndeclaredSymbols(string sig){
    
        if(this->lhs == "main"){

            sig = "wain " + this->getSigMain();
        }   
        if(this->lhs == "procedure"){
            sig = this->getSig();
        }   

        if(this->lhs == "ID"){
            string name = this->rhs;
	    string proName = "F" + name;
            if(sig.substr(0, 4) == "wain"){
                if( !((*tables)[sig]->count(name) || pros.count(proName))){
                    throw "ERROR: undeclared variable a " + name;
                }
            } else if(!((*tables)[sig]->count(name) || (pros.count(proName) && pros[proName].isOrdered(sig) ))){
                throw "ERROR: undeclared variable b " + name;
            }
        }

        for(int i = 0; i<this->size; i++){
            this->children[i]->searchUndeclaredSymbols(sig);
        }
        
    }
    void TreeNode::checkArgslist(){
//      cerr << "making symbole table " << this->lhs << endl;
        string factorName;
        if(this->lhs == "factor" && this->size == 4){
            int numArgs = this->children[2]->getArgsNum();
            factorName = "F" + this->children[0]->rhs;
            if (pros[factorName].params != numArgs){
                throw "ERROR: function " + factorName + " has incorrect number of args";
            }
        } else if(this->lhs == "factor" && this->size == 3 && this->children[0]->lhs == "ID"){
            factorName = this->children[0]->rhs;
            if (pros[factorName].params != 0){
                cerr << " params: " << pros[factorName].params << endl;
                throw "ERROR: function " + factorName + " has incorrect more than 0 args";
            }
        }
        
        for(int i = 0; i<this->size; i++){
            this->children[i]->checkArgslist();
        }
        
    }

    int TreeNode::getArgsNum(){
        if(this->size == 3){
            return 1 + this->children[2]->getArgsNum();
        }
        return 1;
    }   

    void TreeNode::testTraverse(int s){
        for(int i = 0; i < s; i++){
            cerr << " ";
        }
        cerr << this->lhs << this->rhs << endl;
        s++;
        for(int i = 0; i<this->size; i++){
            this->children[i]->testTraverse(s);
        }
    }

    void TreeNode::postOrderTraversal(){
        if(this->size > 0){
            for(int i = 0; i<this->size; i++){
                if(this->children[i] != NULL){
                this->children[i]->postOrderTraversal();
                }
            }
        }
        cerr << this->lhs << " " << this->rhs << endl;
    }

void printSymbolTables(){
//  cerr << "map size " << symbolTable->size() << endl;

    for (map<string, Symbol>::iterator it = symbolTable->begin(); it != symbolTable->end(); ++it) {
//  cerr << "in map loop" << endl;
    it->second.printSymbol();
    }
}

void printTables(){
//  cerr << "map size " << symbolTable->size() << endl;

    int counter = 0;

    for (MegaTable::iterator it = tables->begin(); it != tables->end(); ++it){

        if(it->first != "procedures"){
    //      cerr << it->first << endl;
    //  } else {
//else if(it->first.substr(0,4) == "wain"){
            cerr << it->first << endl;
            for(SymbolTable::iterator it2 = it->second->begin(); it2 != it->second->end(); ++it2){
//              cerr << "  ";       
//              cerr << "sig: " << it->first << " symbol: ";
                 it2->second.printSymbol(); 
            }
        }
        
        if(it->first != "procedures" && counter < tables->size()-1){
            cerr << endl;
        }
        counter++;
    }
    
}
