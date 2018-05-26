#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__
#include "symboltable.h"
#include "printerror.h"
Type analysis_exp(syntax_node *p);
void analysis_tree(syntax_node *p,Type ret);
void analysis_vardec(syntax_node *p,Type t);
void analysis_extdef(syntax_node *p);
void analysis_def(syntax_node *p);
void analysis_stmt(syntax_node *p,Type ret);

void begin_semantic(syntax_node* syntax_h);
int match_args(syntax_node* p,FieldList flp);

Type logic_exp(syntax_node *p);
Type arith_exp(syntax_node *p);
Type dot_exp(syntax_node *p);
Type array_exp(syntax_node *p);
Type lvalue_exp(syntax_node *p);
Type assign_exp(syntax_node *p);

void def_dec_list(syntax_node *p,Type t);
void def_dec(syntax_node *p,Type t);
Type def_func(syntax_node *p,Type t);
void def_struct(syntax_node *p);

Type get_type_specifier(syntax_node *p);
int get_varlist_field(syntax_node *p,FieldList fld);
int get_declist_field(syntax_node *p,Type t,FieldList fld);
Type get_struct_type(syntax_node *p);
int get_vardec_field(syntax_node *p,Type t,FieldList fld);
int get_deflist_field(syntax_node *p,FieldList fld);

int get_deflist_field(syntax_node *p,FieldList fld)
{
	if (p->nr_child==0)
	{
		return 1;
	}	
	syntax_node *q=p->child[0];
	syntax_node *declist_p=q->child[2];
	Type t=get_type_specifier(q->child[0]);
	if (get_declist_field(declist_p,t,fld)==0)
		return 0;
	return get_deflist_field(p->child[1],fld);
}
int get_vardec_field(syntax_node *p,Type t,FieldList fld)
{
	if (p->nr_child==1)
	{
		char *s=p->child[0]->inf;
		FieldList i=fld;
		for (;i->tail!=NULL;i=i->tail)
		{
			if (strcmp(s,i->tail->name)==0)
			{
				print_error(15,p->lineno,s);
				return 0;
			}
		}
		FieldList f=malloc(sizeof(struct FieldList_));
		f->name=s;
		f->type=t;
		f->tail=NULL;
		i->tail=f;
		return 1;
	}
	else
	{
		syntax_node *q=p->child[2];
		Type arr=malloc(sizeof(struct Type_));
		arr->kind=ARRAY;
		arr->array.elem=t;
		arr->array.size=q->int_val;
		return get_vardec_field(p->child[0],arr,fld);
	}
}

void analysis_stmt(syntax_node *p,Type ret)
{
	syntax_node *q=p->child[0];
	if (p->nr_child==3)
	{
		Type ret_t=analysis_exp(p->child[1]);
		if (type_equal(ret_t,ret)==0)
		{
			print_error(8,q->lineno,"");
			return;
		}
		return;
	}
	else if (p->nr_child>=5)
	{
		q=p->child[1];
		Type ret=analysis_exp(p->child[2]);
		if (ret==NULL)
			return;
		if ((ret->kind!=BASIC||ret->basic!=0))
		{
			printf("error 20\n");
		//	print_error(20,q->lineno,"");  //................
			return;
		}
		q=p->child[2];
		for (int i=2;i<p->nr_child;i++)
		{
			analysis_tree(p->child[i],ret);
		}
		return;
	}
	else if (p->nr_child==1)//compst
	{
		analysis_tree(p->child[0],ret);
	}
	else //exp;
	{
		analysis_exp(p->child[0]);
	}
}

void def_struct(syntax_node *p)
{
	p=p->child[0];
	if (strcmp(p->symbol,"TYPE")!=0)
	{
		if (p->nr_child==5)
		{
			syntax_node *opttag_p=p->child[1];
			if (opttag_p->child[0]==NULL)
				return;
			syntax_node *q=p->child[3];
			Type t=get_struct_type(q);
			if (t==NULL)
				return;
			struct symboltype* s=malloc(sizeof(struct symboltype));
			strcpy(s->name,opttag_p->child[0]->inf);
			s->type=t;
			s->kind=STRUCTURE;
			int ret=insert_symbol(s);
			if (ret==0)
			{
				print_error(16,opttag_p->child[0]->lineno,opttag_p->child[0]->inf);
			}
		}
	}
}

