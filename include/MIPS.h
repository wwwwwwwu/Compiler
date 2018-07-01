#ifndef __MIPS_H__
#define __MIPS_H__
#define MAXSIZE 0x7fff
#include <stdio.h>
#include <stdlib.h>
#include "head.h"
#include "IR.h"
extern FILE* outfile;
char firstseg[]=".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\nread:\nli $v0, 4\nla $a0, _prompt\nsyscall\nli $v0, 5\nsyscall\njr $ra\n\nwrite:\nli $v0, 1\nsyscall\nli $v0,4\nla $a0, _ret\nsyscall\nmove $v0, $0\njr $ra\n\n";
typedef struct var_inf_
{
	int off;
	int reg;
} var_inf;
var_inf *temp_i;
int *initoff;
#define NR_REG 8
int lensp;
int reg_cont[32];
int reg_saved[32];
void chgque(int i);
int quelast();
void init_que();
int getreg(int no);
int getregi(int i);
void ldreg(int no,int r);
void sp_add(int o);
void stback(int q);
void MIPS_start(InterCodes p);
void pushall();
void popall(int t);
void init_stinf();
void translate_IR(InterCode code);
void MIPS_assign(InterCode code);
void MIPS_label(InterCode code);
void MIPS_add(InterCode code);
void MIPS_sub(InterCode code);
void MIPS_mul(InterCode code);
void MIPS_div(InterCode code);
void MIPS_getval(InterCode code);
void MIPS_addrval(InterCode code);
void MIPS_if(InterCode code);
void MIPS_goto(InterCode code);
void MIPS_ret(InterCode code);
void MIPS_dec(InterCode code);
void MIPS_arg(InterCode code);
void MIPS_param(InterCode code);
void MIPS_call(InterCode code);
void MIPS_read(InterCode code);
void MIPS_write(InterCode code);
void MIPS_func(InterCode code);
int prtemp;
static int nr_vars=0;
static int nr_param=0;
static int alloff=0;
void begin_MIPS(){
	MIPS_start(MIP);
}
void MIPS_start(InterCodes p)
{
	InterCodes q=p;
	prtemp=total_temp;
	fprintf(outfile,"%s",firstseg);
	total_temp+=total_var_no;
	temp_i=malloc(sizeof(var_inf)*total_temp);
	initoff=malloc(sizeof(int)*total_temp);
	int i;
	for (i=0;i<total_temp;i++)
	{
		temp_i[i].off=temp_i[i].reg=-1;
		initoff[i]=0;
	}
	for (i=0;i<32;i++)
	{
		reg_cont[i]=-1;
		reg_saved[i]=1;
	}
	init_que();
	int temp=0;
	struct symboltype* nowfun=NULL;
	while (p!=NULL)
	{
		if (p->code.kind!=IC_IF)
		{
			if (p->code.binop.target->kind==OP_VARIABLE)
			{
				p->code.binop.target->var_no=prtemp+p->code.binop.target->sym->no;
				p->code.binop.target->kind=OP_TEMP;
			}
			if (p->code.binop.op1!=NULL&&p->code.binop.op1->kind==OP_VARIABLE)
			{
				p->code.binop.op1->var_no=prtemp+p->code.binop.op1->sym->no;
				p->code.binop.op1->kind=OP_TEMP;
			}
			if (p->code.binop.op2!=NULL&&p->code.binop.op2->kind==OP_VARIABLE)
			{
				p->code.binop.op2->var_no=prtemp+p->code.binop.op2->sym->no;
				p->code.binop.op2->kind=OP_TEMP;
			}

		}
		else
		{
			if (p->code.jump.target->kind==OP_VARIABLE)
			{
				p->code.jump.target->var_no=prtemp+p->code.jump.target->sym->no;
				p->code.jump.target->kind=OP_TEMP;
			}
			if (p->code.jump.op1!=NULL&&p->code.jump.op1->kind==OP_VARIABLE)
			{
				p->code.jump.op1->var_no=prtemp+p->code.jump.op1->sym->no;
				p->code.jump.op1->kind=OP_TEMP;
			}
			if (p->code.jump.op2!=NULL&&p->code.jump.op2->kind==OP_VARIABLE)
			{
				p->code.jump.op2->var_no=prtemp+p->code.jump.op2->sym->no;
				p->code.jump.op2->kind=OP_TEMP;
			}

		}
		if (p->code.kind==FUNCTION)
		{
			if (nowfun!=NULL)
			{
				int ii=0;
				temp=0;
				for (;ii<total_temp;ii++)
				{
					if (temp_i[ii].off==1)
					{
						if(initoff[ii]!=0)
						{
							int tt=initoff[ii];
							initoff[ii]=temp*4;
							temp+=tt/4;
						}
						else
						{
							initoff[ii]=temp*4;
							temp++;
						}
					}
					temp_i[ii].off=-1;
				}
				nowfun->no=temp;
			}
			nowfun=p->code.binop.target->sym;
		}
		if (p->code.kind==IC_DEC)
		{
			initoff[p->code.binop.target->sym->no+prtemp]=p->code.binop.op1->int_value;
		}
		if (p->code.kind!=IC_IF)
		{
			if (p->code.binop.target->kind==OP_TEMP)
			{
				temp_i[p->code.binop.target->var_no].off=1;
			}
			if (p->code.binop.op1!=NULL&&p->code.binop.op1->kind==OP_TEMP)
			{
				temp_i[p->code.binop.op1->var_no].off=1;
			}
			if (p->code.binop.op2!=NULL&&p->code.binop.op2->kind==OP_VARIABLE)
			{
				temp_i[p->code.binop.op2->var_no].off=1;
			}

		}
		else
		{
			if (p->code.jump.op1!=NULL&&p->code.jump.op1->kind==OP_TEMP)
			{
				temp_i[p->code.jump.op1->var_no].off=1;
			}
			if (p->code.jump.op2!=NULL&&p->code.jump.op2->kind==OP_TEMP)
			{
				temp_i[p->code.jump.op2->var_no].off=1;
			}

		}
		if (p->code.kind!=IC_IF)
		{
			if (p->code.binop.target->kind==OP_ADDRVAR)
			{
				temp_i[p->code.binop.target->sym->no+prtemp].off=1;
			}
			if (p->code.binop.op1!=NULL&&p->code.binop.op1->kind==OP_ADDRVAR)
			{
				temp_i[p->code.binop.op1->sym->no+prtemp].off=1;
			}
			if (p->code.binop.op2!=NULL&&p->code.binop.op2->kind==OP_VARIABLE)
			{
				temp_i[p->code.binop.op2->sym->no+prtemp].off=1;
			}

		}
		else
		{
			if (p->code.jump.op1!=NULL&&p->code.jump.op1->kind==OP_ADDRVAR)
			{
				temp_i[p->code.jump.op1->sym->no+prtemp].off=1;
			}
			if (p->code.jump.op2!=NULL&&p->code.jump.op2->kind==OP_ADDRVAR)
			{
				temp_i[p->code.jump.op2->sym->no+prtemp].off=1;
			}

		}

		p=p->next;
	}
	if (nowfun!=NULL)
	{
		int ii=0;
		temp=0;
		for (;ii<total_temp;ii++)
		{
			if (temp_i[ii].off==1)
			{
				if(initoff[ii]!=0)
				{
					int tt=initoff[ii];		
					initoff[ii]=temp*4;
					temp+=tt/4;
				}
				else
				{
					initoff[ii]=temp*4;
					temp++;
				}
			}
			temp_i[ii].off=-1;
		}
		nowfun->no=temp;
	}
	while (q!=NULL)
	{
		translate_IR(q->code);
		q=q->next;
	}
}
static int now_reg=0;
static int all_used=0;
int getreg(int no)
{
	if (temp_i[no].reg==-1)
	{
		int r=quelast()+8;
		if (reg_cont[r]!=-1)
			stback(reg_cont[r]);
		ldreg(no,r);
		reg_saved[r]=1;
	}
	else
		chgque(temp_i[no].reg-8);
	return temp_i[no].reg;
}
int getregi(int i)
{
	int r=quelast()+8;
	if (reg_cont[r]!=-1)
		stback(reg_cont[r]);
	reg_cont[r]=-2;
	reg_saved[r]=1;
	fprintf(outfile,"li $%d, %d\n",r,i);
	return r;
}
int getnreg()
{
	int r=quelast()+8;
	if (reg_cont[r]!=-1)
		stback(reg_cont[r]);
	return r;
}
void sp_add(int o)
{
	int i;
	alloff+=o;
	for (i=0;i<total_temp;i++)
		temp_i[i].off=(temp_i[i].off==-1)?-1:temp_i[i].off+o;
}

