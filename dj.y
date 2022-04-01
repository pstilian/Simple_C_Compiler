// I pledge my Honor that I have not cheated, and will not cheat, on this assignment Peter Stilian
/* DJ PARSER */

%code provides {
  #include "lex.yy.c"
 
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
%left DOT
%right NOT

%start pgm

%%

pgm : blocks ENDOFFILE 
	{ return 0; }
    ;

blocks: classlist mainclass
       ;

classlist: classlist classdeclaration
         | classdeclaration
         |
         ;

classdeclaration: CLASS ID EXTENDS ID LBRACE vardeclist methdeclist RBRACE
                | CLASS ID EXTENDS ID LBRACE methdeclist RBRACE
                | CLASS ID EXTENDS ID LBRACE vardeclist RBRACE
                | CLASS ID EXTENDS ID LBRACE RBRACE
                ;

vardeclist: vardeclaration
          | vardeclist vardeclaration
          ;

vardeclaration: type ID SEMICOLON
              | type ID ASSIGN expr SEMICOLON
              | STATIC type ID SEMICOLON
              | STATIC type ID ASSIGN expr SEMICOLON
              ;

methdeclist: methdeclaration
           | methdeclist methdeclaration
           ;

methdeclaration: type ID LPAREN type ID RPAREN varexprblock
               ;

mainclass: MAIN varexprblock
         ;

type: NATTYPE
    | BOOLTYPE
    | ID
    ;

varexprblock: LBRACE vardeclist exprlist RBRACE
            | LBRACE exprlist RBRACE
            | LBRACE RBRACE
            ;

exprlist: expr SEMICOLON
        | exprlist exprlist
        ;

expr: expr AND expr
    | expr GREATER expr
    | expr PLUS expr
    | expr MINUS expr
    | expr EQUALITY expr
    | expr TIMES expr
    | expr DOT ID
    | ID ASSIGN expr
    | expr DOT ID ASSIGN expr
    | ID LPAREN expr RPAREN
    | expr DOT ID LPAREN expr RPAREN
    | booleanliteral
    | ID
    | NOT expr
    | LPAREN expr RPAREN
    | NEW ID LPAREN RPAREN
    | NATLITERAL
    | NUL
    | IF LPAREN expr RPAREN varexprblock
    | IF LPAREN expr RPAREN varexprblock ELSE varexprblock
    | FOR LPAREN expr SEMICOLON expr SEMICOLON expr RPAREN varexprblock
    | THIS 
    | expr INSTANCEOF ID
    | PRINTNAT LPAREN expr RPAREN
    | READNAT LPAREN RPAREN
    ;

booleanliteral: TRUELITERAL
              | FALSELITERAL
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