Type get_struct_type(syntax_node *p)
{
	FieldList fld=malloc(sizeof (struct FieldList_));
	char *st=" ";
	fld->name=st;
	fld->type=NULL;
	fld->tail=NULL;
	if (get_deflist_field(p,fld)==0)
		return NULL;
	Type stru=malloc(sizeof (struct Type_));
	stru->kind=STRUC;
	stru->structure=fld->tail;
	return stru;
}

int get_declist_field(syntax_node *p,Type t,FieldList fld)
{
	syntax_node *q=p->child[0];
	if (q->nr_child==3)
	{
		while (q->child[0]!=NULL) q=q->child[0];
		print_error(15,q->lineno,q->inf);
		return 0;
	}
	syntax_node *vardec_p=q->child[0];
	if (get_vardec_field(vardec_p,t,fld)==1)
	{
		if (p->nr_child==3)
			return get_declist_field(p->child[2],t,fld);
		else
			return 1;	
	}
	else
		return 0;
}
int get_varlist_field(syntax_node *p,FieldList fld)
{
	syntax_node *q=p->child[0];
	syntax_node *r=q->child[0];
	Type t=get_type_specifier(r);
	int ret=get_vardec_field(q->child[1],t,fld);
	if (ret==0)
		return 0;
	if (p->nr_child==3)
		return get_varlist_field(p->child[2],fld);
}

