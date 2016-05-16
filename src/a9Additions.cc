#include "parseTree.h"

using namespace std;

//stack<int> s = stack<int>();
int offset = 0;
int loopCounter = 0;
int ifCounter = 0;

void computeAdd(string sig);
void computeSub(string sig);

void TreeNode::assembleMips(string sig){
	this->children[1]->findMain(sig);
	cout << "; FINISHED assembling main" << endl;
	this->children[1]->assembleProcedures(sig);
}
void TreeNode::findMain(string sig){
	if(this->lhs == "main"){
//		cout << "getting main sig" << endl;
		sig = "wain " + this->getSigMain();

		this->assembleMain(sig);
	}else{
		for(int i = 0; i<this->size; i++){
			this->children[i]->findMain(sig);
		}
	}

}

void TreeNode::assembleProcedures(string sig){
		cerr << "current node: " << endl;
		cerr << this->lhs << " " << this->rhs << endl;

	if(this->children[0]->lhs == "main"){
//		cout << "getting main sig" << endl;
//		string sig = "wain " + this->getSigMain();

//		this->assembleMain(sig);
	}else{
	//	cout << "current node: " << endl;
	//	cout << this->lhs << " " << this->rhs << endl;
		sig = this->children[0]->getSig();
		cout << ";got sig " << sig << endl;	
		this->children[0]->assembleProc(sig);		
		

	
		this->children[1]->assembleProcedures(sig);
		
	}
	
}

void pushRegs(){
	cout << "sw $5, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;
	cout << "sw $1, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;
	cout << "sw $2, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;
	cout << "sw $8, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;
	cout << "sw $9, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;
	
}

void popRegs(){
	cout << "add $30, $30, $4" << endl;
	cout << "lw $9, -4($30)" << endl;
	cout << "add $30, $30, $4" << endl;
	cout << "lw $8, -4($30)" << endl;
	cout << "add $30, $30, $4" << endl;
	cout << "lw $2, -4($30)" << endl;
	cout << "add $30, $30, $4" << endl;
	cout << "lw $1, -4($30)" << endl;
	cout << "add $30, $30, $4" << endl;
	cout << "lw $5, -4($30)" << endl;
}

void TreeNode::assembleProc(string sig){
	offset = 0;
	cout <<  "F" << this->children[1]->rhs <<  ": " << endl;
	
	if(this->children[3]->size == 1){
		offset += 4*  this->children[3]->children[0]->getNumParams();
		this->children[3]->children[0]->storeParams(sig);
	}
//this->store paramas

	cout << "sub $29, $30, $4" << endl;

	this->children[6]->assembleDcls(sig);

	// save regs
//	cout << 


	pushRegs();

	this->children[7]->assembleStatements(sig);

	this->children[9]->computeExpr(sig);
	
	popRegs();

	cout << "add $30, $29, $4" << endl;
	cout << "jr $31" << endl;	

}

void TreeNode::storeParams(string sig){
	string param = this->children[0]->children[1]->rhs;
 	(*(*tables)[sig])[param].offset = offset;
	offset -= 4;
	if(this->size == 3){
		this->children[2]->storeParams(sig);
	}
}

void TreeNode::assembleMain(string sig){
//	cout << "assembling main " << endl; 	
	this->storeMainParams(sig);

	// code for dcls
	this->children[8]->assembleDcls(sig);

	// code for statements
	this->children[9]->assembleStatements(sig);

	// return
	//string result = 
	this->children[11]->computeExpr(sig);

	cout << "add $30, $29, $4" << endl;
	cout << "jr $31" << endl;
}

void TreeNode::computeExpr(string sig){
	if(this->size == 1){
		this->children[0]->computeTerm(sig);
	} else if(this->size == 3){
		if(this->children[1]->lhs=="PLUS"){
			this->computeChildrenExpr(sig);
			// add epr and term
			this->computeAdd(sig);

		} else if(this->children[1]->lhs=="MINUS"){
			this->computeChildrenExpr(sig);
			// add epr and term
			this->computeSub(sig);

		}
	}
}

void TreeNode::computeChildrenExpr(string sig){
	
	this->children[0]->computeExpr(sig);
	cout << "; push result to stack" << endl;
	cout << "sw $3, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;

	cout << endl;
	// compute term
	this->children[2]->computeTerm(sig);

	//pop stack
	cout << "add $30, $30, $4" << endl;
	cout << "lw $5, -4($30)" << endl;

}

