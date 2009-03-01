CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
LDFLAGS=-lfl
LEX=flex
#LFLAGS=--outfile=lex.yy.c
YACC=bison
YFLAGS=--file-prefix=y

parser: parser.o lexer3.o

lexer3.o: lexer3.l
	$(LEX) $(LFLAGS) $< > $@

.PHONY: clean

clean:
	$(RM) parser
