// I pledge my Honor that I have not cheated, and will not cheat, on this assignment Peter Stilian

#include <stdio.h>
#include "symtbl.h"
#include "codegen.h"

// global variable for tracking labels
unsigned int labelNumber = 0;

/* Using the global classesST, calculate the total number of
 (non-static) fields, including inherited fields, in an object of
 the given type. */
int getNumObjectFields(int type){

}

/* Generate code that increments the stack pointer */
void incSP(){
    fprintf(fout, "mov 1 1 ;\n");
    fprintf(fout, "add 6 6 1 ; increment stack register\n");
    fprintf(fout, "mov 1 65536;\n");
    fprintf(fout, "blt 6 1 #%d ;check if stack register is less then 65535\n", labelNumber);
    fprintf(fout, "mov 1 78 ; error code 78 => out of bounds stack\n");
    fprintf(fout, "hlt 1\n");
    fprintf(fout, "#%d: mov 0 0\n", labelNumber);
    labelNumber++;
}

/* Generate code that decrements the stack pointer */
void decSP(){
    fprintf(fout, "mov 1 1\n");
    fprintf(fout, "sub 6 6 1; decrement stack register\n");
    fprintf(fout, "blt 5 6 #%d\n", labelNumber);
    fprintf(fout, "mov 1 77; error code 77 => out of stack memory\n");
    fprintf(fout, "hlt 1\n");
    fprintf(fout, "#%d: mov 0 0\n", labelNumber);
    labelNumber++;
}

/* Returns the address of the given static field in the given class. */
int getStaticFieldAddress(int staticClassNum, int staticMemberNum){

}

/* Output code to check for a null value at the top of the stack.
 If the top stack value (at M[SP+1]) is null (0), the DISM code
 output will halt. */
void checkNullDereference(){

}

/* Generate DISM code for the given single expression, which appears
 in the given class and method (or main block).
 If classNumber < 0 then methodNumber may be anything and we assume
 we are generating code for the program's main block. */