void TreeNode::computeTerm(string sig){
	if(this->size == 1){
		this->children[0]->computeFactor(sig);
	} else if(this->size == 3){
		if(this->children[1]->lhs=="STAR"){
			this->computeChildrenTerm(sig);
			// multiply term and factor
			cout << "mult $5, $3" << endl;
			cout << "mflo $3" << endl;
		} else if(this->children[1]->lhs=="SLASH"){
			this->computeChildrenTerm(sig);
			// divide term and factor
			cout << "div $5, $3" << endl;
			cout << "mflo $3" << endl;
		} else if(this->children[1]->lhs=="PCT"){
			this->computeChildrenTerm(sig);
			// mod term and factor
			cout << "div $5, $3" << endl;
			cout << "mfhi $3" << endl;
		}
	}
}
void TreeNode::computeChildrenTerm(string sig){
	
	this->children[0]->computeTerm(sig);
	cout << "; push result to stack" << endl;
	cout << "sw $3, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;

	cout << endl;
	// compute term
	this->children[2]->computeFactor(sig);

	//pop stack
	cout << "add $30, $30, $4" << endl;
	cout << "lw $5, -4($30)" << endl;

}

void TreeNode::computeFactor(string sig){
	if(size == 1){
		if(this->children[0]->lhs == "ID" ){
			storeIdVal(sig, this->children[0]->rhs);
		} else if(this->children[0]->lhs == "NUM"){
			storeNumVal(sig, this->children[0]->rhs);
		} else if(this->children[0]->lhs == "NULL"){
			// make $1 equal to 0x01 
			cout << "add $3, $0, $11" << endl;
		}
	} else if(size == 2){
		if(this->children[0]->lhs == "STAR"){
			// compute expr into $3
			this->children[1]->computeFactor(sig);
			//interpret $3 as an address
			cout << "lw $3, 0($3)" << endl;
		} else if(this->children[0]->lhs == "AMP"){
			// compute the amp
			this->children[1]->computeFactorAmp(sig);
		}
	} else if(size == 3){
		if(this->children[1]->lhs == "LPAREN"){
			this->callProcedure(sig);
		} else {
			this->children[1]->computeExpr(sig);
		}
	} else if(size == 4){
			this->callProcedure(sig);

	} else if(size == 5){
		// compute expr
		this->children[3]->computeExpr(sig);
		
		cout << "add $1, $3, $0" << endl;
	
		cout << "; calling new " << endl;
		cout << "; first push $31 onto stack" << endl;
		cout << "sw $31, -4($30)" << endl;
		cout << "sub $30, $30, $4" << endl;

		cout << "lis $9" << endl;
		cout << ".word new" << endl;
		cout << "jalr $9" << endl;

		// pop $31 back onto stack
		cout << "add $30, $30, $4" << endl;
		cout << "lw $31, -4($30)" << endl;
		
		// check if $3 is 0
		cout << "bne $3, $0, 2" << endl;
		cout << "add $3, $11, $0" << endl;
		cout << "add $20, $11, $11" << endl;
	}
}

void TreeNode::computeFactorAmp(string sig){
	if(this->size == 1){
		// get id of lval
		string id = this->children[0]->rhs;
	
		int storeOffset = (*(*tables)[sig])[id].offset;
		cout << "lis $3" << endl;
		cout << ".word " << storeOffset << endl;
		cout << "add $3, $3, $29" << endl;
		
	} else if(this->size == 2){
		this->children[1]->computeFactor(sig);
	} else {
		this->children[1]->computeFactorAmp(sig);
	}
}

void storeIdVal(string sig, string id){
	int os =  (*(*tables)[sig])[id].offset;
	cout << "; offset of id " << id << " " << os << endl;
	cout << "lw $3, " << os << "($29)" << endl;
 	
}
void storeNumVal(string sig, string num){
	cout << "lis $3" << endl;
	cout << ".word " << num << endl;
}

