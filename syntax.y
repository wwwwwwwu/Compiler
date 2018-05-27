%{
	#include "lex.yy.c"
	#include <stdio.h>
	#include "include/syntaxtree.h"
	#include "include/semantic.h"
	int noerror=1;
	void missing_error(char *str)
	{
		noerror=0;
		fprintf(stderr,"Error type B at Line %d:Missing \"%s\"\n",yylineno,str);
	}
%}
%token INT FLOAT ID
%token SEMI COMMA TYPE STRUCT RETURN IF WHILE
%right ASSIGNOP
%left OR AND RELOP PLUS MINUS STAR DIV
%right NOT NEG
%left LP RP LB RB LC RC DOT
%nonassoc LOWER_THAN_ELSE 
%nonassoc ELSE 
%nonassoc noer misser
%%
Program	: 	ExtDefList {$$=init_syntax_child_node("Program",1,$1); if(noerror==1){begin_semantic($$);}} 
	   	;
ExtDefList	: /*e*/{$$=init_syntax_child_node("ExtDefList",0,NULL);}
		| ExtDef ExtDefList {$$=init_syntax_child_node("ExtDefList",2,$1,$2);} 
		;
ExtDef : 	Specifier ExtDecList SEMI %prec noer{$$=init_syntax_child_node("ExtDef",3,$1,$2,$3);}
		| Specifier SEMI %prec noer{$$=init_syntax_child_node("ExtDef",2,$1,$2);}
		| Specifier FunDec CompSt {$$=init_syntax_child_node("ExtDef",3,$1,$2,$3);}
		| Specifier %prec misser {missing_error(";");}
		| Specifier ExtDecList %prec misser {missing_error(";");} 
		;
ExtDecList : 	VarDec {$$=init_syntax_child_node("ExtDecList",1,$1);}
		| VarDec COMMA ExtDecList {$$=init_syntax_child_node("ExtDecList",3,$1,$2,$3);}
		;
Specifier : 	TYPE  {$$=init_syntax_child_node("Specifier",1,$1);}
		| StructSpecifier {$$=init_syntax_child_node("Specifier",1,$1);}
		;
StructSpecifier : STRUCT OptTag LC DefList RC {$$=init_syntax_child_node("StructSpecifier",5,$1,$2,$3,$4,$5);}
		| STRUCT Tag {$$=init_syntax_child_node("StructSpecifier",2,$1,$2);}
		;
OptTag : 	/*e*/{$$=init_syntax_child_node("OptTag",0,NULL);}
		| ID {$$=init_syntax_child_node("OptTag",1,$1);}
		;
Tag : 		ID {$$=init_syntax_child_node("Tag",1,$1);}
		;
VarDec : 	ID {$$=init_syntax_child_node("VarDec",1,$1);}
		| VarDec LB INT RB {$$=init_syntax_child_node("VarDec",4,$1,$2,$3,$4);}
		;
FunDec : 	ID LP VarList RP {$$=init_syntax_child_node("FunDec",4,$1,$2,$3,$4);}
		| ID LP RP {$$=init_syntax_child_node("FunDec",3,$1,$2,$3);}
		;
VarList : 	ParamDec COMMA VarList {$$=init_syntax_child_node("VarList",3,$1,$2,$3);}
		| ParamDec {$$=init_syntax_child_node("VarList",1,$1);}
		;
ParamDec : 	Specifier VarDec {$$=init_syntax_child_node("ParamDec",2,$1,$2);}
		;
CompSt : 	LC DefList StmtList RC {$$=init_syntax_child_node("CompSt",4,$1,$2,$3,$4);}
		;
StmtList : 	/*e*/ {$$=init_syntax_child_node("StmtList",0,NULL);}
		| Stmt StmtList {$$=init_syntax_child_node("StmtList",2,$1,$2);}
		;
Stmt : 		Exp SEMI {$$=init_syntax_child_node("Stmt",2,$1,$2);}
		| CompSt {$$=init_syntax_child_node("Stmt",1,$1);}
		| RETURN Exp SEMI {$$=init_syntax_child_node("Stmt",3,$1,$2,$3);}
		| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=init_syntax_child_node("Stmt",5,$1,$2,$3,$4,$5);}
		| IF LP Exp RP Stmt ELSE Stmt {$$=init_syntax_child_node("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
		| WHILE LP Exp RP Stmt {$$=init_syntax_child_node("Stmt",5,$1,$2,$3,$4,$5);}
		;
DefList : 	/*e*/{$$=init_syntax_child_node("DefList",0,NULL);}
		| Def DefList {$$=init_syntax_child_node("DefList",2,$1,$2);}
		;
Def : 		Specifier DecList SEMI {$$=init_syntax_child_node("Def",3,$1,$2,$3);}
		;
DecList : 	Dec {$$=init_syntax_child_node("DecList",1,$1);}
		| Dec COMMA DecList {$$=init_syntax_child_node("DecList",3,$1,$2,$3);}
		;
Dec : 		VarDec {$$=init_syntax_child_node("Dec",1,$1);}
		| VarDec ASSIGNOP Exp {$$=init_syntax_child_node("Dec",3,$1,$2,$3);}
		;
Exp: 		Exp ASSIGNOP Exp {$$=init_syntax_child_node("Exp",3,$1,$2,$3);}
		| Exp AND Exp {$$=init_syntax_child_node("Exp",3,$1,$2,$3);}
		| Exp OR Exp {$$=init_syntax_child_node("Exp",3,$1,$2,$3);}
		| Exp RELOP Exp {$$=init_syntax_child_node("Exp",3,$1,$2,$3);}
		| Exp PLUS Exp {$$=init_syntax_child_node("Exp",3,$1,$2,$3);}
		| Exp MINUS Exp {$$=init_syntax_child_node("Exp",3,$1,$2,$3);}
		| Exp STAR Exp {$$=init_syntax_child_node("Exp",3,$1,$2,$3);}
		| Exp DIV Exp {$$=init_syntax_child_node("Exp",3,$1,$2,$3);}
		| LP Exp RP {$$=init_syntax_child_node("Exp",3,$1,$2,$3);}
		| MINUS Exp %prec NEG {$$=init_syntax_child_node("Exp",2,$1,$2);}
		| NOT Exp {$$=init_syntax_child_node("Exp",2,$1,$2);}
		| ID LP Args RP{$$=init_syntax_child_node("Exp",4,$1,$2,$3,$4);}
		| ID LP RP {$$=init_syntax_child_node("Exp",3,$1,$2,$3);}
		| Exp LB Exp RB {$$=init_syntax_child_node("Exp",4,$1,$2,$3,$4);}
		| Exp DOT ID{$$=init_syntax_child_node("Exp",3,$1,$2,$3);}
		| ID {$$=init_syntax_child_node("Exp",1,$1);}
		| INT {$$=init_syntax_child_node("Exp",1,$1);}
		| FLOAT {$$=init_syntax_child_node("Exp",1,$1);}
		;
Args : 		Exp COMMA Args {$$=init_syntax_child_node("Args",3,$1,$2,$3);}
		| Exp {$$=init_syntax_child_node("Args",1,$1);}
		;
%%
yyerror(){
	noerror=0;
	fprintf(stderr,"Error type B at Line %d:syntax error\n",yylineno);
}