void codeGenExpr(ASTree *t, int classNumber, int methodNumber){
    int temp1, temp2;
    
    if(t->typ == NAT_LITERAL_EXPR){
        fprintf(fout, "mov 1 %d\n", t->natVal);
        fprintf(fout, "str 6 0 1 ; [M[SP] <- R[r1] (a nat literal)\n");
        decSP();
    }
    else if(t->typ == TRUE_LITERAL_EXPR){
        fprintf(fout, "mov 1 %d\n", t->natVal);
        fprintf(fout, "str 6 0 1 ; [M[SP] <- R[r1] (a true literal)\n");
        decSP();
    }
    else if(t->typ == FALSE_LITERAL_EXPR){
        fprintf(fout, "mov 1 %d\n", t->natVal);
        fprintf(fout, "str 6 0 1 ; [M[SP] <- R[r1] (a false literal)\n");
        decSP();
    }
    else if(t->typ == NULL_EXPR){
        fprintf(fout, "str 6 0 0 ; M[SP] <- null\n");
        decSP();
    }
    else if(t->typ == NOT_EXPR){
        codeGenExpr(t->children->data, classNumber, methodNumber);

        fprintf(fout, "lod 1 6 1 ; get result of first expr\n");
        fprintf(fout, "mov 2 0 ; mov 0 into r2 for boolean comparison\n");
        fprintf(fout, "sub 1 1 2 ; R1 = R1 - R2\n");
        fprintf(fout, "beq 1 2 #%d ; If equal false and push 1 onto stack\n", labelNumber);
        fprintf(fout, "mov 1 0 ; store inverse of 1 in r1\n");
        fprintf(fout, "str 6 1 1 ; m[SP] <- R[r1]\n");

        fprintf(fout, "mov 1 #%d ; jump to false case\n", labelNumber + 1);
        fprintf(fout, "jmp 1 0\n");
        fprintf(fout, "#%d: mov 1 1\n", labelNumber);
        fprintf(fout, "str 6 1 1 ; m[SP+1] <- R[r1] inverse of 0\n");
        fprintf(fout, "#%d: mov 0 0 ; exit label\n", labelNumber + 1);
        labelNumber += 2;
    }
    else if(t->typ == AND_EXPR){
       ASTList *temp;
       temp = t->children->next;
       temp1 = labelNumber++;
       temp2 = labelNumber++;

       codeGenExpr(t->children->data, classNumber, methodNumber);

        // First Expression
       fprintf(fout, "lod 1 6 1 ; get result of first expr\n");
       fprintf(fout, "mov 2 0 ; store 1 in R2 to check against result of 1st expr\n");
       fprintf(fout, "beq 1 2 #%d ; jump to short circuit  case if equal\n", temp1);

       codeGenExpr(temp->data, classNumber, methodNumber);

       // Second Expression
       fprintf(fout, "lod 1 6 1 ; get result of second expr\n");
       fprintf(fout, "mov 2 0 ; store 0 in R2 to check against result of 2nd expr\n");
       fprintf(fout, "beq 1 2 #%d ; jump to 0 case if 2nd expr is false\n", temp1);
       fprintf(fout, "mov 1 1 ; return true\n");

       fprintf(fout, "str 6 1 1 ; store R1 at M[SP+1]\n");
       fprintf(fout, "mov 1 #%d ; store exit address in R1\n", temp2);
       fprintf(fout, "jmp 1 0 ; jump to exit address\n");
       fprintf(fout, "#%d: mov 1 0 ; store 0 in R1\n", temp1);
       fprintf(fout, "str 6 1 1 ; store R1 at M[SP+1]\n");
       fprintf(fout, "#%d: mov 0 0 ; exit address for AND_EXPR", temp2);
    }
    // this one is rough come back to it later
    else if(t->typ == GREATER_THAN_EXPR){
        ASTList *temp;
       temp = t->children->next;
       temp1 = labelNumber++;
       temp2 = labelNumber++;

       codeGenExpr(t->children->data, classNumber, methodNumber);
       codeGenExpr(temp->data, classNumber, methodNumber);

       fprintf(fout, "lod 2 6 2 ; load M[SP+2] for first expression\n");
       fprintf(fout, "lod 1 6 1 ; load M[SP+1] for second expression\n");

       fprintf(fout, "blt 1 2 #%d ; Jump to valid case for expr2 < expr 1\n", temp1);
       fprintf(fout, "mov 1 0 ; invalid case start return a false statement\n");
       fprintf(fout, "str 6 2 1 ; m[SP] <- R[r1] store false on stack\n");
       fprintf(fout, "mov 1 #%d ; label for finish\n", temp2);
       fprintf(fout, "jmp 1 0 ; jump over valid to finish\n");
       fprintf(fout, "#%d: mov 1 1 ; valid case start\n", temp1);
       fprintf(fout, "str 6 2 1 ; store true statement on stack\n");
       fprintf(fout, "#%d: mov 0 0 ; exit point for GREATER_THAN_EXPR\n", temp2);
       incSP();

    }
    else if(t->typ == PLUS_EXPR){
        ASTList *temp;
        temp = t->children->next;

        codeGenExpr(t->children->data, classNumber, methodNumber);
        codeGenExpr(temp->data, classNumber, methodNumber);

        fprintf(fout, "lod 1 6 2 ; load M[SP+2] for first expression\n");
        fprintf(fout, "lod 2 6 1 ; load M[SP+1] for second expression\n");
        fprintf(fout, "add 1 1 2 ; add values from PLUS_EXPR\n");
        fprintf(fout, "str 6 2 1; m[SP] <- R[r1] (Result from Plus Expr)\n");
        incSP();
    }
    else if(t->typ == MINUS_EXPR){
        ASTList *temp;
        temp = t->children->next;

        codeGenExpr(t->children->data, classNumber, methodNumber);
        codeGenExpr(temp->data, classNumber, methodNumber);

        fprintf(fout, "lod 1 6 2 ; load M[SP + 2] for first expression\n");
        fprintf(fout, "lod 2 6 1 ; load M[SP + 1] for second expression\n");
        fprintf(fout, "sub 1 1 2 ; subtract expr2 result from expr1 result\n");
        fprintf(fout, "str 6 2 1 ; m[SP] <- R[r1] (Result from Minus Expr)\n");
        incSP();
    }
    else if(t->typ == EQUALITY_EXPR){
        ASTList *temp;
        temp = t->children->next;
        temp1 = labelNumber++;
        temp2 = labelNumber++;

        codeGenExpr(t->children->data, classNumber, methodNumber);
        codeGenExpr(temp->data, classNumber, methodNumber);

        fprintf(fout, "lod 2 6 2 ; load M[SP + 2] for result of first expr\n");
        fprintf(fout, "lod 1 6 1 ; load M[SP + 1] for result of second expr\n");
        fprintf(fout, "beq 1 2 #%d ; break if they are in fact equal\n", temp1);
        fprintf(fout, "mov 1 0 ; return false if they are not equal\n");
        fprintf(fout, "str 6 2 1 ; m[SP] <- R[r1] store false on stack\n");
        fprintf(fout, "mov 1 #%d ; label for finish\n", temp2);
        fprintf(fout, "jmp 1 0 ; jump over true case to finish\n");
        fprintf(fout, "#%d: mov 1 1 ; equal case start, return true\n", temp1);
        fprintf(fout, "str 6 2 1 ; store true on the stack\n");
        fprintf(fout, "#%d: mov 0 0 ; exit point for equality expr\n", temp2);
        incSP();
    }
    else if(t->typ == TIMES_EXPR){
        ASTList *temp;
        temp = t->children->next;

        codeGenExpr(t->children->data, classNumber, methodNumber);
        codeGenExpr(temp->data, classNumber, methodNumber);

        fprintf(fout, "lod 1 6 2 ; load M[SP + 2] for first expression\n");
        fprintf(fout, "lod 2 6 1 ; load M[SP + 1] for second expression\n");
        fprintf(fout, "mul 1 1 2 ; multiply expr2 result by expr1 result\n");
        fprintf(fout, "str 6 2 1 ; m[SP] <- R[r1] (Result from Mult Expr\n");
        incSP();
    }
    else if(t->typ == DOT_ID_EXPR){
        char *id;
        int temp;

        codeGenExpr(t->children->data, classNumber, methodNumber);

        fprintf(fout, "lod 1 6 1 ; get object from LHS of DOT_ID_EXPR\n");
        /*If the variable is a regular variable*/
        if(!t->isMemberStaticVar){
            fprintf(fout, "lod 1 1 %d ; get offset of object member\n",
                           t->staticMemberNum + 1); 
            fprintf(fout, "str 6 0 1 ; place the value of object member on stack\n");
            decSP();
        }

        // needs implementation of static vars---------------------------------------------
        //if(t->isMemberStaticVar){}
    }
    else if(t->typ == ASSIGN_EXPR){
        codeGenExpr(t->children->next->data, classNumber, methodNumber);
        fprintf(fout, "lod 1 6 1 ; load M[SP + 1] from result of RHS\n");

        /*Retrieve the ID value based on pointer */
        if(t->children->data->staticClassNum == -1){
            fprintf(fout, "mov 2 %d\n", t->children->data->staticMemberNum);
            fprintf(fout, "sub 3 7 2 ; add offset of local var to R1\n");
            fprintf(fout, "str 3 0 1 ; store the value of RHS into local variable\n");
        }
    }
    else if(t->typ == DOT_ASSIGN_EXPR){
        codeGenExpr(t->children->data, classNumber, methodNumber);
        codeGenExpr(t->childrenTail->data, classNumber, methodNumber);

        fprintf(fout, "lod 1 6 2 ; get object/class from LHS of DOT_ASSIGN_EXPR\n");
        fprintf(fout, "lod 2 6 1 ; get RHS of DOT_ASSIGN_EXPR\n");

        if(!t->isMemberStaticVar){
            fprintf(fout, "str 1 %d 2 ; store value of RHS into member in LHS\n", t->staticMemberNum + 1);
           
        }
        // needs implementation of static vars---------------------------------------------
        //if(t->isMemberStaticVar){}
    }
    //else if(t->typ == DOT_METHOD_CALL_EXPR)
    //else if(t->typ == METHOD_CALL_EXPR)
    //else if(t->typ == NEW_EXPR)
    else if(t->typ == IF_THEN_ELSE_EXPR){
        ASTList *temp;
        temp = t->children->next;
        temp1 = labelNumber++;
        temp2 = labelNumber++;

        codeGenExpr(t->children->data, classNumber, methodNumber);

        fprintf(fout, "lod 1 6 1; load M[SP + 1] for result of conditional\n");
        fprintf(fout, "mov 2 0 ; move 0 into R2 for comparison\n");
        fprintf(fout, "beq 1 2 #%d ; if equal to 0 then we go to the then block\n", temp1);

        codeGenExprs(temp->data, classNumber, methodNumber);

        fprintf(fout, "mov 1 #%d\n", temp2);
        fprintf(fout, "jmp 1 0 ; jump over the else code and continue execution\n");
        fprintf(fout, "#%d: mov 0 0 ; else then block\n", temp1);

        temp = temp->next;
        codeGenExprs(temp->data, classNumber, methodNumber);

        fprintf(fout, "#%d: mov 0 0 ; exit label for either if or then cases\n", temp2);
    }
    else if(t->typ == FOR_EXPR){
        ASTList *temp;
        temp = t->children->next;
        temp1 = labelNumber++;
        temp2 = labelNumber++;

        codeGenExpr(t->children->data, classNumber, methodNumber);

        fprintf(fout, "#%d: mov 0 0 ; label to jump to for conditional check\n", temp1);

        codeGenExpr(temp->data, classNumber, methodNumber);

        fprintf(fout, "lod 1 6 1 ; take the result from the conditional statement\n");
        fprintf(fout, "mov 2 0 ; move 0 into R2 to check result from conditional against false value\n");
        fprintf(fout, "beq 1 2 #%d ; jump to exit label if we have a false conditional\n", temp2);

        temp = temp->next; 
        temp = temp->next;
        codeGenExprs(temp->data, classNumber, methodNumber);

        /*Generate epilogue code*/
        temp = t->children->next->next;

        codeGenExpr(temp->data, classNumber, methodNumber);

        fprintf(fout, "mov 1 #%d ; store conditional label in 1\n", temp1);
        fprintf(fout, "jmp 1 0 ; jmp to conditional label\n");
        fprintf(fout, "#%d: mov 0 0 ; exit label for loop\n", temp2);
    }
    //else if(t->typ == THIS_EXPR)
    //else if(t->typ == INSTANCEOF_EXPR)
    else if(t->typ == PRINT_EXPR){
        codeGenExpr(t->children->data, classNumber, methodNumber);
        fprintf(fout, "lod 1 6 1 ; load M[SP + 1] for printing\n");
        fprintf(fout, "ptn 1\n");
    }
    else if(t->typ == READ_EXPR){
        fprintf(fout, "rdn 1 ; read input into R1\n");
        fprintf(fout, "str 6 0 1; m[SP] <- R[r1] (Read Expr Result)\n");
        decSP();
    }
    else if(t->typ == ID_EXPR){
        if(t->staticClassNum == -1)
        {
            fprintf(fout, "mov 2 %d\n", t->staticMemberNum);
            fprintf(fout, "sub 1 7 2 ; add offset of local var to R1\n");
            fprintf(fout, "lod 1 1 0 ; take the value from memory address\n");
            fprintf(fout, "str 6 0 1 ; store the value of local variable on the stack\n");
            decSP();
        }
    }
    else{
        printf("ERROR\n");
        exit(0);
    }
    
}