void ldreg(int no,int r)
{
	reg_cont[r]=no;
	temp_i[no].reg=r;
	if (temp_i[no].off==-1)
		temp_i[no].off=alloff+initoff[no];
	fprintf(outfile,"lw $%d, %d($sp)\n",r,temp_i[no].off);
	return;
}

void stback(int q)
{
	if (temp_i[q].off==-1)
	{
		temp_i[q].off=initoff[q]+alloff;
	}
	if (temp_i[q].reg!=-1&&temp_i[q].reg!=-2)
		fprintf(outfile,"sw $%d, %d($sp)\n",temp_i[q].reg,temp_i[q].off);
	reg_saved[temp_i[q].reg]=1;
}

static int prioque[NR_REG];
void init_que()
{
	int i;
	for (i=0;i<NR_REG;i++)
		prioque[i]=NR_REG-1-i;
}
void chgque(int i)
{
	int pre=i,nowi,k;
	for (k=0;k<NR_REG;k++)
	{
		if (prioque[k]!=i)
		{
			nowi=prioque[k];
			prioque[k]=pre;
			pre=nowi;
		}
		else
		{
			prioque[k]=pre;
			break;
		}
	}
}
int quelast()
{
	int t=prioque[NR_REG-1],i;
	for (i=NR_REG-1;i>0;i--)
		prioque[i]=prioque[i-1];
	prioque[0]=t;
	return t;
}
void translate_IR(InterCode code)
{
	switch(code.kind)
	{
		case IC_LABEL:MIPS_label(code);break;
		case IC_FUNCTION:MIPS_func(code);break;
		case IC_ASSIGN:MIPS_assign(code);break;
		case IC_ADD:MIPS_add(code);break;
		case IC_SUB:MIPS_sub(code);break;
		case IC_MUL:MIPS_mul(code);break;
		case IC_DIV:MIPS_div(code);break;
		case IC_GETADDR:printf("getaddr error\n");break;
		case IC_GETVAL:MIPS_getval(code);break;
		case IC_ADDRVAL:MIPS_addrval(code);break;
		case IC_GOTO:MIPS_goto(code);break;
		case IC_IF:MIPS_if(code);break;
		case IC_RETURN:MIPS_ret(code);break;
		case IC_DEC:MIPS_dec(code);break;
		case IC_ARG:MIPS_arg(code);break;
		case IC_ARGADDR:printf("argaddr error\n");break;
		case IC_PARAM:MIPS_param(code);break;
		case IC_READ:MIPS_read(code);break;
		case IC_WRITE:MIPS_write(code);break;
		case IC_CALL:MIPS_call(code);break;
	}
}

