#ifndef __IR_H__
#define __IR_H__
#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"
#define NEW_CODE(ret,k,t,t1,t2) \
	if (ret!=NULL)\
		ret->lastnode=new_exp_code(ret->lastnode,k,t,t1,t2);\
	else\
	{\
		ret=new_cp(new_exp_code(NULL,k,t,t1,t2));\
	}
#define NEW_JUMP(ret,k,t,t1,t2) \
	if (ret!=NULL)\
		ret->lastnode=new_jump_code(ret->lastnode,k,t,t1,t2);\
	else\
	{\
		ret=new_cp(new_jump_code(NULL,k,t,t1,t2));\
	}
static int nowtemp=0;
static int nowlabel=0;
typedef struct Operand_* Operand;
typedef int Label;
struct Operand_
{
	enum {OP_VARIABLE,OP_INT,OP_FLOAT,OP_ADDRESS,OP_ARRAY,OP_STRUCTURE,OP_TEMP,OP_FUNC,OP_LABEL,OP_ADDRVAR} kind;
	union {
		int var_no;
		Label label_no;
		int int_value;
		float float_value;
		struct symboltype* sym;
		Type arr_type;
	};
};
enum IC_kind {	IC_LABEL,IC_FUNCTION,IC_ASSIGN,IC_ADD,IC_SUB,IC_MUL,
		IC_DIV,IC_GETADDR,IC_GETVAL,IC_ADDRVAL,IC_GOTO,IC_IF,
		IC_RETURN,IC_DEC,IC_ARG,IC_ARGADDR,IC_CALL,IC_PARAM,IC_READ,IC_WRITE};
enum IC_relop {MORE,LESS,NOGREATER,NOLESS,EQUAL,NOTEQ};
typedef struct InterCode_
{
	enum IC_kind kind;
	union {
		struct {Operand target,op1,op2;} binop;
		struct {
			Operand target; 
			Operand op1,op2;
			enum IC_relop relop;
		}jump;
	};
}InterCode;

typedef struct InterCodes_* InterCodes;
struct InterCodes_
{
	InterCode code;
	struct InterCodes_ *prev,*next;
};

typedef struct CodesPointer_* CodesPointer;
struct CodesPointer_
{
	InterCodes firstnode,lastnode;
};

//----------------------------------------------------
static Operand opd_zero=NULL,opd_one=NULL;
Operand new_int(int i)
{
	Operand p=malloc(sizeof(struct Operand_));
	p->kind=OP_INT;
	p->int_value=i;
	return p;
}
void change2int(Operand p,int i)
{
	p->kind=OP_INT;
	p->int_value=i;
}
Operand new_float(float i)
{
	Operand p=malloc(sizeof(struct Operand_));
	p->kind=OP_FLOAT;
	p->float_value=i;
	return p;
}
void change2float(Operand p,float i)
{
	p->kind=OP_FLOAT;
	p->float_value=i;
}
Operand new_temp()
{
	Operand p=malloc(sizeof(struct Operand_));
	p->kind=OP_TEMP;
	p->var_no=nowtemp;
	nowtemp++;
	return p;
}

enum IC_relop get_relop(char *s)
{
	if (s[0]=='=')
	{
		return EQUAL;
	}
	else if (s[0]=='>')
	{
		if (s[1]=='\0')
			return MORE;
		else
			return NOLESS;
	}
	else if (s[0]=='<')
	{
		if (s[1]=='\0')
			return LESS;
		else
			return NOGREATER;
	}
	else
		return NOTEQ;
}

void change2var(Operand p,struct symboltype *s)
{
	if (s->kind==VARIBLE)
	{
		if (s->type->kind==BASIC)
			p->kind=OP_VARIABLE;
		else if (s->type->kind==FUNC)
			p->kind=OP_FUNC;
		else
			p->kind=OP_ADDRVAR;
	}
	else if (s->kind==FUNCTION)
		p->kind=OP_FUNC;
	p->sym=s;
}
void change2addrtemp(Operand p)
{
	p->kind=OP_ADDRESS;
}
int new_label()
{
	int t;
	t=nowlabel;
	nowlabel++;
	return t;
}
Operand new_labelopd()
{
	Operand p=malloc(sizeof(struct Operand_));
	p->kind=OP_LABEL;
	p->label_no=new_label();
	return p;
}
CodesPointer merge_cp(CodesPointer p1,CodesPointer p2)
{
	if (p1==NULL)
	{
		return p2;
	}
	if (p2==NULL)
	{
		return p1;
	}
	p1->lastnode->next=p2->firstnode;
	p2->firstnode->prev=p1->lastnode;
	p1->lastnode=p2->lastnode;
	free(p2);
	return p1;
}
CodesPointer new_cp(InterCodes ics)
{
	CodesPointer p=malloc(sizeof(struct CodesPointer_));
	p->firstnode=p->lastnode=ics;
	return p;
}

