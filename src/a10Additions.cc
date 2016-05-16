#include "parseTree.h"

using namespace std;

void TreeNode::computeAdd(string sig){
	string exprType = this->children[0]->checkWellTyped(sig);
	string termType = this->children[2]->checkWellTyped(sig);

	cout << "; EXPRTYPE: " << exprType;
	cout << " And TermType: " << termType << endl;

	if(exprType == "int" && termType == "int"){
		cout << "add $3, $5, $3" << endl;
	} else if(exprType == "int*" && termType == "int"){
		cout << "mult $3, $4" << endl;
		cout << "mflo $3" << endl;
		cout << "add $3, $5, $3" << endl;
	} else if(exprType == "int" && termType == "int*"){
		cout << "mult $5, $4" << endl;
		cout << "mflo $5" << endl;
		cout << "add $3, $5, $3" << endl;
	}

}

void TreeNode::computeSub(string sig){
	string exprType = this->children[0]->checkWellTyped(sig);
	string termType = this->children[2]->checkWellTyped(sig);

	cout << "; EXPRTYPE: " << exprType;
	cout << " And TermType: " << termType << endl;

	if(exprType == "int" && termType == "int"){
		cout << "sub $3, $5, $3" << endl;
	} else if(exprType == "int*" && termType == "int"){
		cout << "mult $3, $4" << endl;
		cout << "mflo $3" << endl;
		cout << "sub $3, $5, $3" << endl;
	} else if(exprType == "int*" && termType == "int*"){
		cout << "sub $3, $5, $3" << endl;
		cout << "div $3, $4" << endl;
		cout << "mflo $3" << endl;
	}

}

void TreeNode::callProcedure(string sig){

	cout << "sw $29, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;
	cout << "sw $31, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;

	if(this->children[2]->lhs == "arglist"){
		this->children[2]->pushArgs(sig);		
	}
	cout << "lis $5" << endl;
	cout << ".word F" << this->children[0]->rhs << endl;
	cout << "jalr $5" << endl;

	if(this->children[2]->lhs == "arglist"){
		this->children[2]->popArgs(sig);		
	}

	// pop stucc
	cout << "add $30, $30, $4" << endl;
	cout << "lw $31, -4($30)" << endl;
	cout << "add $30, $30, $4" << endl;
	cout << "lw $29, -4($30)" << endl;
	

}

void TreeNode::pushArgs(string sig){
	this->children[0]->computeExpr(sig);
	cout << "sw $3, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;
	
	if(this->size == 3){
		this->children[2]->pushArgs(sig);
	}
}
void TreeNode::popArgs(string sig){
	//this->children[0]->computeExpr(sig);
	cout << "add $30, $30, $4" << endl;
//	cout << "lw $3, -4($30)" << endl;
	
	if(this->size == 3){
		this->children[2]->popArgs(sig);
	}
}