void pushall()
{
	int i;
	sp_add(lensp);
	fprintf(outfile,"add $sp, $sp, -%d\n",lensp);
	for (i=0;i<NR_REG;i++)
	{
		fprintf(outfile,"sw $%d, %d($sp)\n",i+8,i*4+4);
	}
	fprintf(outfile,"sw $ra, 0($sp)\n");
}

int nr_args=0;
void popall(int t)
{
	int i;
	fprintf(outfile,"lw $ra, %d($sp)\n",t);
	for (i=0;i<NR_REG;i++)
	{
		fprintf(outfile,"lw $%d, %d($sp)\n",i+8,t+i*4+4);
	}
	fprintf(outfile,"add $sp, $sp, %d\n",lensp+nr_args*4);
	sp_add(-lensp-nr_args*4);
	nr_args=0;
}
void MIPS_call(InterCode code)
{
	lensp=NR_REG*4+4;
	int t=code.binop.op1->sym->no*4;
	pushall();
	fprintf(outfile,"jal %s\n",code.binop.op1->sym->name);
	lensp+=t;
	popall(t);
	fprintf(outfile,"move $%d, $v0\n",getreg(code.binop.target->var_no));
}

void MIPS_read(InterCode code)
{
	fprintf(outfile,"add $sp, $sp, -4\nsw $ra, 0($sp)\njal read\nmove $%d, $v0\nlw $ra, 0($sp)\nadd $sp, $sp, 4\n",getreg(code.binop.target->var_no));
}