Operand new_var(struct symboltype* s)
{
	Operand p=malloc(sizeof(struct Operand_));
	if (s->kind==VARIBLE)
	{
		if (s->type->kind==BASIC)
			p->kind=OP_VARIABLE;
		else if (s->type->kind==FUNC)
			p->kind=OP_FUNC;
		else
			p->kind=OP_ADDRVAR;
	}
	else if (s->kind==FUNCTION)
		p->kind=OP_FUNC;
	else if (s->kind==PARADDR)
		p->kind=OP_VARIABLE;
	p->sym=s;
	return p;
}

InterCodes new_exp_code(InterCodes pr,enum IC_kind kind,Operand target,Operand op1,Operand op2)
{
	InterCodes p=malloc(sizeof(struct InterCodes_));
	if (pr!=NULL)
	{
		pr->next=p;
	}
	p->prev=pr;
	p->next=NULL;
	p->code.kind=kind;
	p->code.binop.target=target;
	p->code.binop.op1=op1;
	p->code.binop.op2=op2;
	return p;
}
InterCodes new_jump_code(InterCodes pr,enum IC_relop relop,Operand target,Operand op1,Operand op2)
{
	InterCodes p=malloc(sizeof(struct InterCodes_));
	if (pr!=NULL)
	{
		pr->next=p;
	}
	p->prev=pr;
	p->next=NULL;
	p->code.kind=IC_IF;
	p->code.jump.target=target;
	p->code.jump.op1=op1;
	p->code.jump.op2=op2;
	p->code.jump.relop=relop;
	return p;
}
CodesPointer get_straddr(syntax_node *p,Operand t);
CodesPointer get_addr(syntax_node *p,Operand t,Type *type);

CodesPointer translate_exp(syntax_node *p,Operand t);
CodesPointer translate_logic(syntax_node *p,Operand t);
CodesPointer translate_cond(syntax_node *p,Operand ltrue,Operand lfalse);
CodesPointer translate_assign(syntax_node *p,Operand t);
CodesPointer translate_dot(syntax_node *p,Operand t);
CodesPointer translate_arith_exp(syntax_node *p,enum IC_kind ic_kind,Operand t);
CodesPointer translate_call(syntax_node *p,Operand t);
CodesPointer translate_arr(syntax_node *p,Operand t);
CodesPointer translate_tree(syntax_node *p);
CodesPointer translate_stmt(syntax_node *p);
CodesPointer translate_if(syntax_node *p);
CodesPointer translate_if_else(syntax_node *p);
CodesPointer translate_while(syntax_node *p);
CodesPointer translate_vardec(syntax_node *p,Operand t);
CodesPointer translate_fundec(syntax_node *p);
CodesPointer translate_param(syntax_node *p);
CodesPointer translate_param(syntax_node *p)
{
	CodesPointer ret=NULL;
	syntax_node *vardec_p=p->child[0]->child[1];
	if (vardec_p->nr_child==1)
	{
		syntax_node *id_p=vardec_p->child[0];
		Operand t1=new_var(find_symbol(id_p->inf));
		NEW_CODE(ret,IC_PARAM,t1,NULL,NULL);
		if (p->nr_child==1)
		{
			return ret;
		}
		else
			return merge_cp(ret,translate_param(p->child[2]));
	}
	else
	{
		syntax_node *id_p;
		while (vardec_p->nr_child!=1)
		{
				vardec_p=vardec_p->child[0];
		}
		id_p=vardec_p->child[0];
		Operand t1=new_var(find_symbol(id_p->inf));
		NEW_CODE(ret,IC_PARAM,t1,NULL,NULL);
		if (p->nr_child==1)
		{
			return ret;
		}
		else
			return merge_cp(ret,translate_param(p->child[2]));
	}
}	

