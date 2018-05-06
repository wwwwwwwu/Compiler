CC = gcc
SC = syntax.tab.c
SH = syntax.tab.h
name = 151220128.tar.gz
all:bison flex parser 
.PHONY: clean1 parser bison flex

bison: syntax.y
	@bison -d syntax.y

flex: lexical.l
	@flex lexical.l
                                            
clean:
	@rm -f lex.yy.c parser $(SC) $(SH)

tar:
	@tar -zcvf ../$(name) .

parser:
	@bison -d syntax.y
	@flex lexical.l
	@$(CC) main.c $(SC) -lfl -ly -o parser