void MIPS_write(InterCode code)
{
	if (code.binop.target->kind==OP_TEMP)
		fprintf(outfile,"move $a0, $%d\nadd $sp, $sp, -4\nsw $ra, 0($sp)\njal write\nlw $ra, 0($sp)\nadd $sp, $sp, 4\n",getreg(code.binop.target->var_no));
	else if (code.binop.target->kind==OP_INT)
		fprintf(outfile,"li $a0, %d\nadd $sp, $sp, -4\nsw $ra, 0($sp)\njal write\nlw $ra, 0($sp)\nadd $sp, $sp, 4\n",code.binop.target->int_value);
	else
		printf("undo write\n");
}

void MIPS_param(InterCode code)
{
	temp_i[code.binop.target->var_no].off=lensp+nr_param*4;
	nr_param++;
}

void MIPS_arg(InterCode code)
{
	nr_args++;
	if (code.binop.target->kind==OP_INT)
	{
		int ri=getregi(code.binop.target->int_value);
		fprintf(outfile,"add $sp, $sp, -4\nsw $%d, 0($sp)\n",ri);
		sp_add(4);
	}
	else if (code.binop.target->kind==OP_TEMP)
	{
		int r=getreg(code.binop.target->var_no);
		fprintf(outfile,"add $sp, $sp, -4\nsw $%d, 0($sp)\n",r);
		sp_add(4);
	}
	else
	{
		printf("undo arg\n");
	}
}

void MIPS_dec(InterCode code)
{

}

void MIPS_ret(InterCode code)
{
	int i;
	for (i=0;i<32;i++)
	{
		if (reg_saved[i]==0)
			stback(reg_cont[i]);
	}
	if (code.binop.target->kind==OP_TEMP)
	fprintf(outfile,"move $v0, $%d\njr $ra\n",getreg(code.binop.target->var_no));
	else if (code.binop.target->kind==OP_INT)
		fprintf(outfile,"li $v0, %d\njr $ra\n",code.binop.target->int_value);
}

void MIPS_if(InterCode code)
{
	int i;
	for (i=0;i<32;i++)
	{
		if (reg_saved[i]==0)
			stback(reg_cont[i]);
	}
	int r1=0,r2=0,l=code.jump.target->label_no;
	if (code.jump.op1->kind==OP_INT)
		r1=getregi(code.jump.op1->int_value);
	else
		r1=getreg(code.jump.op1->var_no);
	if (code.jump.op2->kind==OP_INT)
		r2=getregi(code.jump.op2->int_value);
	else
		r2=getreg(code.jump.op2->var_no);
	switch(code.jump.relop)
	{
		case MORE:fprintf(outfile,"bgt $%d, $%d, l%d\n",r1,r2,l);break;
		case LESS:fprintf(outfile,"blt $%d, $%d, l%d\n",r1,r2,l);break;
		case NOGREATER:fprintf(outfile,"ble $%d, $%d, l%d\n",r1,r2,l);break;
		case NOLESS:fprintf(outfile,"bge $%d, $%d, l%d\n",r1,r2,l);break;
		case EQUAL:fprintf(outfile,"beq $%d, $%d, l%d\n",r1,r2,l);break;
		case NOTEQ:fprintf(outfile,"bne $%d, $%d, l%d\n",r1,r2,l);break;
	}
}