CodesPointer translate_fundec(syntax_node *p)
{
	CodesPointer ret=NULL;
	Operand t1=new_var(find_symbol(p->child[0]->inf));	
	NEW_CODE(ret,IC_FUNCTION,t1,NULL,NULL);
	if (p->nr_child==3)
	{
		return ret;
	}
	else
	{
		return merge_cp(ret,translate_param(p->child[2]));
	}
	
}
CodesPointer translate_vardec(syntax_node *p,Operand t)
{
	if (p->nr_child==1)
	{
		struct symboltype *q=find_symbol(p->child[0]->inf);
		if (q->kind==VARIBLE&&(q->type->kind==ARRAY||q->type->kind==STRUC))
		{
			int size=get_size(q->type);
			Operand t1=new_int(size);
			change2var(t,q);
			CodesPointer ret=NULL;
			NEW_CODE(ret,IC_DEC,t,t1,NULL)
			return ret;
		}
		free(t);
		return NULL;
	}
	else
	{
		return translate_vardec(p->child[0],t);
	}
}
CodesPointer translate_while(syntax_node *p)
{
	Operand t1,t2,t3;
	t1=new_labelopd();
	t2=new_labelopd();
	t3=new_labelopd();
	CodesPointer ret=NULL,code1=NULL,code2=NULL;
	code1=translate_cond(p->child[2],t2,t3);
	code2=translate_stmt(p->child[4]);
	NEW_CODE(ret,IC_LABEL,t1,NULL,NULL);
	ret=merge_cp(ret,code1);
	NEW_CODE(ret,IC_LABEL,t2,NULL,NULL);
	ret=merge_cp(ret,code2);
	NEW_CODE(ret,IC_GOTO,t1,NULL,NULL);
	NEW_CODE(ret,IC_LABEL,t3,NULL,NULL);
	return ret;
}
CodesPointer translate_if(syntax_node *p)
{
	Operand t1,t2;
	t1=new_labelopd();
	t2=new_labelopd();
	CodesPointer code1=NULL,code2=NULL;
	code1=translate_cond(p->child[2],t1,t2);
	code2=translate_stmt(p->child[4]);
	NEW_CODE(code1,IC_LABEL,t1,NULL,NULL);
	NEW_CODE(code2,IC_LABEL,t2,NULL,NULL);
	return merge_cp(code1,code2);
}

CodesPointer translate_if_else(syntax_node *p)
{
	Operand t1,t2,t3;
	t1=new_labelopd();
	t2=new_labelopd();
	t3=new_labelopd();
	CodesPointer code1=NULL,code2=NULL,code3=NULL;
	code1=translate_cond(p->child[2],t1,t2);
	code2=translate_stmt(p->child[4]);
	code3=translate_stmt(p->child[6]);
	NEW_CODE(code1,IC_LABEL,t1,NULL,NULL);
	NEW_CODE(code2,IC_GOTO,t3,NULL,NULL);
	NEW_CODE(code2,IC_LABEL,t2,NULL,NULL);
	NEW_CODE(code3,IC_LABEL,t3,NULL,NULL);
	code1=merge_cp(code1,code2);
	return merge_cp(code1,code3);
}

CodesPointer translate_stmt(syntax_node *p)
{
	syntax_node *q=p->child[0];
	if (p->nr_child==2)
	{
		Operand t=new_temp();
		CodesPointer ret=translate_exp(q,t);
		return ret;
	}
	else if(p->nr_child==1)
	{
		return translate_tree(q);
	}
	else if(p->nr_child==3)
	{
		q=p->child[1];
		Operand t=new_temp();
		CodesPointer ret=translate_exp(q,t);
		NEW_CODE(ret,IC_RETURN,t,NULL,NULL);
		return ret;
	}
	else if (strcmp(q->symbol,"IF")==0)
	{
		if (p->nr_child==5)
		{
			return translate_if(p);
		}
		else
		{
			return translate_if_else(p);
		}
	}
	else 
	{
		return translate_while(p);
	}
}

