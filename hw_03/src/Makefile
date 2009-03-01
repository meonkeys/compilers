SHELL=/bin/bash
CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
VALGRIND_OPTS=--quiet --leak-check=full --show-reachable=yes --error-exitcode=2
LEX=flex
YACC=bison --yacc
TEST_DATA_DIR=test_data

test: main valgrind
	bash unit_test.sh $(TEST_DATA_DIR)

# check for memory leaks
valgrind: main
	$@ $(VALGRIND_OPTS) ./$< < $(TEST_DATA_DIR)/empty.in > /dev/null

main: main.c parser.o lexer3.o

.PHONY: distclean clean tools valgrind

clean:
	$(RM) main *.tab.{c,h} lexer3.{c,h} *.o

tools:
	cscope -bR
	ctags -R

# "clean enough for source distribution"
distclean: clean
	$(RM) tags cscope.out
