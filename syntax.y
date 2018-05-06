%{
	#include "lex.yy.c"
	#include <stdio.h>
	#include "include/syntaxtree.h"
%}
%token INT FLOAT ID
%token SEMI COMMA TYPE STRUCT RETURN IF WHILE
%right ASSIGNOP
%left OR AND RELOP PLUS MINUS STAR DIV
%right NOT NEG
%left LP RP LB RB LC RC DOT
%nonassoc LOWER_THAN_ELSE 
%nonassoc ELSE 
%%
Program	: 	ExtDefList 
	   	;
ExtDefList	: /*e*/
		| ExtDef ExtDefList 
		;
ExtDef : 	Specifier ExtDecList SEMI 
		| Specifier SEMI 
		| Specifier FunDec CompSt 
		;
ExtDecList : 	VarDec 
		| VarDec COMMA ExtDecList 
		;
Specifier : 	TYPE  
		| StructSpecifier 
		;
StructSpecifier : STRUCT OptTag LC DefList RC 
		| STRUCT Tag 
		;
OptTag : 	/*e*/
		| ID 
		;
Tag : 		ID 
		;
VarDec : 	ID 
		| VarDec LB INT RB 
		;
FunDec : 	ID LP VarList RP 
		| ID LP RP 
		;
VarList : 	ParamDec COMMA VarList 
		| ParamDec 
		;
ParamDec : 	Specifier VarDec 
		;
CompSt : 	LC DefList StmtList RC 
		;
StmtList : 	/*e*/ 
		| Stmt StmtList 
		;
Stmt : 		Exp SEMI 
		| CompSt 
		| RETURN Exp SEMI 
		| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
		| IF LP Exp RP Stmt ELSE Stmt 
		| WHILE LP Exp RP Stmt 
		;
DefList : 	/*e*/
		| Def DefList 
		;
Def : 		Specifier DecList SEMI 
		| error SEMI
		;
DecList : 	Dec 
		| Dec COMMA DecList 
		;
Dec : 		VarDec 
		| VarDec ASSIGNOP Exp 
		;
Exp: 		Exp ASSIGNOP Exp 
		| Exp AND Exp 
		| Exp OR Exp 
		| Exp RELOP Exp 
		| Exp PLUS Exp 
		| Exp MINUS Exp 
		| Exp STAR Exp 
		| Exp DIV Exp 
		| LP Exp RP 
		| MINUS Exp 
		| NOT Exp 
		| ID LP Args RP
		| ID LP RP 
		| Exp LB Exp RB 
		| Exp DOT ID
		| ID 
		| INT 
		| FLOAT 
		;
Args : 		Exp COMMA Args 
		| Exp 
		;
%%
yyerror(){
	fprintf(stderr,"Error type B at Line %d:syntax error\n",yylineno);
}


