// I pledge my Honor that I have not cheated, and will not cheat, on this assignment Peter Stilian
/* DJ Typechecker */

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "typecheck.h"

// This helperfunction checks to see if the class hierarchy is cyclic
// instead of returning bool I kept it simple. return 1 for cycle and 0 for acyclic
int checkIfCycle(int index1, int index2){
	int localChecker;

	// if more that the total number of classes has been recursively checked then there muust be a cycle and return 1
	if(index2 >= numClasses + 1) return 1;
	// if we find the superclass of the obkect then it is proven to be acyclic so return a 0
	if(index1 == -4) return 0;
	// reursively track through all classes until a cycle is/isnt found
	else{
		localChecker = checkIfCycle(classesST[index1].superclass, index2 + 1);
	}
	return localChecker;
}


/*------------------------------------------------------------------------------------------------------------------------*/


// This helper function recursively checks field name validity returns 1 for valid 0 for invalid.
int isUniqueField(char* fname, int index, int lineno){
	int localChecker;

	if(index == 0 || index == -4) return 1;

	// check against variable names
	for(int i = 0 ; i < classesST[index].numVars ; i++){
		if(!strcmp( fname, classesST[index].varList[i].varName)){
			printf("Duplicate field name located in class heirarchy at line # %d\n", lineno);
			exit(0);
		}
	}

	// check against static variable names
	for(int i = 0 ; i < classesST[index].numStaticVars ; i++){
		if(!strcmp( fname, classesST[index].staticVarList[i].varName)){
			printf("Duplicate field name located in class heirarchy at line # %d\n", lineno);
			exit(0);
		}
	}

	// recurse through heiarchy here
	return isUniqueField(fname, classesST[index].superclass, lineno);
}


/*------------------------------------------------------------------------------------------------------------------------*/


// This helper function recursively checks superclass types and names
// index1 tracks the class index while index 2 tracks the method index
int superMethodTypeCheck( int index1, int index2){
	int retVal = 1;
	int sClassVal = classesST[index1].superclass;

	if(index1 == 0 || index1 == -4) return 1;
	for(int i = 0 ; classesST[sClassVal].numMethods ; i++){
		// compare current method name with all of it's superclass methods
		if(!strcmp(classesST[index1].methodList[index2].methodName, classesST[sClassVal].methodList[i].methodName)){
			// if true, type check the return and param types
			if(classesST[index1].methodList[index2].returnType != classesST[sClassVal].methodList[i].returnType){
				retVal = 2;
				break;
			}
			if(classesST[index1].methodList[index2].paramType != classesST[sClassVal].methodList[i].paramType){
				retVal = 3;
				break;
			}
			retVal = superMethodTypeCheck(sClassVal, i);
		}
	}

	return retVal;

}

/*------------------------------------------------------------------------------------------------------------------------*/

// implementation described in typecheck.h
/* Returns nonzero iff sub is a subtype of super */
int isSubtype(int sub, int super){
	// check if NAT and BOOL
	if((sub == -1 && super == -1) || (sub == -2 && super == -2)) return 1;
	else if ((sub == -1 && super != -1) || (sub != -1 && super == -1) || (sub == -2 && super != -2) || (sub != -2 && super == -2)) return 0;

	// check Class/Objects
	if(sub == -3) return 1;
	if(super == -3) return 0;
	if(sub == -4) return 0;
	if(sub == super || classesST[sub].superclass == super) return 1;

	return isSubtype(classesST[sub].superclass, super);
}

/*------------------------------------------------------------------------------------------------------------------------*/

// recursive function that checks all super classes for an ID type. Returns -5 if nothing found. 
int superIDType(ASTree *t, char* fname, int index){

	if(index == 0 || index == -4) return -5;
	//check regular variables
	for(int i = 0 ; i < classesST[index].numVars ; i++){
		if(!strcmp(fname, classesST[index].varList[i].varName)){
			if(t->typ == DOT_ID_EXPR || t->typ == ID_EXPR || t->typ == ASSIGN_EXPR || t->typ == DOT_ASSIGN_EXPR || t->typ == METHOD_CALL_EXPR || DOT_METHOD_CALL_EXPR){
				t->staticMemberNum = i;
				t->isMemberStaticVar = 0;
				t->staticClassNum = index;
			}
			return classesST[index].varList[i].type;
		}
	}

	// check static vars
	for(int i = 0 ; i < classesST[index].numStaticVars ; i++){
		if(!strcmp(fname, classesST[index].staticVarList[i].varName)) return classesST[index].staticVarList[i].type;
	}

	return superIDType(t, fname, classesST[index].superclass);

}