CodesPointer translate_args(syntax_node *p,CodesPointer* arg_code)
{
	syntax_node *q=p->child[0];
	Operand t=new_temp();
	Type tye=analysis_exp(q);
	CodesPointer ret;
	if (tye->kind==BASIC)
		ret=translate_exp(q,t);
	else
	{
		ret=get_addr(q,t,&tye);	
	}	
	if (p->nr_child==3)
	{
		ret=merge_cp(ret,translate_args(p->child[2],arg_code));
	}
	NEW_CODE((*arg_code),IC_ARG,t,NULL,NULL);
	return ret;
}
CodesPointer translate_arr(syntax_node *p,Operand t)
{
	CodesPointer ret=NULL;
	Operand t1=new_temp();
	Type temp;
	ret=get_addr(p,t1,&temp);
	NEW_CODE(ret,IC_GETVAL,t,t1,NULL);
	return ret;
}
CodesPointer translate_call(syntax_node *p,Operand t)
{
	syntax_node *q=p->child[0];
	CodesPointer ret=NULL;
	if (strcmp(q->inf,"read")==0)
	{
		NEW_CODE(ret,IC_READ,t,NULL,NULL);
		return ret;
	}
	else if (strcmp(q->inf,"write")==0)
	{
		Operand t1=new_temp();
		q=p->child[2]->child[0];
		ret=translate_exp(q,t1);
		NEW_CODE(ret,IC_WRITE,t1,NULL,NULL);
		change2int(t,1);
		return ret;
	}
	Operand t1=new_var(find_symbol(q->inf));
	if (p->nr_child==4)
	{
		CodesPointer args_code=NULL;
		ret=merge_cp(ret,translate_args(p->child[2],&args_code));
		ret=merge_cp(ret,args_code);
	}
	NEW_CODE(ret,IC_CALL,t,t1,NULL);
	return ret;
}

CodesPointer translate_arith_exp(syntax_node *p,enum IC_kind ic_kind,Operand t)
{
	Operand t1=new_temp();
	Operand t2=new_temp();
	CodesPointer ret1=translate_exp(p->child[0],t1);
	CodesPointer ret2=translate_exp(p->child[2],t2);
	if (t1->kind==OP_INT&&t2->kind==OP_INT)
	{
		t->kind=OP_INT;
		switch(ic_kind)
		{
			case IC_ADD:t->int_value=t1->int_value+t2->int_value;break;
			case IC_SUB:t->int_value=t1->int_value-t2->int_value;break;
			case IC_MUL:t->int_value=t1->int_value*t2->int_value;break;
			case IC_DIV:t->int_value=t1->int_value/t2->int_value;break;			
		}
		free(t1);
		free(t2);
		return NULL;
	}
	else if (t1->kind==OP_FLOAT&&t2->kind==OP_FLOAT)
	{
		t->kind=OP_FLOAT;
		switch(ic_kind)
		{
			case IC_ADD:t->float_value=t1->float_value+t2->float_value;break;
			case IC_SUB:t->float_value=t1->float_value-t2->float_value;break;
			case IC_MUL:t->float_value=t1->float_value*t2->float_value;break;
			case IC_DIV:t->float_value=t1->float_value/t2->float_value;break;			
		}
		free(t1);
		free(t2);
		return NULL;
	}
	else if (t1->kind==OP_INT&&t1->int_value==0&&(ic_kind==IC_ADD))
	{
		t->kind=t2->kind;
		t->int_value=t2->int_value;
		return ret2;
	}
	else if (t2->kind==OP_INT&&t2->int_value==0&&(ic_kind==IC_ADD||ic_kind==IC_SUB))
	{
		t->kind=t1->kind;
		t->int_value=t1->int_value;
		return ret1;
	}
	else if (t1->kind==OP_INT&&t1->int_value==1&&(ic_kind==IC_MUL))
	{
		t->kind=t2->kind;
		t->int_value=t2->int_value;
		return ret2;
	}
	else if (t2->kind==OP_INT&&t2->int_value==1&&(ic_kind==IC_MUL||ic_kind==IC_DIV))
	{
		t->kind=t1->kind;
		t->int_value=t1->int_value;
		return ret1;
	}
	else
	{
		CodesPointer ret=merge_cp(ret1,ret2);
		NEW_CODE(ret,ic_kind,t,t1,t2);
		return ret;
	}
}
CodesPointer get_addr(syntax_node *p,Operand t,Type *type)
{
	if (p->nr_child==3)
	{
		*type=analysis_exp(p);
		return get_straddr(p,t);
	}
	syntax_node *q1=p->child[0];
	CodesPointer ret=NULL;
	Operand t1,t2,t3;
	if (p->nr_child==1)
	{
		struct symboltype *s=find_symbol(q1->inf);
		change2var(t,s);
		*type=s->type;
		return ret;
	}
	t1=new_temp();
	t2=new_temp();
	t3=new_temp();
	syntax_node *q2=p->child[2];
	CodesPointer offcode=translate_exp(q2,t3);
	ret=get_addr(q1,t1,type);
	Operand ts=new_int(get_size((*type)->array.elem));
	if (t3->kind==OP_INT)
	{
		change2int(t2,t3->int_value*ts->int_value);
		free(t3);
		free(ts);
	}
	else
	{
		NEW_CODE(offcode,IC_MUL,t2,t3,ts);
	}
	ret=merge_cp(ret,offcode);
	if (t2->kind==OP_INT&&t2->int_value==0)
	{
		t->kind=t1->kind;
		t->int_value=t1->int_value;
		free(t2);
		*type=(*type)->array.elem;
		return ret;
	}
	NEW_CODE(ret,IC_ADD,t,t1,t2);
	*type=(*type)->array.elem;
	return ret;

}

