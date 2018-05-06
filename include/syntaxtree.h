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


syntax_node* init_syntax_node(int,char[]);

syntax_node* init_syntax_node(int lineno,char symbol[]) {
	syntax_node* p=malloc(sizeof(syntax_node));
	strcpy(p->symbol,symbol);
	p->lineno=lineno;
	p->is_terminal=1;
	p->int_val=0;
	p->nr_child=0;
	return p;
}

#endif
