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
0|[1-9]{digit}*							{yylval=init_syntax_node(yylineno,"INT","INT",atoi(yytext));	return INT;}
0{octn}*							{int t;sscanf(yytext,"%o",&t);yylval=init_syntax_node(yylineno,"INT","INT",t);return INT;}
0x{hexn}+							{int t;sscanf(yytext,"%x",&t);yylval=init_syntax_node(yylineno,"INT","INT",t);return INT;}
(({digit}*\.{digit}+)|({digit}+\.))|((({digit}*\.{digit}+)|({digit}+\.?))[eE][+-]?{digit}+)	
								{printf("%f",atof(yytext))
									yylval=init_syntax_floatnode(yylineno,"FLOAT",atof(yytext));
									return FLOAT;
								}
;								{yylval=init_syntax_node(yylineno,"SEMI",NULL);	return SEMI;}
,								{yylval=init_syntax_node(yylineno,"COMMA",NULL);	return COMMA;}
=								{yylval=init_syntax_node(yylineno,"ASSIGNOP",NULL);	return ASSIGNOP;}
>|<|>=|<=|==|!=							{yylval=init_syntax_node(yylineno,"RELOP",yytext);	return RELOP;}
\+								{yylval=init_syntax_node(yylineno,"PLUS",NULL);	return PLUS;}
-								{yylval=init_syntax_node(yylineno,"MINUS",NULL);	return MINUS;}
\*								{yylval=init_syntax_node(yylineno,"STAR",NULL);	return STAR;}
\/								{yylval=init_syntax_node(yylineno,"DIV",NULL);	return DIV;}
&&								{yylval=init_syntax_node(yylineno,"AND",NULL);	return AND;}
\|\|								{yylval=init_syntax_node(yylineno,"OR",NULL);	return OR;}
\.								{yylval=init_syntax_node(yylineno,"DOT",NULL);	return DOT;}
!								{yylval=init_syntax_node(yylineno,"NOT",NULL);	return NOT;}
"int"|"float"							{yylval=init_syntax_node(yylineno,"TYPE",yytext);	return TYPE;}
"("								{yylval=init_syntax_node(yylineno,"LP",NULL);	return LP;}
")"								{yylval=init_syntax_node(yylineno,"RP",NULL);	return RP;}
"["								{yylval=init_syntax_node(yylineno,"LB",NULL);	return LB;}
"]"								{yylval=init_syntax_node(yylineno,"RB",NULL);	return RB;}
"{"								{yylval=init_syntax_node(yylineno,"LC",NULL);	return LC;}
"}"								{yylval=init_syntax_node(yylineno,"RC",NULL);	return RC;}
struct								{yylval=init_syntax_node(yylineno,"STRUCT",NULL);	return STRUCT;}
return								{yylval=init_syntax_node(yylineno,"RETURN",NULL);	return RETURN;}
if								{yylval=init_syntax_node(yylineno,"IF",NULL);	return IF;}
else								{yylval=init_syntax_node(yylineno,"ELSE",NULL);	return ELSE;}
while								{yylval=init_syntax_node(yylineno,"WHILE",NULL);	return WHILE;}
{letter}({letter}|{digit})*					{yylval=init_syntax_node(yylineno,"ID",NULL);	return ID;}
.								{printf("Error type A at Line %d: Mysterious character '%s'\n",yylineno,yytext);}

%%