CodesPointer get_straddr(syntax_node *p,Operand t)
{
	Operand t1=new_temp();
	CodesPointer ret=NULL;	
	syntax_node *q=p->child[0];
	Type type=analysis_exp(q);
	int r=get_structoff(type,p->child[2]->inf);
	if (r!=0)
	{
		Type temp;
		ret=get_addr(q,t1,&temp);
		Operand t2=new_int(r);	
		NEW_CODE(ret,IC_ADD,t,t1,t2)
		change2addrtemp(t);
		return ret;
	}
	else
	{
		Type temp;
		ret=get_addr(q,t,&temp);
		return ret;
	}
}
CodesPointer translate_dot(syntax_node *p,Operand t)
{
	CodesPointer ret=NULL;
	Operand t1=new_temp();
	ret=get_straddr(p,t1);
	NEW_CODE(ret,IC_GETVAL,t,t1,NULL);
	return ret;
}
CodesPointer translate_assign(syntax_node *p,Operand t)
{
	CodesPointer ret=NULL;
	if (strcmp(p->child[0]->child[0]->symbol,"ID")==0)
	{
		Operand t1=new_var(find_symbol(p->child[0]->child[0]->inf));
		Operand t2=new_temp();
		ret=merge_cp(ret,translate_exp(p->child[2],t2));
		NEW_CODE(ret,IC_ASSIGN,t1,t2,NULL);
		change2var(t,t1->sym);
		return ret;
	}
	else if (p->child[0]->nr_child==4)
	{
		Operand t1=new_temp();
		Operand t2=new_temp();
		Type temp;
		ret=get_addr(p,t1,&temp);
		ret=merge_cp(ret,translate_exp(p->child[2],t2));
		NEW_CODE(ret,IC_ADDRVAL,t1,t2,NULL);
		t->kind=t2->kind;
		t->int_value=t2->int_value;
		return ret;
	}
	else if (p->child[0]->nr_child==3)
	{
		Operand t1=new_temp();
		Operand t2=new_temp();
		ret=get_straddr(p,t1);
		ret=merge_cp(ret,translate_exp(p->child[2],t2));
		NEW_CODE(ret,IC_ADDRVAL,t1,t2,NULL);
		t->kind=t2->kind;
		t->int_value=t2->int_value;
		return ret;
	}
}

