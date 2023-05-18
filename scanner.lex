%{

/* Declarations section */
#include <stdio.h>
#include "output.hpp"
#include "parser.tab.hpp"

%}

%option yylineno
%option noyywrap
whitespace		([\t\n\r ])
comment         (\/\/[^\r\n]*[\r\n|\r\n]?)

%%

void                                                        yyval = new Type(yytext); return VOID;
int                                                         yyval = new Type(yytext); return INT;
byte                                                        yyval = new Type(yytext); return BYTE;
b                                                           yyval = new Type(yytext); return B;
bool                                                        yyval = new Type(yytext); return BOOL;
override                                                    yyval = new Node(yytext); return OVERRIDE;
and                                                         yyval = new Node(yytext); return AND;
or                                                          yyval = new Node(yytext); return OR;
not                                                         yyval = new Node(yytext); return NOT;
true                                                        yyval = new Node(yytext); return TRUE;
false                                                       yyval = new Node(yytext); return FALSE;
return                                                      yyval = new Node(yytext); return RETURN;
if                                                          yyval = new Node(yytext); return IF;
else                                                        yyval = new Node(yytext); return ELSE;
while                                                       yyval = new Node(yytext); return WHILE;
break                                                       yyval = new Node(yytext); return BREAK;
continue                                                    yyval = new Node(yytext); return CONTINUE;
;                                                           yyval = new Node(yytext); return SC; 
,                                                           yyval = new Node(yytext); return COMMA;   
\(                                                          yyval = new Node(yytext); return LPAREN;  
\)                                                          yyval = new Node(yytext); return RPAREN;  
\{                                                          yyval = new Node(yytext); return LBRACE;  
\}                                                          yyval = new Node(yytext); return RBRACE;  
=                                                           yyval = new Node(yytext); return ASSIGN;  
==|!=|<|>|<=|>=                                             yyval = new Node(yytext); return RELOP;   
\+                                                          yyval = new Node(yytext); return PLUS;
\-                                                          yyval = new Node(yytext); return MINUS;
\*                                                          yyval = new Node(yytext); return MULT;
\/                                                          yyval = new Node(yytext); return DIV;
[a-zA-Z][a-zA-Z0-9]*                                        yyval = new Node(yytext); return ID;  
0|[1-9][0-9]*                                               yyval = new Node(yytext); return NUM; 
\"([^\n\r\"\\]|\\[rnt"\\])+\"                               yyval = new Node(yytext); return STRING;  
{whitespace}|{comment}				                        ;
.		                                                    {output::errorLex(yylineno); exit(0);}

%%