Type def_func(syntax_node *p,Type t)
{
	if (p->nr_child==3)
	{
		FieldList fld=malloc(sizeof (struct FieldList_));
		char *p="return";
		fld->name=p;
		fld->type=t;
		fld->tail=NULL;
		Type fun=malloc(sizeof (struct Type_));
		fun->kind=FUNC;
		fun->structure=fld;
		return fun;
	}
	else
	{
		syntax_node *q=p->child[2];
		FieldList fld=malloc(sizeof (struct FieldList_));
		char *p="return";
		fld->name=p;
		fld->type=t;
		fld->tail=NULL;
		if (get_varlist_field(q,fld)==0)
			return NULL;
		FieldList i=fld->tail;
		for (;i!=NULL;i=i->tail)
		{
			struct symboltype* s=malloc(sizeof(struct symboltype));
			strcpy(s->name,i->name);
			s->type=i->type;
			s->kind=VARIBLE;
			insert_symbol(s);
		}
		Type fun=malloc(sizeof (struct Type_));
		fun->kind=FUNC;
		fun->structure=fld;
		return fun;
	}
}
void analysis_extdef(syntax_node *p)
{
	syntax_node* q=p->child[0];
	if (strcmp(p->child[1]->symbol,"ExtDecList")==0)
	{
		Type t=get_type_specifier(q);
		if (t==NULL)
			return;
		def_dec_list(p->child[1],t);
	}
	else if (strcmp(p->child[1]->symbol,"SEMI")==0)
	{
		def_struct(q);
	}
	else
	{
		Type fun_ret=get_type_specifier(q);
		if (fun_ret==NULL)
			return;
		q=p->child[1];
		Type t=def_func(q,fun_ret);
		if (t==NULL)
			return;
		struct symboltype* s=malloc(sizeof(struct symboltype));
		strcpy(s->name,q->child[0]->inf);
		s->type=t;
		s->kind=FUNC;
		int ret=insert_symbol(s);
		if (ret==0)
		{
			print_error(4,q->child[0]->lineno,q->child[0]->inf);
		}
		analysis_tree(p->child[2],fun_ret);	
	}
}
void analysis_vardec(syntax_node *p,Type t)
{
	if (p->nr_child==1)
	{
		struct symboltype* s=malloc(sizeof(struct symboltype));
		strcpy(s->name,p->child[0]->inf);
		s->type=t;
		s->kind=VARIBLE;
		int ret=insert_symbol(s);
		if (ret==0)
		{
			print_error(3,p->child[0]->lineno,p->child[0]->inf);
		}
	}
	else
	{
		syntax_node *q=p->child[2];
		Type arr=malloc(sizeof (struct Type_));
		arr->kind=ARRAY;
		arr->array.elem=t;
		arr->array.size=q->int_val;
		analysis_vardec(p->child[0],arr);
	}
}
void def_dec(syntax_node *p,Type t)
{
	syntax_node *q=p->child[0];
	if (p->nr_child==1)
	{
		analysis_vardec(q,t);
	}
	else
	{
		Type rt=analysis_exp(p->child[2]);
		if (rt==NULL)
			return;
		if (type_equal(rt,t)==0)
		{
			print_error(5,q->lineno,"");
			return;
		}
		analysis_vardec(q,t);
	}
}
void def_dec_list(syntax_node *p,Type t)
{
	if (strcmp(p->symbol,"DecList")==0)
	{
		syntax_node *q=p->child[0];
		def_dec(q,t);
		if (p->nr_child==3)
			def_dec_list(p->child[2],t);
	}
	else
	{
		syntax_node *q=p->child[0];
		analysis_vardec(q,t);
		if (p->nr_child==3)
			def_dec_list(p->child[2],t);
	}
}
Type get_type_specifier(syntax_node *p)
{
	syntax_node* q=p->child[0];
	if (strcmp(q->symbol,"TYPE")==0)
	{
		if (strcmp(q->inf,"int")==0){
			Type t=malloc(sizeof (struct Type_));
			t->kind=BASIC;
			t->basic=0;
			return t;
		}
		else{
			Type t=malloc(sizeof (struct Type_));
			t->kind=BASIC;
			t->basic=1;
			return t;
		}
	}
	else
	{
		if (q->nr_child==2)
		{
			syntax_node* qc=q->child[1]->child[0];
			struct symboltype* qq=find_symbol(qc->inf);
			if (qq==NULL||qq->kind!=STRUCTURE)
			{
				print_error(17,qc->lineno,qc->inf);
				return NULL;
			}
			return qq->type;
		}
		else
		{
			syntax_node *opttag_p=q->child[1];
			int opt=0;
			if (opttag_p->child[0]==NULL)
				opt=1;
			syntax_node *deflist_p=q->child[3];
			Type t=get_struct_type(deflist_p);
			if (t==NULL)
				return NULL;
			if (opt==0)
			{
				struct symboltype* s=malloc(sizeof(struct symboltype));
				strcpy(s->name,opttag_p->child[0]->inf);
				s->type=t;
				s->kind=STRUCTURE;
				int ret=insert_symbol(s);
				if (ret==0)
				{
					print_error(16,opttag_p->child[0]->lineno,opttag_p->child[0]->inf);
				}
			}
			return t;
		}
	}
	return NULL;
}
void analysis_def(syntax_node *p)
{
	syntax_node* q=p->child[0];
	Type t=get_type_specifier(q);
	if (t==NULL)
		return;
	q=p->child[1];
	def_dec_list(q,t);
}
Type assign_exp(syntax_node *p)
{
	syntax_node *q=p->child[0];
	Type lt=lvalue_exp(q);
	if (lt==NULL)
		return NULL;
	Type rt=analysis_exp(p->child[2]);
	if (rt==NULL)
		return NULL;
	if (type_equal(lt,rt)==0)
	{
		print_error(5,q->lineno,"");
		return NULL;
	}
	return lt;
}
Type lvalue_exp(syntax_node *p)
{
	syntax_node *q=p->child[0];
	if (p->nr_child==1&&strcmp(q->symbol,"ID")==0)
	{
		struct symboltype* pp=find_symbol(q->inf);
		if (pp==NULL||pp->kind!=VARIBLE)
		{
			print_error(1,q->lineno,q->inf);
			return NULL;
		}
		return pp->type;
	}
	else if (p->nr_child==3&&strcmp(q->symbol,"LP")==0)
	{
		return lvalue_exp(p->child[1]);
	}
	else if (p->nr_child==3&&strcmp(p->child[1]->symbol,"DOT")==0)
	{
		return dot_exp(p);
	}
	else if (p->nr_child==3&&strcmp(p->child[1]->symbol,"ASSIGNOP")==0)
	{
		return assign_exp(p);
	}
	else if (p->nr_child==4&&strcmp(q->symbol,"Exp")==0)
	{
		return array_exp(p);
	}
	else
	{
		print_error(6,q->lineno,"");
		return NULL;
	}
}
Type array_exp(syntax_node *p)
{
	syntax_node *q=p->child[0];
	Type t=analysis_exp(q);
	if (t==NULL)
		return NULL;
	if (t->kind!=ARRAY)
	{
		while (q->child[0]!=NULL) q=q->child[0];
		print_error(10,q->lineno,q->inf);
		return NULL;
	}
	q=p->child[2];
	Type offset=analysis_exp(q);
	if ((offset->kind!=BASIC||offset->basic!=0))
	{
		print_error(12,q->lineno,"");
		return NULL;
	}
	t=t->array.elem;
	return t;
}
Type dot_exp(syntax_node *p)
{
	syntax_node *q=p->child[0];
	Type t=analysis_exp(q);
	q=p->child[1];
	if (t==NULL)
		return NULL;
	if (t->kind!=STRUCTURE)
	{
		print_error(13,q->lineno,"");
		return NULL;
	}
	q=p->child[2];
	FieldList i=t->structure;
	for (;i!=NULL;i=i->tail)
	{
		if (strcmp(i->name,q->inf)==0)
			return i->type;
	}
	print_error(14,q->lineno,q->inf);
	return NULL;
}