void TreeNode::storeMainParams(string sig){
	// get ID of param 1 and 2 and store offsets
	string param1 = this->children[3]->children[1]->rhs;
	string param2 = this->children[5]->children[1]->rhs;
 	(*(*tables)[sig])[param1].offset = offset;
	offset -= 4;
 	(*(*tables)[sig])[param2].offset = offset;
	offset -= 4;
//	cout << "done offets" << endl;	

	cout << ".import print" << endl;
	cout << ".import init" << endl;
	cout << ".import new" << endl;
	cout << ".import delete" << endl;
	
	cout << "lis $11" << endl;
 	cout << ".word 1" << endl;

	cout << "lis $4" << endl;
 	cout << ".word 4" << endl;
 	cout << "sub $29, $30, $4" << endl;
// get ID of param1

 	cout << "; push param 1 onto stack" << endl;
 	cout << "sw $1, -4($30)" << endl;
 	cout << "sub $30, $30, $4" << endl;

 	cout << "; push param 2" << endl;
 	cout << "sw $2, -4($30)" << endl;
 	cout << "sub $30, $30, $4" << endl;

	string param1Type = this->children[3]->children[0]->rhs;
	cout << "; THIS IS param1 " << param1Type << endl;
	if(param1Type == "INT"){
		cout << "add $2, $0, $0" << endl;
	}

	
		//call init
		cout << ";initing " << endl;
		cout << "; first push $31 onto stack" << endl;
		cout << "sw $31, -4($30)" << endl;
		cout << "sub $30, $30, $4" << endl;

		cout << "lis $9" << endl;
		cout << ".word init" << endl;
		cout << "jalr $9" << endl;

		// pop $31 back onto stack
		cout << "add $30, $30, $4" << endl;
		cout << "lw $31, -4($30)" << endl;

		cout << "lis $4" << endl;
		cout << ".word 4" << endl;

}

void TreeNode::assembleDcls(string sig){
	if(this->size == 5){
		this->children[0]->assembleDcls(sig);
		
		// compute current assignment
		if(this->children[3]->lhs == "NUM"){
			cout << "; push declaration onto stack" << endl;
			// get id of variable
			string id = this->children[1]->children[1]->rhs;
 			(*(*tables)[sig])[id].offset = offset;
			offset -= 4;			
			
			// get value of num and put it into $1
			cout << "lis $1" << endl;
			cout << ".word " << this->children[3]->rhs << endl;

			cout << "sw $1, -4($30)" << endl;
			cout << "sub $30, $30, $4" << endl;
		} else if(this->children[3]->lhs == "NULL"){
			cout << "; push NULL onto stack" << endl;
			// get id of variable
			string id = this->children[1]->children[1]->rhs;
 			(*(*tables)[sig])[id].offset = offset;
			offset -= 4;			
			
			// make $1 equal to 0x01 
			cout << "add $1, $0, $11" << endl;

			cout << "sw $1, -4($30)" << endl;
			cout << "sub $30, $30, $4" << endl;
		}
	}
}

void TreeNode::assembleStatements(string sig){
	if(this->size == 2){
		cout << "; COMPUTING STATEMENTS" << endl;
		this->children[0]->assembleStatements(sig);
		this->children[1]->computeStatement(sig);
	}
}