CodesPointer translate_cond(syntax_node *p,Operand ltrue,Operand lfalse)
{
	syntax_node *q=p->child[0];
	if (p->nr_child==3&&strcmp(q->symbol,"LP")==0)
	{
		return translate_cond(p->child[1],ltrue,lfalse);
	}
	else if (p->nr_child==2&&strcmp(q->symbol,"NOT")==0)
	{
		return translate_cond(p->child[1],lfalse,ltrue);
	}
	else if (p->nr_child==3&&strcmp(q->symbol,"Exp")==0)
	{
		syntax_node *p2=p->child[1];
		if (strcmp(p2->symbol,"RELOP")==0)
		{
			Operand t1=new_temp();
			Operand t2=new_temp();
			CodesPointer code1,code2;
			code1=translate_exp(q,t1);
			code2=translate_exp(p->child[2],t2);
			enum IC_relop r=get_relop(p2->inf);
			code1=merge_cp(code1,code2);
			NEW_JUMP(code1,r,ltrue,t1,t2);
			NEW_CODE(code1,IC_GOTO,lfalse,NULL,NULL);
			return code1;
		}
		else if (strcmp(p2->symbol,"AND")==0)
		{
			Operand l1=new_labelopd();
			CodesPointer code1,code2;
			code1=translate_cond(q,l1,lfalse);
			code2=translate_cond(p->child[2],ltrue,lfalse);
			NEW_CODE(code1,IC_LABEL,l1,NULL,NULL);
			code1=merge_cp(code1,code2);
			return code1;
		}
		else if (strcmp(p2->symbol,"OR")==0)
		{
			Operand l1=new_labelopd();
			CodesPointer code1,code2;
			code1=translate_cond(q,ltrue,l1);
			code2=translate_cond(p->child[2],ltrue,lfalse);
			NEW_CODE(code1,IC_LABEL,l1,NULL,NULL);
			code1=merge_cp(code1,code2);
			return code1;
		}
	}
	Operand t1=new_temp();
	CodesPointer code1,code2;
	code1=translate_exp(p,t1);
	Operand t2=opd_zero;
	NEW_JUMP(code1,NOTEQ,ltrue,t1,t2);
	NEW_CODE(code1,IC_GOTO,lfalse,NULL,NULL);
	return code1;
}
CodesPointer translate_logic(syntax_node *p,Operand t)
{
	Operand t1,t2,t3,int1,int0;
	t1=new_labelopd();
	t2=new_labelopd();
	t3=new_labelopd();
	int1=opd_one;
	int0=opd_zero;
	CodesPointer code1=NULL,code2=NULL,code3=NULL,ret=NULL;
	code1=translate_cond(p,t1,t2);
	NEW_CODE(code2,IC_ASSIGN,t,int1,NULL);
	NEW_CODE(code3,IC_ASSIGN,t,int0,NULL);
	NEW_CODE(code1,IC_LABEL,t1,NULL,NULL);
	NEW_CODE(code2,IC_GOTO,t3,NULL,NULL);
	NEW_CODE(code2,IC_LABEL,t2,NULL,NULL);
	NEW_CODE(code3,IC_LABEL,t3,NULL,NULL);
	code1=merge_cp(code1,code2);
	return merge_cp(code1,code3);
} 

CodesPointer translate_exp(syntax_node *p,Operand t) 
{
	syntax_node *q=p->child[0];
	CodesPointer ret=NULL;
	if (p->nr_child==1)
	{
		if (strcmp(q->symbol,"INT")==0)
		{
			change2int(t,q->int_val);
			return NULL;
		}
		else if (strcmp(q->symbol,"FLOAT")==0)
		{
			change2float(t,q->float_val);
			return NULL;
		}
		else
		{
			struct symboltype* s=find_symbol(q->inf);
			change2var(t,s);
			return NULL;
		}
	}
	else if (p->nr_child==2)
	{
		if (strcmp(q->symbol,"NOT")==0)
		{
			return translate_logic(p,t);
		}
		else
		{
			Operand t1=new_temp();
			ret=translate_exp(p->child[1],t1);
			if (t1->kind==OP_INT)
			{
				t->int_value=-t1->int_value;
				t->kind=OP_INT;
				free(t1);
			}
			else if (t1->kind==OP_FLOAT)
			{
				t->float_value=-t1->float_value;
				t->kind=OP_FLOAT;
				free(t1);
			}
			else
			{
				NEW_CODE(ret,IC_SUB,t,opd_zero,t1);
			}
			return ret;
		}
	}
	else if (p->nr_child==3)
	{
		if (strcmp(q->symbol,"Exp")==0)
		{
			syntax_node *ope_p=p->child[1];
			if (strcmp(ope_p->symbol,"ASSIGNOP")==0)
			{
				return translate_assign(p,t);
			}
			else if (strcmp(ope_p->symbol,"DOT")==0)
			{
				return translate_dot(p,t);
			}
			else if (strcmp(ope_p->symbol,"AND")==0)
			{
				return translate_logic(p,t);
			}
			else if (strcmp(ope_p->symbol,"OR")==0)
			{
				return translate_logic(p,t);
			}
			else if (strcmp(ope_p->symbol,"RELOP")==0)
			{
				return translate_logic(p,t);
			}
			else if (strcmp(ope_p->symbol,"PLUS")==0)
			{
				return translate_arith_exp(p,IC_ADD,t);
			}
			else if (strcmp(ope_p->symbol,"MINUS")==0)
			{
				return translate_arith_exp(p,IC_SUB,t);
			}
			else if (strcmp(ope_p->symbol,"STAR")==0)
			{
				return translate_arith_exp(p,IC_MUL,t);
			}	
			else if (strcmp(ope_p->symbol,"DIV")==0)
			{
				return translate_arith_exp(p,IC_DIV,t);
			}	

		}
		else
		{
			if (strcmp(q->symbol,"LP")==0)
			{
				return translate_exp(p->child[1],t);
			}
			else
			{
				return translate_call(p,t);
			}
		}
	}
	else
	{
		if (strcmp(q->symbol,"Exp")==0)
		{
			return translate_arr(p,t);	
		}
		else
		{
			return translate_call(p,t);
		}

	}	
}

