#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "syntaxtree.h"
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
enum symbolkind {VARIBLE,FUNCTION,STRUCTURE};
struct Type_{
	enum {BASIC, ARRAY, STRUC, FUNC} kind;
	union{
		int basic;
		struct{Type elem;int size;}array;
		FieldList structure;
	};
};

struct FieldList_
{
	char* name;
	Type type;
	FieldList tail;
};

struct symboltype
{
	enum symbolkind kind;
	Type type;
	char name[64];
	struct symboltype* next;
};

struct symboltype *head , *tail;
struct symboltype* find_symbol(char* name);
void print_sumbol();
int type_equal(Type t1,Type t2);
int insert_symbol(struct symboltype* a);
void symbol_init(){
	tail=malloc(sizeof(struct symboltype));
	head=tail;
}

int insert_symbol(struct symboltype* a){
print_symbol();
	if(find_symbol(a->name)!=NULL)return 0;
	tail->next=a;
	tail=tail->next;
	tail->next=NULL;
	return 1;
}
void print_symbol(){
	if(head->next==tail)return;
	struct symboltype* p = head->next;
	while(p!=NULL){
		p=p->next;
	}
}
struct symboltype* find_symbol(char* name){
	struct symboltype* p = head->next;
	if(p==NULL){
		return NULL;
	}
	while(strcmp(name,p->name)!=0){
		p=p->next;
		if(p==NULL){
			return NULL;
		}
	}
	return p;
}

int type_equal(Type t1,Type t2)
{
	if (t1==t2)
		return 1;
	if (t1->kind==t2->kind)
	{
		if (t1->kind==BASIC){
			return (t1->basic==t2->basic);
		}
		else if (t1->kind==ARRAY)
		{
			return type_equal(t1->array.elem,t2->array.elem);
		}
		else
		{
			FieldList f1=t1->structure;
			FieldList f2=t2->structure;
			while (f1!=NULL&&f2!=NULL)
			{
				if (type_equal(f1->type,f2->type)==0)
					return 0;
				f1=f1->tail;
				f2=f2->tail;
			}
			if (f1!=NULL||f2!=NULL)
				return 0;
			return 1;
		}
	}
	return 0;
}
/*

void delete_table(symboltype a){
	symboltype p=head;
	if(symbol_eq(a,p)){
		head=p.next;
		return;
	}
	while(p.next!=tail){
		if(symbol_eq(a,p.next)){
			p.next=p.next.next;
			return;
		}
		p=p.next;
	}
//	printf("can't find symbol %s",a.name);
}
*/

#endif