void TreeNode::computeStatement(string sig){
	if(this->children[0]->lhs == "PRINTLN"){
		this->children[2]->computeExpr(sig);
		//print expression
		cout << ";printing " << endl;
		cout << "; first push $31 onto stack" << endl;
		cout << "sw $31, -4($30)" << endl;
		cout << "sub $30, $30, $4" << endl;

		// place result of expr in $1
		cout << "add $1, $3, $0" << endl;
		cout << "lis $2" << endl;
		cout << ".word print" << endl;
		cout << "jalr $2" << endl;

		// pop $31 back onto stack
		cout << "add $30, $30, $4" << endl;
		cout << "lw $31, -4($30)" << endl;
	} else if(this->children[0]->lhs == "lvalue"){
		// compute expr
		this->children[2]->computeExpr(sig);
			// move expr result to unused reg
			cout << "add $8, $0, $3" << endl;
	
		// get id of lVal
		string id = this->children[0]->lValGetId(sig);
		// if lvalue has star
		if( id == ""){
			cout << "sw $8, 0($3)" << endl;
		} else {


			int storeOffset = (*(*tables)[sig])[id].offset;
			cout << "; assigning value " << endl;
			cout << "sw $3, " << storeOffset << "($29)" << endl;	
		}
	} else if(this->children[0]->lhs == "WHILE"){
		// place lable
		loopCounter++;
		int currentLoopCounter = loopCounter;
		cout << "loop" << currentLoopCounter << ": ";
		// compute test
		this->children[2]->computeTest(sig);			
		// if test failed, go to end of loop
		cout << "beq $3, $0, endloop" << currentLoopCounter << endl;
		// else print statements	
		this->children[5]->assembleStatements(sig);
		cout << "beq $0, $0, loop" << currentLoopCounter << endl;
		cout << "endloop" << currentLoopCounter << ":" << endl;
		//loopCounter--;
	} else if(this->children[0]->lhs == "IF"){
		ifCounter++;
		int counter = ifCounter;
		cout << "if" << counter << ": ";
		this->children[2]->computeTest(sig);

		cout << "beq $3, $0, else" << counter << endl;
		this->children[5]->assembleStatements(sig);
		cout << "beq $0, $0, endif" << counter << endl;
		cout << "else" << counter << ": ";
		this->children[9]->assembleStatements(sig);
		cout << "endif" << counter << ": " << endl;	
	} else if(this->children[0]->lhs == "DELETE"){
		// first compute expr
		this->children[3]->computeExpr(sig);

		// check if $3 is NULL don't call delete
		cout << "beq $3, $11, 9" << endl;

		cout << "add $1, $3, $0" << endl;
		
		cout << ";deleting " << endl;
		cout << "; first push $31 onto stack" << endl;
		cout << "sw $31, -4($30)" << endl;
		cout << "sub $30, $30, $4" << endl;

		cout << "lis $9" << endl;
		cout << ".word delete" << endl;
		cout << "jalr $9" << endl;

		// pop $31 back onto stack
		cout << "add $30, $30, $4" << endl;
		cout << "lw $31, -4($30)" << endl;
		cout << "add $21, $11, $11" << endl;
	}
}
 
string TreeNode::lValGetId(string sig){
	if(this->size == 1){
		return this->children[0]->rhs;
	} else if(this->size == 2){
		this->children[1]->computeFactor(sig);
//		cout << "lw $3, 0($3)" << endl;
		return "";
	}
	return this->children[1]->lValGetId(sig);
}

void TreeNode::computeTest(string sig){

	this->children[0]->computeExpr(sig);
	cout << "; push (test) result to stack" << endl;
	cout << "sw $3, -4($30)" << endl;
	cout << "sub $30, $30, $4" << endl;
	this->children[2]->computeExpr(sig);
	cout << "add $30, $30, $4" << endl;
	cout << "lw $5, -4($30)" << endl;

	// this part will determine whether to us slt or sltu
	string type = this->children[0]->checkWellTyped(sig);
	string comp = "slt";
	if(type == "int*"){
		comp = "sltu";
	}

	if(this->children[1]->lhs == "LT"){
		cout << ";LESS THAN? " << endl;
		cout << comp << " $3, $5, $3" << endl;
	} else if(this->children[1]->lhs == "GT"){
		cout << ";Greater THAN? " << endl;
		cout << comp << " $3, $3, $5" << endl;
	}else if(this->children[1]->lhs == "NE"){
		cout << ";NOT EQUAL? " << endl;
		cout << comp << " $6, $5, $3" << endl;
		cout << comp << " $7, $3, $5" << endl;
		cout << "add $3, $7, $6" << endl;
	}else if(this->children[1]->lhs == "EQ"){
		cout << ";EQUAL? " << endl;
		cout << comp << " $6, $5, $3" << endl;
		cout << comp << " $7, $3, $5" << endl;
		cout << "add $3, $7, $6" << endl;
		cout << "sub $3, $11, $3" << endl;
	}else if(this->children[1]->lhs == "GE"){
		cout << ";GREATER OR EQUAL? " << endl;
		cout << comp << " $3, $5, $3" << endl;
		cout << "sub $3, $11, $3" << endl;
	}else if(this->children[1]->lhs == "LE"){
		cout << ";LESS OR EQUAL? " << endl;
		cout << comp << " $3, $3, $5" << endl;
		cout << "sub $3, $11, $3" << endl;
	}
}

	
