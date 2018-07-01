#ifndef __HEAD_H__
#define __HEAD_H__
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

#endif
