#include <stdio.h>
FILE *outfile;
int main(int argc,char **argv){
	if (argc <= 1) {
		return 1;
	}
	FILE *f =fopen(argv[1],"r");
	if (!f)
	{
		perror(argv[1]);
		return 1;
	}
	outfile=fopen(argv[2],"w");
	yyrestart(f);
	yyparse();
	return 0;
}