/*------------------------------------------------------------------------------------------------------------------------*/

// This helper function finds the specific type of an ID 
int idType(ASTree *t, char* fname, int classContainingExpr, int methodContainingExpr){
	int temp;

	if(classContainingExpr >= 0){
		if(methodContainingExpr >= 0){
			// compare ID and param names. If matching return param val
			if(!strcmp(fname, classesST[classContainingExpr].methodList[methodContainingExpr].paramName)) return classesST[classContainingExpr].methodList[methodContainingExpr].paramType;

			// if first match unsuccessful check locals
			for(int i = 0 ; i < classesST[classContainingExpr].methodList[methodContainingExpr].numLocals ; i++){
				if(!strcmp(fname, classesST[classContainingExpr].methodList[methodContainingExpr].localST[i].varName)){
					if(t->typ == DOT_ID_EXPR || t->typ == ID_EXPR || t->typ == ASSIGN_EXPR || t->typ == DOT_ASSIGN_EXPR || t->typ == METHOD_CALL_EXPR || DOT_METHOD_CALL_EXPR){
						t->staticMemberNum = i;
						t->isMemberStaticVar = 0;
						t->staticClassNum = classContainingExpr;
					}
					return classesST[classContainingExpr].methodList[methodContainingExpr].localST[i].type;
				}
			}
		}

		// if we havent found type inside the class then search current class regular and static variables
		for(int i = 0 ; i < classesST[classContainingExpr].numVars ; i++){
			if(!strcmp(fname, classesST[classContainingExpr].varList[i].varName)){
				if(t->typ == DOT_ID_EXPR || t->typ == ID_EXPR || t->typ == ASSIGN_EXPR || t->typ == DOT_ASSIGN_EXPR || t->typ == METHOD_CALL_EXPR || DOT_METHOD_CALL_EXPR){
					t->staticMemberNum = 0;
					t->isMemberStaticVar = 0;
					t->staticClassNum = classContainingExpr;
					temp = classContainingExpr;
					while(temp != 0 && temp != -4){
						t->staticMemberNum += classesST[temp].numVars - 1;
						temp = classesST[temp].superclass;
					}
					t->staticMemberNum += i + 1;
				}
				return classesST[classContainingExpr].varList[i].type;
			}
		}

		// for static variables
		for(int i = 0 ; i < classesST[classContainingExpr].numStaticVars ; i++){
			if(!strcmp(fname, classesST[classContainingExpr].staticVarList[i].varName)){
				if(t->typ == DOT_ID_EXPR || t->typ == ID_EXPR || t->typ == ASSIGN_EXPR || t->typ == DOT_ASSIGN_EXPR || t->typ == METHOD_CALL_EXPR || DOT_METHOD_CALL_EXPR){
					t->staticMemberNum = i;
					t->isMemberStaticVar = 1;
					t->staticClassNum = classContainingExpr;
				}
				return classesST[classContainingExpr].staticVarList[i].type;
			}
		}

		// If not in current class check superclasses for ID type
		superIDType(t, fname, classesST[classContainingExpr].superclass);
	}

	// If still no match check the main block for ID type
	for(int i = 0 ; i < numMainBlockLocals ; i++){
		if(!strcmp(fname, mainBlockST[i].varName)){
			if(t->typ == ID_EXPR){
				t->staticMemberNum = i;
				t->isMemberStaticVar = 0;
				t->staticClassNum = -1;
			}
			if(t->typ == ASSIGN_EXPR){
				t->children->data->staticMemberNum = i;
				t->children->data->isMemberStaticVar = 1;
				t->children->data->staticClassNum = -1;
			}
			return mainBlockST[i].type;
		}
	}
	// IF no match literally anywhere I can think of then return ill-typed (-5)
	return -5;
}

/*------------------------------------------------------------------------------------------------------------------------*/
// struct for containing method info to return parameter and return types from checkForSuperMethods function
typedef struct methodInfo{
	int paramType;
	int returnType;
} MethodInfo;

MethodInfo returnInfo;