Type arith_exp(syntax_node *p)
{
	syntax_node *q=p->child[0];
	if (p->nr_child==2)
	{
		q=p->child[1];
		Type ret=analysis_exp(q);
		if (ret==NULL)
			return NULL;
		if ((ret->kind!=BASIC||ret->basic!=0)&&(ret->kind!=BASIC||ret->basic!=1))
		{
			print_error(7,q->lineno,"");
			return NULL;
		}
		return ret;
	}
	else
	{
		Type ret=analysis_exp(q);
		if (ret==NULL)
			return NULL;
		if ((ret->kind!=BASIC||ret->basic!=0)&&(ret->kind!=BASIC||ret->basic!=1))
		{
			print_error(7,q->lineno,"");
			return NULL;
		}
		Type ret2=analysis_exp(p->child[2]);
		if (ret2==NULL)
			return NULL;
		if (ret2!=ret)
		{
			print_error(7,p->child[2]->lineno,"");
			return NULL;
		}

		return ret;
	}
}
Type logic_exp(syntax_node *p)
{
	syntax_node *q=p->child[0];
	if (p->nr_child==2)
	{
		q=p->child[1];
		Type ret=analysis_exp(q);
		if (ret==NULL)
			return NULL;
		if (ret->kind!=BASIC||ret->basic!=0)
		{
			print_error(7,q->lineno,"");
			return NULL;
		}
		return ret;
	}
	else
	{
		Type ret=analysis_exp(q);
		if (ret==NULL)
			return NULL;
		if (ret->kind!=BASIC||&ret->basic!=0)
		{
			print_error(7,q->lineno,"");
			return NULL;
		}
		Type ret2=analysis_exp(p->child[2]);
		if (ret2==NULL)
			return NULL;
		if (ret2->kind!=BASIC||ret2->basic!=0)
		{
			print_error(7,p->child[2]->lineno,"");
			return NULL;
		}

		Type t=malloc(sizeof (struct Type_));
		t->kind=BASIC;
		t->basic=0;
		return t;
	}
}


int match_args(syntax_node* p,FieldList flp)
{
	if (flp==NULL)
	{
		return 0;
	}
	for(int i=0;i<p->nr_child;i+=2){
		if (type_equal(analysis_exp(p->child[i]),flp->type)==0)
		{
			return 0;
		}
		if (flp->tail!=NULL)return 0;
		flp=flp->tail;
	}
	return 1;
}

