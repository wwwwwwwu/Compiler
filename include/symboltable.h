#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "syntaxtree.h"
static int total_var_no;
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
	int no;
};

static struct symboltype *head , *tail;
struct symboltype* find_symbol(char* name);
static int No=0;
void print_sumbol();
int type_equal(Type t1,Type t2);
void insert_wr();
int insert_symbol(struct symboltype* a);
void symbol_init(){
	tail=malloc(sizeof(struct symboltype));
	head=tail;
	insert_wr();
}
void insert_wr(){
	Type t=malloc(sizeof(struct Type_));
	t->kind=BASIC;
	t->basic=0;
	FieldList read=malloc(sizeof(struct FieldList_));
	//strcpy(read->name,"");
	read->type=t;
	read->tail=NULL;
	Type r=malloc(sizeof(struct Type_));
	r->kind=FUNC;
	r->structure=read;
	struct symboltype *s=malloc (sizeof(struct symboltype));
	strcpy(s->name,"read");
	s->type=r;
	s->kind=FUNCTION;
	insert_symbol(s);



	struct symboltype *ss=malloc (sizeof(struct symboltype));
	
	FieldList write1=malloc(sizeof(struct FieldList_));
	//strcpy(write1->name,"");
	write1->type=t;
	write1->tail=NULL;

	FieldList write2=malloc(sizeof(struct FieldList_));
	//strcpy(write2->name,"");
	write2->type=t;
	write2->tail=write1;

	Type funt=malloc(sizeof(struct Type_));
	funt->kind=FUNC;
	funt->structure=write2;

	strcpy(ss->name,"write");
	ss->type=funt;
	ss->kind=FUNCTION;
	insert_symbol(ss);
}
int insert_symbol(struct symboltype* a){
	if(find_symbol(a->name)!=NULL)return 0;
	tail->next=a;
	tail=tail->next;
	tail->no=No;
	No++;
	tail->next=NULL;
	total_var_no=No;
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
int get_size(Type a)
{
	if (a->kind==BASIC)
	{
		return 4;
	}
	else if (a->kind==ARRAY)
	{
		return a->array.size*get_size(a->array.elem);
	}
	else if (a->kind==STRUCTURE)
	{
		FieldList p=a->structure;
		int  ret=0;
		for (;p!=NULL;p=p->tail)
			ret+=get_size(p->type);
		return ret;
	}
}

int get_structoff(Type a,char *s)
{
	int ret=0;
	FieldList p=a->structure;
	while (strcmp(p->name,s)!=0)
	{
		ret+=get_size(p->type);
		p=p->tail;
	}
	return ret;
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