void MIPS_goto(InterCode code)
{
	int i;
	for (i=0;i<32;i++)
	{
		if (reg_saved[i]==0)
			stback(reg_cont[i]);
	}
	fprintf(outfile,"j l%d\n",code.binop.target->label_no);
}

void MIPS_addrval(InterCode code)
{
	if (code.binop.target->kind==OP_TEMP||code.binop.target->kind==OP_ADDRESS)
	{
		fprintf(outfile,"sw $%d, 0($%d)\n",getreg(code.binop.op1->var_no),getreg(code.binop.target->var_no));
	}	
	else if(code.binop.target->kind==OP_ADDRVAR)
	{
		fprintf(outfile,"move $%d, $%d\n",getreg(code.binop.target->sym->no+prtemp),getreg(code.binop.op1->var_no));	
		reg_saved[getreg(code.binop.target->sym->no+prtemp)]=0;
	}
}

void MIPS_getval(InterCode code)
{
	int r=getreg(code.binop.target->var_no);
	if (code.binop.op1->kind==OP_TEMP||code.binop.op1->kind==OP_ADDRESS)
	{
		fprintf(outfile,"lw $%d, 0($%d)\n",r,getreg(code.binop.op1->var_no));
	}
	else if(code.binop.op1->kind==OP_ADDRVAR)
	{
		fprintf(outfile,"move $%d, $%d\n",getreg(code.binop.target->var_no),getreg(code.binop.op1->sym->no+prtemp));
	}
}

void MIPS_add(InterCode code)
{
	int r=getreg(code.binop.target->var_no);
	reg_saved[r]=0;
	int r1;
	if(code.binop.op1->kind==OP_ADDRVAR)
	{
		int n=code.binop.op1->sym->no+prtemp;
		if (temp_i[n].off=-1) temp_i[n].off=initoff[n]+alloff;
		r1=getnreg();
		fprintf(outfile,"la $%d, %d($sp)\n",r1,temp_i[n].off);
		if (code.binop.op2->kind==OP_INT)
		{
			fprintf(outfile,"addi $%d, $%d, %d\n",r,r1,code.binop.op2->int_value);
		}
		else
		{
			fprintf(outfile,"add $%d, $%d, $%d\n",r,r1,getreg(code.binop.op2->var_no));
		}
	}
	else if(code.binop.op1->kind==OP_INT)
	{
		fprintf(outfile,"addi $%d, $%d, %d\n",getreg(code.binop.target->var_no),getreg(code.binop.op2->var_no),code.binop.op1->int_value);
	}
	else if(code.binop.op2->kind==OP_INT&&code.binop.op1->kind==OP_TEMP)
	{
		fprintf(outfile,"addi $%d, $%d, %d\n",getreg(code.binop.target->var_no),getreg(code.binop.op1->var_no),code.binop.op2->int_value);
	}
	else
	{
		fprintf(outfile,"add $%d, $%d, $%d\n",getreg(code.binop.target->var_no),getreg(code.binop.op1->var_no),getreg(code.binop.op2->var_no));
	}
}
void MIPS_sub(InterCode code)
{
	int r=getreg(code.binop.target->var_no);
	reg_saved[r]=0;
	if(code.binop.op1->kind==OP_INT)
	{
		int r1=getregi(code.binop.op1->int_value);
		fprintf(outfile,"sub $%d, $%d, $%d\n",getreg(code.binop.target->var_no),r1,getreg(code.binop.op2->var_no));
	}
	else if(code.binop.op2->kind==OP_INT)
	{
		fprintf(outfile,"addi $%d, $%d, %d\n",getreg(code.binop.target->var_no),getreg(code.binop.op1->var_no),-code.binop.op2->int_value);
	}
	else
	{
		fprintf(outfile,"sub $%d, $%d, $%d\n",getreg(code.binop.target->var_no),getreg(code.binop.op1->var_no),getreg(code.binop.op2->var_no));
	}
}
void MIPS_mul(InterCode code)
{
	int r=getreg(code.binop.target->var_no);
	reg_saved[r]=0;
	if(code.binop.op1->kind==OP_INT)
	{
		int r1=getregi(code.binop.op1->int_value);
		fprintf(outfile,"mul $%d, $%d, $%d\n",getreg(code.binop.target->var_no),r1,getreg(code.binop.op2->var_no));
	}
	else if(code.binop.op2->kind==OP_INT)
	{
		int r2=getregi(code.binop.op2->int_value);
		fprintf(outfile,"mul $%d, $%d, $%d\n",getreg(code.binop.target->var_no),getreg(code.binop.op1->var_no),r2);
	}
	else
	{
		fprintf(outfile,"mul $%d, $%d, $%d\n",getreg(code.binop.target->var_no),getreg(code.binop.op1->var_no),getreg(code.binop.op2->var_no));
	}
}
void MIPS_div(InterCode code)
{
	int r=getreg(code.binop.target->var_no);
	reg_saved[r]=0;
	if(code.binop.op1->kind==OP_INT)
	{
		int r1=getregi(code.binop.op1->int_value);
		fprintf(outfile,"div $%d, $%d\n",r1,getreg(code.binop.op2->var_no));
		fprintf(outfile,"mflo $%d\n",getreg(code.binop.target->var_no));
	}
	else if(code.binop.op2->kind==OP_INT)
	{
		int r2=getregi(code.binop.op2->int_value);
		fprintf(outfile,"div $%d, $%d\n",getreg(code.binop.op1->var_no),r2);
		fprintf(outfile,"mflo $%d\n",getreg(code.binop.target->var_no));
	}
	else
	{
		fprintf(outfile,"div $%d, $%d\n",getreg(code.binop.op1->var_no),getreg(code.binop.op2->var_no));
		fprintf(outfile,"mflo $%d\n",getreg(code.binop.target->var_no));
	}
}

