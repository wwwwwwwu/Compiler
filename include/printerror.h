#ifndef __PRINTERROR_H__
#define __PRINTERROR_H__
#include <stdio.h>



void print_error(int i,int lineno,char *inf){
	printf("Error type %d at Line %d:",i,lineno);
	switch(i)
	{
		case 1:printf("Undefined varieble \"%s\"",inf);break;
		case 2:printf("Undefined function \"%s\"",inf);break;
		case 3:printf("Redefined varieble \"%s\"",inf);break;
		case 4:printf("Redefined function \"%s\"",inf);break;
		case 5:printf("Type mismatched for assignment");break;
		case 6:printf("The left-hand side of an assignment must be a variable");break;
		case 7:printf("Type mismatched for operands");break;
		case 8:printf("Type mismatched for return");break;
		case 9:printf("Function \"%s\" is not applicable for these arguments",inf);break;
		case 10:printf("\"%s\" is not an array",inf);break;
		case 11:printf("\"%s\" is not a function",inf);break;
		case 12:printf("Invalid types for array sbscript");break;
		case 13:printf("Illegal use of \".\"");break;
		case 14:printf("Non-existent field \"%s\"",inf);break;
		case 15:printf("Redefined field \"%s\"",inf);break;
		case 16:printf("Duplicated name \"%s\"",inf);break;
		case 17:printf("Undefined structure \"%s\"",inf);break;
//		case 71:printf("71\n");break;
//		case 72:printf("72\n");break;
//		case 73:printf("73\n");break;
//		case 74:printf("74\n");break;
	}
	printf("\n");
}

#endif
