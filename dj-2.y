// I pledge my Honor that I have not cheated, and will not cheat, on this assignment Peter Stilian
/* DJ PARSER */

%code provides {
  #include "lex.yy.c"
  #include <stdio.h>
  #include "ast.h"

  #define YYSTYPE ASTree *

  ASTree *pgmAST;
 
  /* Function for printing generic syntax-error messages */
  void yyerror(const char *str) {
    printf("Syntax error on line %d at token %s\n",yylineno,yytext);
    printf("(This version of the compiler exits after finding the first ");
    printf("syntax error.)\n");
    exit(-1);
  }
}

%token CLASS ID EXTENDS MAIN NATTYPE BOOLTYPE
%token TRUELITERAL FALSELITERAL AND NOT IF ELSE FOR
%token NATLITERAL PRINTNAT READNAT PLUS MINUS TIMES EQUALITY GREATER
%token STATIC ASSIGN NUL NEW THIS DOT INSTANCEOF
%token SEMICOLON LBRACE RBRACE LPAREN RPAREN
%token ENDOFFILE

%right ASSIGN
%left AND
%nonassoc EQUALITY
%nonassoc GREATER
%left PLUS MINUS
%left TIMES
%right NOT
%nonassoc INSTANCEOF
%left DOT


%start pgm

%%

pgm : classlist MAIN LBRACE vardeclist exprlist RBRACE ENDOFFILE 
	{ pgmAST = newAST(PROGRAM, NULL, 0, NULL, yylineno);
	  pgmAST = appendToChildrenList(pgmAST, $1);
	  pgmAST = appendToChildrenList(pgmAST, $4);
	  pgmAST = appendToChildrenList(pgmAST, $5);
      
      printAST(pgmAST);

	  return 0;
	}
    ;

classlist: classlist classdeclaration
         {$$ = appendToChildrenList($1, $2);}
         |
         {$$ = newAST(CLASS_DECL_LIST, NULL, 0, NULL, yylineno);}
         ;

classdeclaration: CLASS id EXTENDS id LBRACE staticdeclist vardeclist methodMaybe RBRACE
                {$$ = newAST(CLASS_DECL, NULL, 0, NULL, yylineno);
                 $$ = appendToChildrenList($$, $2);
                 $$ = appendToChildrenList($$, $4);
                 $$ = appendToChildrenList($$, $6);
                 $$ = appendToChildrenList($$, $7);
                 $$ = appendToChildrenList($$, $8);
                 }
                ;

staticdeclist: staticdeclist staticvardec 
             {$$ = appendToChildrenList($1, $2);}
             |
             {$$ = newAST(STATIC_VAR_DECL_LIST, NULL, 0, NULL, yylineno);}
             ;

staticvardec: STATIC type id SEMICOLON
            {$$ = newAST(STATIC_VAR_DECL, $2, 0, NULL, yylineno);
             $$ = appendToChildrenList($$, $3);}
            ;

vardeclist: vardeclist vardeclaration SEMICOLON
          {$$ = appendToChildrenList($1, $2);}
          |
          {$$ = newAST(VAR_DECL_LIST, NULL, 0, NULL, yylineno);}
          ;

vardeclaration: type id
              {$$ = newAST(VAR_DECL, $1, 0, NULL, yylineno);
               $$ = appendToChildrenList($$, $2);}
              ;

methodMaybe: methdeclist
           {$$ = $1;}
           |
           {$$ = newAST(METHOD_DECL_LIST, NULL, 0, NULL, yylineno);}
           ;

methdeclist: methdeclist methdeclaration
           {$$ = appendToChildrenList($1, $2);}
           |methdeclaration
           {$$ = newAST(METHOD_DECL_LIST, $1, 0, NULL, yylineno);}
           ;

methdeclaration: type id LPAREN type id RPAREN LBRACE vardeclist exprlist RBRACE
               {$$ = newAST(METHOD_DECL, $1, 0, NULL, yylineno);
                $$ = appendToChildrenList($$, $2);
                $$ = appendToChildrenList($$, $4);
                $$ = appendToChildrenList($$, $5);
                $$ = appendToChildrenList($$, $8);
                $$ = appendToChildrenList($$, $9);}
               ;

type: NATTYPE
    {$$ = newAST(NAT_TYPE, NULL, 0, NULL, yylineno);}
    | BOOLTYPE
    {$$ = newAST(BOOL_TYPE, NULL, 0, NULL, yylineno);}
    | id
    {$$ = $1;}
    ;

