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
extern FILE* outfile;
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
		symbol_entry* sym;
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

Operand new_var(symboltype* s)
{
	Operand p=malloc(sizeof(struct Operand_));
	if (s->kind==VARIABLE)
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
Operand new_arr(Type s)
{
	Operand p=malloc(sizeof(struct Operand_));
	p->kind=OP_VARIABLE;
	p->arr_type=s;
	return p;
}
void change2arr(Operand p,Type s)
{
	p->kind=OP_VARIABLE;
	p->arr_type=s;
}
Operand new_addrtemp(int s)
{
	Operand p=new_temp();
	p->kind=OP_ADDRESS;
	return p;
}
void change2addrtemp(Operand p)
{
	p->kind=OP_ADDRESS;
}

void change2var(Operand p,symboltype *s)
{
	if (s->kind==VARIABLE)
	{
		if (s->type->kind==BASIC)
			p->kind=OP_VARIABLE;
		else if (s->type->kind==FUN)
			p->kind=OP_FUNC;
		else
			p->kind=OP_ADDRVAR;
	}
	else if (s->kind==FUNCTION)
		p->kind=OP_FUNC;
	else if (s->kind==PARADDR)
		p->kind=OP_VARIABLE;
	p->sym=s;
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


CodesPointer translate_exp(syntax_node *p,Operand t);


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
			symboltype* s=find_symbol(q->inf);
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
				NEW_CODE(ret,IC_SUB,t,get_zero(),t1);
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

#endif
