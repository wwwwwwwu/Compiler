%{
	#include "include/syntaxtree.h"
	#define YYSTYPE syntax_node*
	#include "syntax.tab.h"
%}

digit	[0-9]
letter	[_a-zA-Z]
DELIM	[ \t]
octn	[0-7]
hexn	[0-9a-fA-F]

%option yylineno
%%
{DELIM}+							{}
\n								{}
0|[1-9]{digit}*							{/*yylval=atoi(yytext)*/;	return INT;}
0{octn}*							{int t;sscanf(yytext,"%o",&t);printf("oct %s %d \n",yytext,t);return INT;}
0x{hexn}+							{int t;sscanf(yytext,"%x",&t);printf("hex %s %d \n",yytext,t);return INT;}
(({digit}*\.{digit}+)|({digit}+\.))|((({digit}*\.{digit}+)|({digit}+\.?))[eE][+-]?{digit}+)	{
									printf("float %s %f \n",yytext,atof(yytext));
									return FLOAT;
								}
;								{yylval=init_syntax_node(yylineno,"SEMI");	return SEMI;}
,								{printf("COMMA \n");	return COMMA;}
=								{printf("ASSIGNOP \n");	return ASSIGNOP;}
>|<|>=|<=|==|!=							{printf("RELOP \n");	return RELOP;}
\+								{printf("PLUS \n");	return PLUS;}
-								{printf("MINUS \n");	return MINUS;}
\*								{printf("STAR \n");	return STAR;}
\/								{printf("DIV \n");	return DIV;}
&&								{printf("AND \n");	return AND;}
\|\|								{printf("OR \n");	return OR;}
\.								{printf("DOT \n");	return DOT;}
!								{printf("NOT \n");	return NOT;}
"int"|"float"							{printf("TYPE \n");	return TYPE;}
"("								{printf("LP \n");	return LP;}
")"								{printf("RP \n");	return RP;}
"["								{printf("LB \n");	return LB;}
"]"								{printf("RB \n");	return RB;}
"{"								{printf("LC \n");	return LC;}
"}"								{printf("RC \n");	return RC;}
struct								{printf("STRUCT \n");	return STRUCT;}
return								{printf("RETURN \n");	return RETURN;}
if								{printf("IF \n");	return IF;}
else								{printf("ELSE \n");	return ELSE;}
while								{printf("WHILE \n");	return WHILE;}
{letter}({letter}|{digit})*					{printf("ID \n");	return ID;}
.								{printf("Error type A at Line %d: Mysterious character '%s'\n",yylineno,yytext);}

%%