/* Generate DISM code for an expression list, which appears in
 the given class and method (or main block).
 If classNumber < 0 then methodNumber may be anything and we assume
 we are generating code for the program's main block. */
void codeGenExprs(ASTree *expList, int classNumber, int methodNumber){
    ASTList* temp = expList->children;

    while(temp){
        codeGenExpr(temp->data, classNumber, methodNumber);
        temp = temp->next;
    }
}

/* Generate DISM code as the prologue to the given method or main
 block. If classNumber < 0 then methodNumber may be anything and we
 assume we are generating code for the program's main block. */
void genPrologue(int classNumber, int methodNumber){

}

/* Generate DISM code as the epilogue to the given method or main
 block. If classNumber < 0 then methodNumber may be anything and we
 assume we are generating code for the program's main block. */
void genEpilogue(int classNumber, int methodNumber){

}

/* Generate DISM code for the given method or main block.
 If classNumber < 0 then methodNumber may be anything and we assume
 we are generating code for the program's main block. */
void genBody(int classNumber, int methodNumber){
    codeGenExprs(classesST[classNumber].methodList[methodNumber].bodyExprs, classNumber, methodNumber);
}

/* Map a given (1) static class number, (2) a method number defined
 in that class, and (3) a dynamic object's type to:
 (a) the dynamic class number and (b) the dynamic method number that
 actually get called when an object of type (3) dynamically invokes
 method (2).
 This method assumes that dynamicType is a subtype of staticClass. */
