// I pledge my Honor that I have not cheated, and will not cheat, on this assignment Peter Stilian

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

void printError(char *reason) {
  printf("AST Error: %s\n", reason);
  exit(-1);
}

/* Create a new AST node of type t */
ASTree *newAST(ASTNodeType t, ASTree *child, unsigned int natAttribute, char *idAttribute, unsigned int lineNum) {
  ASTree *newNode = malloc(sizeof(ASTree));
  if(newNode==NULL) printError("Failed to allocate memory for new node\n");
  if(!newNode){
    printf("failed to allcoate memory\n");
    exit(0);
  }

  // set type value for AST
  newNode->typ = t;

  // create a linked list of children
  ASTList *childList = malloc(sizeof(ASTList));
  if(childList==NULL) printError("malloc in newAST()");
  childList->data = child;
  childList->next = NULL;
  newNode->childrenTail = childList;
  
  // Set natVal if applicable
  if(t == NAT_LITERAL_EXPR || t == TRUE_LITERAL_EXPR || t == FALSE_LITERAL_EXPR){
    newNode->natVal = natAttribute;
  }

  // set ID values
  else if( t == AST_ID){
    char *copyStr = malloc(strlen(idAttribute)+1);
    if(copyStr == NULL) printError("malloc in newAST()\n");
    strcpy(copyStr, idAttribute);
    newNode->idVal = copyStr;
  }
  
  // set lineNumber
  newNode->lineNumber = lineNum;

  return newNode;
}

/* Append a new child AST node onto a parent's list of children */
ASTree *appendToChildrenList(ASTree *parent, ASTree *newChild) {
  // check for parent
  if(parent==NULL) printError("append called with null parent");
  // check for child
  if(newChild==NULL) printError("append called with null newChild");
  
  if(parent->childrenTail->data==NULL) //replace empty tail with new child
    parent->childrenTail->data = newChild;
  else { //tail of children is currently nonempty; append new child to list
    ASTList *newList = malloc(sizeof(ASTList));
    if(newList==NULL) printError("malloc in appendAST()");
    newList->data = newChild;
    newList->next = NULL;
    parent->childrenTail->next = newList;
    parent->childrenTail = newList;
  }

  // Allocate memory for node
  ASTList *newListNode = malloc(sizeof(ASTList));
  // Check for memory allocation
  if(!newListNode){
    printf("Failed to allocate memory\n");
    exit(0);
  }

  if(!parent->children) parent->children = newListNode;

  parent->childrenTail = newListNode;
  newListNode->data = newChild;
  newListNode->next = NULL;

  return parent;

}

/* Print the type of this node and any node attributes */
void printNodeTypeAndAttribute(ASTree *t) {
  if(t==NULL) return;
  switch(t->typ) {
    case PROGRAM: printf("PROGRAM"); break;
    case CLASS_DECL_LIST: printf("CLASS_DECL_LIST"); break;
    case CLASS_DECL: printf("CLASS_DECL"); break;
    case STATIC_VAR_DECL_LIST: printf("STATIC_VAR_DECL_LIST"); break;
    case STATIC_VAR_DECL: printf("STATIC_VAR_DECL"); break;
    case VAR_DECL_LIST: printf("VAR_DECL_LIST"); break;
    case VAR_DECL: printf("VAR_DECL"); break;
    case METHOD_DECL_LIST: printf("METHOD_DECL_LIST"); break;
    case METHOD_DECL: printf("METHOD_DECL"); break;
    case NAT_TYPE: printf("NAT_TYPE"); break;
    case BOOL_TYPE: printf("BOOL_TYPE"); break;
    case AST_ID: printf("AST_ID(%s)", t->idVal); break;
    case EXPR_LIST: printf("EXPR_LIST"); break;
    case DOT_METHOD_CALL_EXPR: printf("DOT_METHOD_CALL_EXPR"); break;
    case METHOD_CALL_EXPR: printf("METHOD_CALL_EXPR"); break;
    case DOT_ID_EXPR: printf("DOT_ID_EXPR"); break;
    case ID_EXPR: printf("ID_EXPR"); break;
    case DOT_ASSIGN_EXPR: printf("DOT_ASSIGN_EXPR"); break;
    case ASSIGN_EXPR: printf("ASSIGN_EXPR"); break;
    case PLUS_EXPR: printf("PLUS_EXPR"); break;
    case MINUS_EXPR: printf("MINUS_EXPR"); break;
    case TIMES_EXPR: printf("TIMES_EXPR"); break;
    case EQUALITY_EXPR: printf("EQUALITY_EXPR"); break;
    case GREATER_THAN_EXPR: printf("GREATER_THAN_EXPR"); break;
    case NOT_EXPR: printf("NOT_EXPR"); break;
    case AND_EXPR: printf("AND_EXPR"); break;
    case INSTANCEOF_EXPR: printf("INSTANCEOF_EXPR"); break;
    case IF_THEN_ELSE_EXPR: printf("IF_THEN_ELSE_EXPR"); break;
    case FOR_EXPR: printf("FOR_EXPR"); break;
    case PRINT_EXPR: printf("PRINT_EXPR"); break;
    case READ_EXPR: printf("READ_EXPR"); break;
    case THIS_EXPR: printf("THIS_EXPR"); break;
    case NEW_EXPR: printf("NEW_EXPR"); break;
    case NULL_EXPR: printf("NULL_EXPR"); break;
    case NAT_LITERAL_EXPR: printf("NAT_LITERAL_EXPR(%s)", t->natVal); break;
    case TRUE_LITERAL_EXPR: printf("TRUE_LITERAL_EXPR"); break;
    case FALSE_LITERAL_EXPR: printf("FALSE_LITERAL_EXPR"); break;
    default: printError("unknown node type in printNodeTypeAndAttribute()");
  }  
}

/* print tree in preorder */
void printASTree(ASTree *t, int depth) {
  if(t==NULL) return;

  printf("%d:",depth);

  int i=0;
  for(; i<depth; i++) printf("  ");

  printNodeTypeAndAttribute(t);
  printf(" (ends on line %d)", t->lineNumber);
  printf("\n");
  //recursively print all children
  ASTList *childListIterator = t->children;
  while(childListIterator!=NULL) {
    //printf("iterating\n");
    printASTree(childListIterator->data, depth+1);
    childListIterator = childListIterator->next;
  }
}

/* Print the AST to stdout with indentations marking tree depth. */
void printAST(ASTree *t) { printASTree(t, 0); }