Type analysis_exp(syntax_node *p)
{
	if (p->nr_child==1)
	{
		syntax_node* q=p->child[0];
		if (strcmp(q->symbol,"INT")==0){
			Type t=malloc(sizeof (struct Type_));
			t->kind=BASIC;
			t->basic=0;
			return t;
		}
		else if (strcmp(q->symbol,"FLOAT")==0){
			Type t=malloc(sizeof (struct Type_));
			t->kind=BASIC;
			t->basic=0;
			return t;
		}
		else
		{
			struct symboltype* pp=find_symbol(q->inf);
			if (pp==NULL||pp->kind!=VARIBLE)
			{
				print_error(1,q->lineno,q->inf);
				return NULL;
			}
			return pp->type;
		}
	}
	else if (p->nr_child==2)
	{
		if (strcmp((p->child[0])->symbol,"NOT")==0)
			return logic_exp(p);
		else
			return arith_exp(p);
	}
	else if (p->nr_child==3)
	{
		if (strcmp((p->child[0])->symbol,"Exp")==0)
		{
			syntax_node* pp=p->child[1];
			if (strcmp(pp->symbol,"ASSIGNOP")==0)
			{
				return assign_exp(p);
			}
			else if (strcmp(pp->symbol,"DOT")==0)
			{
				return dot_exp(p);
			}
			else
			{
				return arith_exp(p);
			}
		}
		else
		{
			syntax_node* q=p->child[0];
			if (strcmp(q->symbol,"LP")==0)
				return analysis_exp(p->child[1]);
			else
			{
				struct symboltype* pp=find_symbol(q->inf);
				if (pp==NULL)
				{
					print_error(2,q->lineno,q->inf);
					return NULL;
				}
				if (pp->kind!=FUNC)
				{
					print_error(11,q->lineno,q->inf);
					return NULL;
				}
				if (pp->type->structure->tail==NULL)
					return pp->type->structure->type;
				else
				{
					print_error(9,q->lineno,q->inf);
					return NULL;
				}
			}
		}
	}
	else
	{
		syntax_node* q=p->child[0];
		if (strcmp(q->symbol,"ID")==0)
		{
			struct symboltype* pp=find_symbol(q->inf);
			if (pp==NULL)
			{
				print_error(2,q->lineno,q->inf);
				return NULL;
			}
			if (pp->kind!=FUNC)
			{
				print_error(11,q->lineno,q->inf);
				return NULL;
			}
			int ret=match_args(p->child[2],pp->type->structure->tail);
			if (ret==1)
				return pp->type->structure->type;
			print_error(9,q->lineno,q->inf);
			return NULL;
		}
		else
		{
			return array_exp(p);
		}
	}
}

void analysis_tree(syntax_node *p,Type ret)
{
printf("%s\n",p->symbol);
	if (p==NULL)
	{
		printf("NULL\n");
		return;
	}
	if (strcmp(p->symbol,"Exp")==0)
	{
	//	printf("Exp\n");
		analysis_exp(p);
	//	printf("finishExp\n");
	}
	else if (strcmp(p->symbol,"Def")==0)
	{
	//	printf("Def\n");
		analysis_def(p);
	//	printf("finishDEF\n");
	}
	else if (strcmp(p->symbol,"ExtDef")==0)
	{
	//	printf("ExtDef\n");
		analysis_extdef(p);
	//	printf("finishEXTDEF\n");
	}
	else if (strcmp(p->symbol,"Stmt")==0)
	{
	//	printf("Stmt\n");
		analysis_stmt(p,ret);
	//	printf("finishSTMT\n");
	}
	else
	{//printf("else %d\n",p->nr_child);
		for (int i=0;i<p->nr_child;i++)
		{
			analysis_tree(p->child[i],ret);
		}
	}

}

void begin_semantic(syntax_node* p)
{
//	printf("begin_semantic\n");
	symbol_init();
//	printf("symbol_init\n");
	analysis_tree(p,NULL);
	printf("semantic finished\n");
}
#endif