// recursive function that stores the parameter and return types in a global struct used only for this function.
int checkForSuperMethods(char* fname, int index){

	if(index == -4 || index == 0 ) return -5;
	for( int i = 0 ; i < classesST[index].numMethods ; i++){
		if(!strcmp(fname, classesST[index].methodList[i].methodName)){
			returnInfo.paramType = classesST[index].methodList[i].paramType;
			returnInfo.returnType = classesST[index].methodList[i].returnType;
			return index;
		}
	}

	return checkForSuperMethods(fname, classesST[index].superclass);

}

/*------------------------------------------------------------------------------------------------------------------------*/

int joinTypes(int t1, int t2){
	if(isSubtype(t1, t2)) return t2;
	if(isSubtype(t2, t1)) return t1;

	return joinTypes(classesST[t1].superclass, t2);
}

/*------------------------------------------------------------------------------------------------------------------------*/

int typeExpr(ASTree *t, int classContainingExpr, int methodContainingExpr){
	int checker;
	ASTList *temp;
	int sub1, sub2, sub3, sub4;

	// check for error of no typ value
	if(t->typ == NULL){
		printf("Error, missing an expression type on line # %d\n", t->lineNumber);
		exit(0);
	}

	if(t->typ == NAT_LITERAL_EXPR) return -1;
	else if (t->typ == TRUE_LITERAL_EXPR || t->typ == FALSE_LITERAL_EXPR) return -2;
	else if (t->typ == NULL_EXPR) return -3;
	else if (t->typ == NOT_EXPR){
		//recursively check the type of the original expr
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
		if( sub1 != -2){
			printf("Error, Invalid NOT expression found at line # %d\n", t->lineNumber);
			exit(0);
		}
		return -2;
	}
	else if(t->typ == AND_EXPR){
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
		sub2 = typeExpr(t->childrenTail->data, classContainingExpr, methodContainingExpr);

		if(sub1 != -2 || sub2 != -2){
			printf("Error, Invalid AND expression found at line # %d\n", t->lineNumber);
			exit(0);
		}
		return -2;
	}
	else if(t->typ == GREATER_THAN_EXPR){
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
		sub2 = typeExpr(t->childrenTail->data, classContainingExpr, methodContainingExpr);

		// expects nat types for sub expers
		if(sub1 != -1 || sub2 != -1){
			printf("Error, Invalid GREATER THAN expression found at line # %d\n", t->lineNumber);
			exit(0);
		}
		return -2;
	}
	else if(t->typ == PLUS_EXPR){
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
		sub2 = typeExpr(t->childrenTail->data, classContainingExpr, methodContainingExpr);

		// expects nat types for sub expers
		if(sub1 != -1 || sub2 != -1){
			printf("Error, Invalid PLUS expression found at line # %d\n", t->lineNumber);
			exit(0);
		}
		return -1;
	}
	else if(t->typ == MINUS_EXPR){
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
		sub2 = typeExpr(t->childrenTail->data, classContainingExpr, methodContainingExpr);

		// expects nat types for sub expers
		if(sub1 != -1 || sub2 != -1){
			printf("Error, Invalid MINUS expression found at line # %d\n", t->lineNumber);
			exit(0);
		}
		return -1;
	}
	else if(t->typ == EQUALITY_EXPR){
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
		sub2 = typeExpr(t->childrenTail->data, classContainingExpr, methodContainingExpr);

		// check to see if null or invalid
		if((sub1 < -3 || sub1 >= numClasses) || (sub2 < -3 || sub2 >= numClasses)){
			printf("Error, Invalid EQUALITY expression found at line # %d\n", t->lineNumber);
			exit(0);
		}

		if(!(isSubtype(sub1, sub2) ||isSubtype(sub2, sub1))){
			printf("Error, Invalid EQUALITY expression found at line # %d\n", t->lineNumber);
			exit(0);
		}
		return -2;
	}
	else if(t->typ == TIMES_EXPR){
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
		sub2 = typeExpr(t->childrenTail->data, classContainingExpr, methodContainingExpr);

		if(sub1 != -1 || sub2 != -1){
			printf("Error, Invalid TIMES expression found at line # %d\n", t->lineNumber);
			exit(0);
		}
		return -1;
	}
	else if(t->typ == DOT_ID_EXPR){
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);

		if(sub1 < 1 || sub1 >= numClasses){
			printf("Error, Invalid DOT ID expression found at line # %d\n", t->lineNumber);
			exit(0);
		}

		temp = t->children->next;
		sub2 = idType(t, temp->data->idVal, sub1, -1);

		// if no identifiable type found then error
		if(sub2 == -5){
			printf("Error, Invalid DOT ID expression found at line # %d\n", t->lineNumber);
			exit(0);
		}
		return sub2;
	}
	else if(t->typ == ASSIGN_EXPR){
		sub1 = idType(t, t->children->data->idVal, classContainingExpr, methodContainingExpr);
		sub2 = typeExpr(t->childrenTail->data, classContainingExpr, methodContainingExpr);

		if(!isSubtype(sub2, sub1)){
			printf("Invalid assignment on line # %d\n", t->lineNumber);
			exit(0);
		}
		return sub1;
	}
	else if(t->typ == DOT_ASSIGN_EXPR){
		// sub 1 will be LHS expr
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
		if(sub1 < 1 || sub1 >= numClasses){
			printf("Error, Invalid DOT ASSIGN expression found at line # %d\n", t->lineNumber);
			exit(0);
		}

		temp = t->children->next;
		sub2 = idType(t, temp->data->idVal, sub1, -1);

		// if no identifiable type found then error
		if(sub2 == -5){
			printf("Error, Invalid DOT ASSIGN expression found at line # %d\n", t->lineNumber);
			exit(0);
		}
		
		temp = temp->next;
		sub3 = typeExpr(temp->data, classContainingExpr, methodContainingExpr);

		if(!isSubtype(sub3, sub2)){
			printf("Invalid assignment on line # %d\n", t->lineNumber);
			exit(0);
		}

		return sub2;
	}
	else if(t->typ == DOT_METHOD_CALL_EXPR){
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
		if(sub1 < 1 || sub1 >= numClasses){
			printf("Error, Invalid DOT METHOD CALL expression found at line # %d\n", t->lineNumber);
			exit(0);
		}

		temp = t->children->next;
		checker = checkForSuperMethods(temp->data->idVal, sub1);
		if(checker == -5){
			printf("Method not found on line # %d\n", t->lineNumber);
			exit(0);
		}


		temp = temp->next;
		sub2 = typeExpr(temp->data, classContainingExpr, methodContainingExpr);

		if(!isSubtype(sub2, returnInfo.paramType)){
			printf("Invalid cal on line # %d\n", t->lineNumber);
			exit(0);
		}

		return returnInfo.returnType;		
	}
	else if(t->typ == METHOD_CALL_EXPR){
		
		if(classContainingExpr < 0){
			printf("Error with the METHOD CALL expression at line # %d\n", t->lineNumber);
			exit(0);
		}

		checker = checkForSuperMethods(t->children->data->idVal, classContainingExpr);

		if(checker == -5){
			printf("Invalid method call at line # %d\n", t->lineNumber);
			exit(0);
		}

		idType(t, t->children->data->idVal, classContainingExpr, methodContainingExpr);
		temp = t->children->next;
		sub1 = typeExpr(temp->data, classContainingExpr, methodContainingExpr);

		if(!isSubtype(sub1, returnInfo.paramType)){
			printf("Invalid parameter in method at line # %d\n", temp->data->lineNumber);
			exit(0);
		}

		return returnInfo.returnType;

	}
	else if(t->typ == NEW_EXPR){
		// method found in symtbl.h
		sub1 = typeNameToNumber(t->children->data->idVal);

		if(sub1 < 0 || sub1 >= numClasses){
			printf("Error, Invalid NEW expression found at line # %d\n", t->lineNumber);
			exit(0);
		}

		return sub1;
	}
	else if(t->typ == IF_THEN_ELSE_EXPR){
		// If block
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
		if(sub1 != -2){
			printf("Error, Invalid IF-THEN-ELSE expression found at line # %d\n", t->lineNumber);
			exit(0);
		}
		// Then block
		temp = t->children->next;
		sub2 = typeExprs(temp->data, classContainingExpr, methodContainingExpr);
		if(sub2 < -3 || sub2 >= numClasses){
			printf("Invalid type in THEN block of IF THEN ELSE expression found on line # %d\n", t->lineNumber);
			exit(0);
		}
		// Else block
		temp = temp->next;
		sub3 = typeExprs(temp->data, classContainingExpr, methodContainingExpr);
		if(sub3 < -3 || sub3 >= numClasses){
			printf("Invalid type in ELSE block of IF THEN ELSE expression found on line # %d\n", t->lineNumber);
			exit(0);
		}
		if(sub2 == -1 && sub3 == -1) return -1;
		if(sub2 == -2 && sub3 == -2) return -2;
		if(sub2 >= 0 && sub2 < numClasses && sub3 == -3) return sub2;
		if(sub3 >= 0 && sub3 < numClasses && sub2 == -3) return sub3;
		if(sub2 >= 0 && sub2 <= numClasses && sub3 >= 0 && sub3 <= numClasses) return joinTypes(sub2, sub3);
		else{
			printf("Invalid IF THEN ELSE expression at line # %d\n", t->lineNumber);
			exit(0);
		}
	}
	else if(t->typ == FOR_EXPR){
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
		if(sub1 < -3 || sub1 >= numClasses){
			printf("Invalid FOR loop initiaization found at line # %d\n", temp->data->lineNumber);
			exit(0);
		}

		temp = t->children->next;
		sub2 = typeExpr(temp->data, classContainingExpr, methodContainingExpr);

		if(sub2 != -2){
			printf("Invalid expression in FOR loop found at line # %d\n", temp->data->lineNumber);
			exit(0);
		}

		temp = temp->next;
		sub3 = typeExpr(temp->data, classContainingExpr, methodContainingExpr);

		if(sub3 < -3 || sub3 >= numClasses){
			printf("Invalid conclusion in FOR loop found at line # %d\n", temp->data->lineNumber);
			exit(0);
		}

		temp = temp->next;
		sub4 = typeExpr(temp->data, classContainingExpr, methodContainingExpr);

		if(sub4 < -3 || sub4 >= numClasses){
			printf("Invalid type in FOR loop found at line # %d\n", temp->data->lineNumber);
			exit(0);
		}
		return -1;
	}
	else if(t->typ == THIS_EXPR){
		if(classContainingExpr < 0){
			printf("Invalid THIS expression found at line # %d\n", t->lineNumber);
			exit(0);
		}
		return classContainingExpr;
	}
	else if(t->typ == INSTANCEOF_EXPR){
			sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
			sub2 = typeNameToNumber(t->childrenTail->data->idVal);

			if(sub1 < 0 || sub1 >= numClasses){
				if (sub1 != -3){
					printf("Invalid INSTANCE OF (LHS) expression found at line # %d\n", t->lineNumber);
					exit(0);
				}
			}

			if(sub2 < 0 || sub2 >= numClasses){
				if(sub2 != -3){
					printf("Invalid INSTANCE OF (RHS) expression found at line # %d\n", t->lineNumber);
					exit(0);
				}
			}
			//returns a boolean
			return -2;

		}
	else if(t->typ == PRINT_EXPR){
		sub1 = typeExpr(t->children->data, classContainingExpr, methodContainingExpr);
		if(sub1 != -1){
			printf("Invalid PRINT NAT expression. Expecting a NAT on line # %d\n", t->lineNumber);
			exit(0);
		}
		return -1;
	}
	else if(t->typ == READ_EXPR){
		return -1;
	}
	else if(t->typ == ID_EXPR){
		sub1 = idType(t, t->children->data->idVal, classContainingExpr, methodContainingExpr);

		if(sub1 == -5){
			printf("Invalid ID expression on line # %d\n", t->lineNumber);
			exit(0);
		}
		return sub1;
	}

	// If everything is messed up return ill typed (-5)
	else return -5;

}