void getDynamicMethodInfo(int staticClass, int staticMethod, int dynamicType, int *dynamicClassToCall, int *dynamicMethodToCall){

}

/* Emit code for the program's vtable, beginning at label #VTABLE.
 The vtable jumps (i.e., dispatches) to code based on
 (1) the dynamic calling object's address (at M[SP+4]),
 (2) the calling object's static type (at M[SP+3]), and
 (3) the static method number (at M[SP+2]). */
void genVTable(){

}

/* Emit code for the program's instanceof-table (itable).
 The itable stores a zero on the stack (at M[SP+1]) if the
 object being tested is an instanceof the required class;
 otherwise the itable stores a nonzero value on the stack
 (at M[SP+1]). */
void genITable(){

}

void generateDISM(FILE *outputFile){
    
    fout = outputFile;

    // initialize pointers in output file
    fprintf(fout, "mov 7 65535 ; Initialize FP\n");
    fprintf(fout, "mov 6 65535 ; Initialize SP\n");
    fprintf(fout, "mov 5 1; Initialize HP\n");

    for( int i = 0 ; i < numMainBlockLocals ; i++){
        fprintf(fout, "mov 1 0 ;\n");
        fprintf(fout, "str 6 0 1 ; store 0 in M[SP]\n");
        decSP(); //derement SP to travel up the stack
    }

    codeGenExprs(mainExprs, -1, -1);
    fprintf(fout, "mov 1 0\n");
    fprintf(fout, "hlt 1 ; Terminate execution of main block code\n");
}