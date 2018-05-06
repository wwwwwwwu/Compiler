#ifndef __SYNTAXTREE_H__
#define __SYNTAXTREE_H__

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct syntax_tree_node {
	char symbol[31];
	char is_terminal;
	int lineno;
	int nr_child;
	union{
		int int_val;
		float float_val;
		char inf[256];
	};
	struct syntax_tree_node* child[10];
}syntax_node;


syntax_node* init_syntax_node(int,char[],char*,...);
syntax_node* init_syntax_floatnode(int,char[],float);
syntax_node* init_syntax_child_node(char[],int,...);
void print_tree(syntax_node*,int,int);
syntax_node* init_syntax_node(int lineno,char symbol[],char* text,...) {
	syntax_node* p=malloc(sizeof(syntax_node));
	strcpy(p->symbol,symbol);
	p->lineno=lineno;
	p->is_terminal=1;
	p->int_val=0;
	p->nr_child=0;
/*
	va_list valist;
	va_start(valist,symbol);
	char* text=va_arg(valist,char*);
	va_end(valist);
	strcpy(p->inf,text);
	printf("%s\n",text);
*/
	if(text=="INT"){
		va_list valist;
		va_start(valist,text);
		p->int_val=va_arg(valist,int);
//		printf("int a %d",p->int_val);
		va_end(valist);
//		printf("int b %d",p->int_val);
	}
	else if(text!=NULL){
		strcpy(p->inf,text);
//		printf("%s\n",text);
	}
	
	return p;
}

syntax_node* init_syntax_floatnode(int lineno,char symbol[],float value) {
//	printf("2fffffffffffffffffffff\n");
	syntax_node* p=malloc(sizeof(syntax_node));
	strcpy(p->symbol,symbol);
	p->lineno=lineno;
	p->is_terminal=1;
	p->nr_child=0;
	p->float_val=value;
	return p;
}

syntax_node* init_syntax_child_node(char symbol[],int nr_child,...) {
	syntax_node* p=malloc(sizeof(syntax_node));
	strcpy(p->symbol,symbol);
	p->is_terminal=0;
	p->nr_child=nr_child;
	va_list ap;
	va_start(ap,nr_child);
	int i;
	if(nr_child==0)return p;
	syntax_node* q=va_arg(ap,syntax_node*);
	p->child[0]=q;
	p->lineno=p->child[0]->lineno;
	for (i=1;i<nr_child;i++)
	{
//		printf("child %d %d\n",i,nr_child);
		q=va_arg(ap,syntax_node*);
		p->child[i]=q;
//		printf("lineon %d\n",p->child[i]->lineno);
	}
	return p;
}

void print_tree(syntax_node *p,int nr_blank,int noerror) {
//	printf("print\n");
	if (noerror==0)
		return;
	int i=0;
//	printf("print %d %d\n",p->is_terminal,p->nr_child);
	if (p->is_terminal==0&&p->nr_child==0)
		return;
	for (;i<nr_blank;i++)
	{
		printf("  ");
	}
	printf("%s",p->symbol);
	if ((strcmp(p->symbol,"TYPE")==0)||(strcmp(p->symbol,"ID")==0))
		printf(": %s",p->inf);
	else if (strcmp(p->symbol,"INT")==0)
		printf(": %d",p->int_val);
	else if (strcmp(p->symbol,"FLOAT")==0)
		printf(": %f",p->float_val);
	if (p->is_terminal==0)
		printf(" (%d)",p->lineno);	
	printf("\n");
	int n=0;
	for (;n<p->nr_child;n++)
	{
		print_tree(p->child[n],nr_blank+1,1);
	}
}
#endif