/*------------------------------------------------------------------------------------------------------------------------*/

// Function cycles through all children in the AST tree and finds their expression types. returns -5 for ill-typed expers
// Uses the helper function typeExpr to fund the unique values.
int typeExprs(ASTree *t, int classContainingExpr, int methodContainingExpr){
	// set initial type to ill typed (-5)
	int eType = -5;
	ASTList *temp = t->children;

	// recurse through all children in the AST tree and continue checking if children are ill-typed
	while(temp){
		eType = typeExpr(temp->data, classContainingExpr, methodContainingExpr);
		temp = temp->next;
	}

	return eType;
}


/*------------------------------------------------------------------------------------------------------------------------*/


void typecheckProgram(){
	// I use the checker integer as a catch all comparison variable for all types. This allows me to have all helper functions return integers allowing more flexibility
	// than a boolean return value.
	int checker, class_id;

	//printf("typeChecker step 1 begins\n");


	// Begin step 1 of algorithm and check for unique class names
	for(int i = 0 ; i < numClasses ; i++){
		for( int j = i + 1 ; j < numClasses ; j++){
			if(strcmp(classesST[i].className, classesST[j].className) == 0 ){
				printf("Class already exists at line # %d\n", classesST[j].classNameLineNumber);
				exit(0);
			}
		}
	}

	//printf("typeChecker step 2 begins\n");

	// Begin Step 2 checking that all types in the symbol table are valid.
	for(int i = 1 ; i < numClasses ; i++){
		// Checking for valid superclass
		// If supertype < 0 || == class number then exit with error
		if(classesST[i].superclass < 0 || classesST[i].superclass == i){
			printf("Super class is invalid at line # %d\n", classesST[i].superclassLineNumber);
			exit(0);
		}

		//chack static var types
		for(int j = 0 ; j < classesST[i].numStaticVars ; j++){
			if(classesST[i].staticVarList[j].type < -2){
				printf("Invalid static variable type at line # %d\n", classesST[i].staticVarList[j].typeLineNumber);
				exit(0);
			}
		}


		//check method parameters and return types
		for(int j = 0 ; j < classesST[i].numMethods ; j++){

			if(classesST[i].methodList[j].paramType < -2){
				printf("Invalid parameter type for method at line # %d\n", classesST[i].methodList[j].paramTypeLineNumber);
				exit(0);
			}

			if(classesST[i].methodList[j].returnType < -2)
            {
                printf("Invalid return type for method at line # %d\n", classesST[i].methodList[j].returnTypeLineNumber);
                exit(0);
            }

            for(int k = 0 ; k <classesST[i].methodList[j].numLocals ; k++){
            	if(classesST[i].methodList[j].localST[k].type < -2){
            		printf("Invalid local variable in method at line # %d\n", classesST[i].methodList[j].localST[k].typeLineNumber);
            		exit(0);
            	}
            }
		}

		//check var types for validity
		for( int j = 0 ; j < classesST[i].numVars ; j++){
			if(classesST[i].varList[j].type < -2 ){
				printf("Invalid class variable type at line # %d\n", classesST[i].varList[j].typeLineNumber);
				exit(0);
			}
		}
	}

	// Check that all variables in the main block are also valid
	for( int i = 0 ; i < numMainBlockLocals ; i++){
		if(mainBlockST[i].type < -2){
			printf("Invalid local variable in main block at line # %d\n", mainBlockST[i].typeLineNumber);
			exit(0);
		}
	}

	//printf("typeChecker step 3 begins\n");

	// Begin Step 3, checking to make sure the class hierarchy is acyclic
	for(int i = 0 ; i < numClasses ; i++){
		checker = checkIfCycle(i, 0);

		// if helper function returns that the class has a cyclic inheritance then exit on error
		if(checker){
			printf("The class at line # \n is not acyclic", classesST[i].classNameLineNumber);
			exit(0);
		}
	}

	//printf("typeChecker step 4 begins\n");

	// Begin Step 4, checking that all field names are unique in both their defining class and its superclass
	// Part A, check field names are unique in defining class.
    for( int i = 0 ; i < numClasses ; i++){

    	for(int j = 0 ; j < classesST[i].numStaticVars ; j++){
    		for(int k = j + 1 ; k < classesST[i].numStaticVars; k++){
    			if(!strcmp(classesST[i].staticVarList[j].varName, classesST[i].staticVarList[k].varName)){
    				printf("Duplicate static field name found at line # %d\n", classesST[i].varList[k].varNameLineNumber);
    				exit(0);
    			}
    		}
    		for(int k = j + 1 ; k < classesST[i].numVars ; k++){
    			if(!strcmp(classesST[i].staticVarList[j].varName, classesST[i].varList[k].varName)){
    				printf("Duplicate field name in defining class\n");
    				exit(0);
    			}
    		}
    	}
    	for(int j = 0 ; j < classesST[i].numVars ; j++){
    		for( int k = j + 1 ; k < classesST[i].numVars ; k++){
    			if(!strcmp(classesST[i].varList[k].varName, classesST[i].varList[j].varName)){
    				printf("Duplicate field name in defining class\n");
    				exit(0);
    			}
    		}
    	}

    	// Part B checking to make sure all field names are unique in all superclasses
    	// check variable fields
    	for(int j = 0 ; j < classesST[i].numVars ; j++){
    		isUniqueField(classesST[i].varList[j].varName, classesST[i].superclass, classesST[i].varList[j].varNameLineNumber);
    	}
    	//check static variable fields
    	for(int j = 0 ; j < classesST[i].numStaticVars ; j++){
    		isUniqueField(classesST[i].staticVarList[j].varName, classesST[i].superclass, classesST[i].staticVarList[j].varNameLineNumber);
    	}

    }

    //printf("typeChecker step 5A begins\n");

	/* Begin Step 5, check all declared methods (method name is unique in its defining class, methods in superclasses with the same name have the same type
	signature, the parameter and local variable names lack duplicates, and the expression-list body’s type is a subtype of the declared return type)
	*/
    // Part A, check if method name is unique in it's defining class
    for(int i = 0 ; i < numClasses ; i++){
    	for(int j = 0 ; j < classesST[i].numMethods ; j++){
    		for(int k = j + 1 ; k < classesST[i].numMethods ; k++){
    			if(!strcmp(classesST[i].methodList[j].methodName, classesST[i].methodList[k].methodName)){
    				printf("Duplicate method name found at line # %d\n", classesST[i].methodList[k].methodNameLineNumber);
    				exit(0);
    			}
    		}
    	}
    }

    //printf("typeChecker step 5A complete\n");

    // Part B, check that methods in superclasses with the same name have the same type
    for(int i = 0 ; i < numClasses ; i++){
    	for(int j = 0 ; j < classesST[i].numMethods ; j++){

    		checker = superMethodTypeCheck(i,j);

    		if(checker == 2){
    			printf("Identified mismatched return type on line # %d\n", classesST[i].methodList[j].returnTypeLineNumber);
    			exit(0);
    		}

    		if(checker == 3){
    			printf("Identified mismatched parameter types at line # %d\n", classesST[i].methodList[j].returnTypeLineNumber);
    			exit(0);
    		}
    	}
    }

    //printf("typeChecker step 5B complete\n");

    // Part C, check that the parameters and local variables in methods lack duplicates
    for(int i = 0 ; i < numClasses ; i++){
    	for(int j = 0 ; j < classesST[i].numMethods ; j++){
    		for(int k = 0 ; k < classesST[i].methodList[j].numLocals; k++){
    			// compare local variable names in method against it's parameter names
    			if(!strcmp(classesST[i].methodList[j].localST[k].varName, classesST[i].methodList[j].paramName)){
    				printf("Method local variable has a name matching a method marameter. Error found at line # %d\n", classesST[i].methodList[j].localST[k].varNameLineNumber);
    				exit(0);
    			}
    			//compare all local variable names against others inside the same method
    			for(int n = k + 1 ; n < classesST[i].methodList[j].numLocals ; n++){
    				if(!strcmp(classesST[i].methodList[j].localST[k].varName, classesST[i].methodList[j].localST[n].varName)){
    					printf("Duplicate local variable name found at line # %d\n", classesST[i].methodList[j].localST[n].varNameLineNumber);
    					exit(0);
    				}
    			}
    		}
    	}
    }

    //printf("typeChecker step 5C complete\n");

    // Part D, check that the expression-list body's type is a subtye of the declared return type 
    for(int i = 0 ; i < numClasses ; i++){
    	for(int j = 0 ; j < classesST[i].numMethods ; j++){
    		// checks all expressions for validity
    		checker= typeExprs(classesST[i].methodList[j].bodyExprs, i, j);

    		if(!isSubtype(checker, classesST[i].methodList[j].returnType)){
    			printf("Invalid return type for the expression on line # %d\n", classesST[i].methodList[j].returnTypeLineNumber);
    		}
    	}
    }

   // printf("typeChecker step 5D complete\n");

    //printf("typeChecker step 6 begins\n");

    // Begin step 6, checking that the main-block variable names lack duplicates and the main-block expression list is well typed
    for(int i = 0 ; i < numMainBlockLocals ; i++){
    	for(int j = i + 1 ; j < numMainBlockLocals ; j++){
    		//printf("Max j = %d and current j = %d\n", numMainBlockLocals, j);
    		if(!strcmp(mainBlockST[i].varName, mainBlockST[j].varName)){
    			printf("Duplicate variable name found in main block at line # %d\n", mainBlockST[j].varNameLineNumber);
    			exit(0);
    		}
    	}
    }

    //printf("typeChecker step 7 begins\n");
    // Begin Step 7, Check to make sure main body expressions are not ill-typed
    checker = typeExprs(mainExprs, -1, -1);

}