exprlist: exprlist expr SEMICOLON {$$ = appendToChildrenList($1, $2);}
        |expr SEMICOLON {$$ = newAST(EXPR_LIST, $1, 0, NULL, yylineno);}
        ;

id: ID
   {$$ = newAST(AST_ID, NULL, 0, yytext, yylineno);}
   ;

expr: expr AND expr
    {$$ = newAST(AND_EXPR, $1, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $3);}
    | expr GREATER expr
    {$$ = newAST(GREATER_THAN_EXPR, $1, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $3);}
    | expr PLUS expr
    {$$ = newAST(PLUS_EXPR, $1, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $3);}
    | expr MINUS expr
    {$$ = newAST(MINUS_EXPR, $1, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $3);}
    | expr EQUALITY expr
    {$$ = newAST(EQUALITY_EXPR, $1, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $3);}
    | expr TIMES expr
    {$$ = newAST(TIMES_EXPR, $1, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $3);}
    | expr DOT id
    {$$ = newAST(DOT_ID_EXPR, $1, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $3);}
    | id ASSIGN expr
    {newAST(ASSIGN_EXPR, NULL, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $1);
     $$ = appendToChildrenList($$, $3);}
    | expr DOT id ASSIGN expr
    {$$ = newAST(DOT_ASSIGN_EXPR, $1, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $3);
     $$ = appendToChildrenList($$, $5);}
    | expr DOT id LPAREN expr RPAREN
    {$$ = newAST(DOT_METHOD_CALL_EXPR, $1, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $3);
     $$ = appendToChildrenList($$, $5);}
    | id LPAREN expr RPAREN
    {$$ = newAST(METHOD_CALL_EXPR, NULL, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $1);
     $$ = appendToChildrenList($$, $3);}
    | NOT expr
    {$$ = newAST(NOT_EXPR, $2, 0, NULL, yylineno);}
    | LPAREN expr RPAREN
    {$$ = appendToChildrenList($$, $2);}
    | NEW id LPAREN RPAREN
    {$$ = newAST(NEW_EXPR, NULL, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $2);}
    | NATLITERAL
    {$$ = newAST(NAT_LITERAL_EXPR, NULL, atoi(yytext), NULL, yylineno);}
    | TRUELITERAL
    {$$ = newAST(TRUE_LITERAL_EXPR, NULL, 0, yytext, yylineno);}
    | FALSELITERAL
    {$$ = newAST(FALSE_LITERAL_EXPR, NULL, 0, yytext, yylineno);}
    | NUL
    {$$ = newAST(NULL_EXPR, NULL, 0, NULL, yylineno);}
    | IF LPAREN expr RPAREN LBRACE vardeclist exprlist RBRACE ELSE LBRACE vardeclist exprlist RBRACE
    {$$ = newAST(IF_THEN_ELSE_EXPR, $3, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $6);
     $$ = appendToChildrenList($$, $7);
     $$ = appendToChildrenList($$, $11);
     $$ = appendToChildrenList($$, $12);}
    | FOR LPAREN expr SEMICOLON expr SEMICOLON expr RPAREN LBRACE vardeclist exprlist RBRACE
    {$$ = newAST(FOR_EXPR, $3, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $5);
     $$ = appendToChildrenList($$, $7);
     $$ = appendToChildrenList($$, $10);
     $$ = appendToChildrenList($$, $11);}
    | THIS
    {$$ = newAST(THIS_EXPR, NULL, 0, NULL, yylineno);}
    | expr INSTANCEOF id
    {$$ = newAST(INSTANCEOF_EXPR, $1, 0, NULL, yylineno);
     $$ = appendToChildrenList($$, $3);}
    | PRINTNAT LPAREN expr RPAREN
    {$$ = newAST(PRINT_EXPR, $1, 0, yytext, yylineno);
     $$ = appendToChildrenList($$, $3);}
    | READNAT LPAREN RPAREN
    {$$ = newAST(READ_EXPR, NULL, 1, NULL, yylineno);}
    |id
    {$$ = newAST(ID_EXPR, $1, 0, yytext, yylineno);}
    ;

%%

int main(int argc, char **argv) {
  if(argc!=2) {
    printf("Usage: dj-parse filename\n");
    exit(-1);
  }
  yyin = fopen(argv[1],"r");
  if(yyin==NULL) {
    printf("ERROR: could not open file %s\n",argv[1]);
    exit(-1);
  }
  /* parse the input program */
  return yyparse();
}
