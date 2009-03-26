SHELL=/bin/bash
CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
VALGRIND_OPTS=--quiet --leak-check=full --show-reachable=yes --error-exitcode=2
LEX=flex
YACC=bison
LDFLAGS=-lm

main: main.c calc.o scan.o

clean:
	$(RM) calc *.o *.c
