/* I pledge to my Honor that I have not cheated, and will not cheat, on this assignment Peter Stilian*/
/*Lexer for Diminishd Java*/

%option yylineno
%option noyywrap

%{
   #define DEBUG 1
   typedef YYTOKENTYPE Token;
   Token scanned(Token t);
%}

digit  [0-9]
id [a-zA-Z0-9]+
wspace [ \t\r\n]+

%%

class      {return scanned(CLASS);}
extends    {return scanned(EXTENDS);}
main       {return scanned(MAIN);}
nat        {return scanned(NATTYPE);}
bool       {return scanned(BOOLTYPE);}
true       {return scanned(TRUELITERAL);}
false      {return scanned(FALSELITERAL);}
&&         {return scanned(AND);}
!          {return scanned(NOT);}
if         {return scanned(IF);}
else       {return scanned(ELSE);}
for        {return scanned(FOR);}
{digit}    {return scanned(NATLITERAL);}
printNat   {return scanned(PRINTNAT);}
readNat    {return scanned(READNAT);}
"+"        {return scanned(PLUS);}
-          {return scanned(MINUS);}
"*"        {return scanned(TIMES);}
==         {return scanned(EQUALITY);}
"="        {return scanned(ASSIGN);}
">"        {return scanned(GREATER);}
NULL       {return scanned(NUL);}
instanceof {return scanned(INSTANCEOF);}
new        {return scanned(NEW);}
"."        {return scanned(DOT);}
";"        {return scanned(SEMICOLON);}
"{"        {return scanned(LBRACE);}
"}"        {return scanned(RBRACE);}
"("        {return scanned(LPAREN);}
")"        {return scanned(RPAREN);}
{id}       {return scanned(ID);}
{wspace}   {}
<<EOF>>    {return scanned(ENDOFFILE);}

%%

// print and return scanned Token
Token scanned(Token t){
    if(DEBUG == 0) return t;
    switch(t){
        case CLASS:        printf("CLASS "); return t;
        case EXTENDS:      printf("EXTENDS "); return t;
        case MAIN:         printf("MAIN "); return t;
        case NATTYPE:      printf("NATTYPE "); return t;
        case BOOLTYPE:     printf("BOOLTYPE "); return t;
        case TRUELITERAL:  printf("TRUELITERAL "); return t;
        case FALSELITERAL: printf("FALSELITERAL "); return t;
        case AND:          printf("AND "); return t;
        case NOT:          printf("NOT "); return t;
        case IF:           printf("IF "); return t;
        case ELSE:         printf("ELSE "); return t;
        case FOR:          printf("FOR" ); return t;
        case NATLITERAL:   printf("NATLITERAL(%s) ",yytext); return t;
        case PRINTNAT:     printf("PRINTNAT "); return t;
        case READNAT:      printf("READNAT "); return t;
        case PLUS:         printf("PLUS "); return t;
        case MINUS:        printf("MINUS "); return t;
        case TIMES:        printf("TIMES "); return t;
        case EQUALITY:     printf("EQUALITY "); return t;
        case ASSIGN:       printf("ASSIGN "); return t;
        case GREATER:      printf("GREATER "); return t;
        case NUL:          printf("NUL "); return t;
        case INSTANCEOF:   printf("INSTANCEOF "); return t;
        case NEW:          printf("NEW "); return t;
        case DOT:          printf("DOT "); return t;
        case SEMICOLON:    printf("SEMICOLON "); return t;
        case LBRACE:       printf("LBRACE "); return t;
        case RBRACE:       printf("RBRACE "); return t;
        case LPAREN:       printf("LPAREN "); return t;
        case RPAREN:       printf("RPAREN "); return t;
        case ID:           printf("ID(%s) ",yytext); return t;
        case ENDOFFILE:    printf("ENDOFFILE\n"); return t;
        default:           printf("ERROR: invalid token in scanned().\n"); exit(-1);
    }
}