void MIPS_label(InterCode code)
{
	init_stinf();
	fprintf(outfile,"l%d:\n",code.binop.target->label_no);
}

void init_stinf()
{
	int i;
	for (i=0;i<32;i++)
	{
		if (reg_saved[i]==0)
			stback(reg_cont[i]);
	}
	for (i=0;i<total_temp;i++)
	{
		temp_i[i].reg=-1;
	}
	for (i=0;i<32;i++)
		reg_cont[i]=-1;
	init_que();
}

void MIPS_func(InterCode code)
{
	init_stinf();
	fprintf(outfile,"%s:\n",code.binop.target->sym->name);
	fprintf(outfile,"add $sp, $sp, %d\n",-code.binop.target->sym->no*4);
	lensp=NR_REG*4+4+code.binop.target->sym->no*4;
	nr_vars=code.binop.target->sym->no;
	nr_param=0;
	alloff=0;
	return;
}

void MIPS_assign(InterCode code)
{
	int r=getreg(code.binop.target->var_no);
	reg_saved[r]=0;
	if (code.binop.target->kind==OP_TEMP)
	{
		if (code.binop.op1->kind==OP_INT)
		{
			fprintf(outfile,"li $%d, %d\n",r,code.binop.op1->int_value);
		}
		else if (code.binop.op1->kind==OP_TEMP)
		{
			fprintf(outfile,"move $%d, $%d\n",r,getreg(code.binop.op1->var_no));
		}
		else if (code.binop.op1->kind==OP_ADDRESS)
		{
			int n=code.binop.op1->sym->no+prtemp;
			if (temp_i[n].off==-1)
				temp_i[n].off=initoff[n]+alloff;
			fprintf(outfile,"la $%d, %d($sp)\n",r,temp_i[n].off);
		}
	}
	else
	{
		printf("= undo\n");
	}
}
#endif
