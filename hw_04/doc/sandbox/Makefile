SHELL=/bin/bash
CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
VALGRIND_OPTS=--quiet --leak-check=full --show-reachable=yes --error-exitcode=2
LEX=flex
YACC=bison
LDFLAGS=-lm

main: main.c parser.o scanner.o

clean:
	$(RM) y.tab.h *.o main scanner.h