CodesPointer translate_tree(syntax_node *p)
{
	CodesPointer ret=NULL;
	if (p==NULL)
	{
		return NULL;
	}
	if (strcmp(p->symbol,"Exp")==0)
	{printf("11\n");
		Operand t=new_temp();
		return translate_exp(p,t);
	}
	else if (strcmp(p->symbol,"Stmt")==0)
	{printf("12\n");
		return translate_stmt(p);
	}
	else if (strcmp(p->symbol,"Dec")==0)
	{
		Operand t=new_temp();
		if (p->nr_child!=3)
			return translate_vardec(p->child[0],t);
		return translate_assign(p,t);
	}
	else if (strcmp(p->symbol,"VarDec")==0)
	{printf("14\n");
		Operand t=new_temp();
		return translate_vardec(p,t);
	}
	else if (strcmp(p->symbol,"FunDec")==0)
	{printf("15\n");
		return translate_fundec(p);
	}
	else if (strcmp(p->symbol,"StructSpecifier")==0)
	{printf("16\n");
		return NULL;
	}
	else
	{printf("17\n");
		int i=0;
		for (;i<p->nr_child;i++)
		{
			ret=merge_cp(ret,translate_tree(p->child[i]));
		}
		return ret;
	}

}

//---------------------------------------
void fprint_opd(FILE *f,Operand p)
{
	switch(p->kind)
	{
		case OP_ADDRVAR:fprintf(f,"&");
		case OP_VARIABLE:fprintf(f,"v%d",p->sym->no);break;
		case OP_FUNC:fprintf(f,"%s",p->sym->name);break;
		case OP_ADDRESS:
		case OP_TEMP:fprintf(f,"t%d",p->var_no);break;
		case OP_INT:fprintf(f,"#%d",p->int_value);break;
		case OP_LABEL:fprintf(f,"l%d",p->label_no);break;
	}
}
void fprint_staropd(FILE *f,Operand p)
{
	switch(p->kind)
	{
		case OP_ADDRVAR:fprintf(f,"v%d",p->sym->no);break;
		case OP_VARIABLE:fprintf(f,"*v%d",p->sym->no);break;
		case OP_ADDRESS:
		case OP_TEMP:fprintf(f,"*t%d",p->var_no);break;
		case OP_INT:fprintf(f,"*#%d",p->int_value);break;
	}
}
void fprint_relop(FILE *f,InterCode p)
{
	switch (p.jump.relop)
	{
		case MORE:fprintf(f,">");break;
		case LESS:fprintf(f,"<");break;
		case NOGREATER:fprintf(f,"<=");break;
		case NOLESS:fprintf(f,">=");break;
		case EQUAL:fprintf(f,"==");break;
		case NOTEQ:fprintf(f,"!=");break;
	}
}

void fprint_code(FILE *f,InterCode p)
{
	switch (p.kind)
	{
		case IC_LABEL:
			fprintf(f,"LABEL\t");
			fprint_opd(f,p.binop.target);
			fprintf(f,"\t:\n");
			break;
		case IC_FUNCTION:
			fprintf(f,"FUNCTION\t%s\t:\n",p.binop.target->sym->name);
			break;
		case IC_ASSIGN:
			fprint_opd(f,p.binop.target);
			fprintf(f,"\t:=\t");
			fprint_opd(f,p.binop.op1);
			fprintf(f,"\n");
			break;
		case IC_ADD:
			fprint_opd(f,p.binop.target);
			fprintf(f,"\t:=\t");
			fprint_opd(f,p.binop.op1);
			fprintf(f,"\t+\t");
			fprint_opd(f,p.binop.op2);
			fprintf(f,"\n");
			break;
		case IC_SUB:
			fprint_opd(f,p.binop.target);
			fprintf(f,"\t:=\t");
			fprint_opd(f,p.binop.op1);
			fprintf(f,"\t-\t");
			fprint_opd(f,p.binop.op2);
			fprintf(f,"\n");
			break;
		case IC_MUL:
			fprint_opd(f,p.binop.target);
			fprintf(f,"\t:=\t");
			fprint_opd(f,p.binop.op1);
			fprintf(f,"\t*\t");
			fprint_opd(f,p.binop.op2);
			fprintf(f,"\n");
			break;
		case IC_DIV:
			fprint_opd(f,p.binop.target);
			fprintf(f,"\t:=\t");
			fprint_opd(f,p.binop.op1);
			fprintf(f,"\t/\t");
			fprint_opd(f,p.binop.op2);
			fprintf(f,"\n");
			break;
		case IC_GETADDR:
			fprint_opd(f,p.binop.target);
			fprintf(f,"\t:=\t&");
			fprint_opd(f,p.binop.op1);
			fprintf(f,"\n");
			break;
		case IC_GETVAL:
			fprint_opd(f,p.binop.target);
			fprintf(f,"\t:=\t");
			fprint_staropd(f,p.binop.op1);
			fprintf(f,"\n");
			break;
		case IC_ADDRVAL:
			fprint_staropd(f,p.binop.target);
			fprintf(f,"\t:=\t");
			fprint_opd(f,p.binop.op1);
			fprintf(f,"\n");
			break;
		case IC_GOTO:
			fprintf(f,"GOTO\t");
			fprint_opd(f,p.binop.target);
			fprintf(f,"\n");
			break;
		case IC_IF:
			fprintf(f,"IF\t");
			fprint_opd(f,p.jump.op1);
			fprintf(f,"\t");
			fprint_relop(f,p);
			fprintf(f,"\t");
			fprint_opd(f,p.jump.op2);
			fprintf(f,"\tGOTO\t");
			fprint_opd(f,p.jump.target);
			fprintf(f,"\n");
			break;
		case IC_RETURN:
			fprintf(f,"RETURN\t");
			fprint_opd(f,p.binop.target);
			fprintf(f,"\n");
			break;
		case IC_DEC:
			fprintf(f,"DEC\t");
			fprint_staropd(f,p.binop.target);
			fprintf(f,"\t");
			fprintf(f,"%d",p.binop.op1->int_value);
			fprintf(f,"\n");
			break;
		case IC_ARG:
			fprintf(f,"ARG\t");
			fprint_opd(f,p.binop.target);
			fprintf(f,"\n");
			break;
		case IC_ARGADDR:
			fprintf(f,"ARG\t&");
			fprint_opd(f,p.binop.target);
			fprintf(f,"\n");
			break;
		case IC_PARAM:
			fprintf(f,"PARAM\t");
			fprint_opd(f,p.binop.target);
			fprintf(f,"\n");
			break;
		case IC_READ:
			fprintf(f,"READ\t");
			fprint_opd(f,p.binop.target);
			fprintf(f,"\n");
			break;
		case IC_WRITE:
			fprintf(f,"WRITE\t");
			fprint_opd(f,p.binop.target);
			fprintf(f,"\n");
			break;
		case IC_CALL:
			fprint_opd(f,p.binop.target);
			fprintf(f,"\t:=\tCALL\t");
			fprint_opd(f,p.binop.op1);
			fprintf(f,"\n");
			break;
	}
}

void fprint_codes(InterCodes p,FILE* f)
{
	if (f==NULL)
	{
		printf("fopen error\n");
		return;
	}
	while (p!=NULL)
	{
		fprint_code(f,p->code);
		p=p->next;
	}
}

//---------------------------------------
void begin_translate(syntax_node *p)
{
	
	FILE* outfile=fopen("outfile.ir","w");
	opd_zero=new_int(0);
	opd_one=new_int(1);
	CodesPointer t=translate_tree(p);//printf("111111111111\n");
	fprint_codes(t->firstnode,outfile);//printf("3333333\n");
}
#endif
