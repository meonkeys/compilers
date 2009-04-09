SHELL=/bin/bash
CC=gcc
CFLAGS=-g -Wall -pedantic -ansi -I.
VALGRIND_OPTS=--quiet --leak-check=full --show-reachable=yes --error-exitcode=2
LEX=flex
YACC=bison
TEST_DATA_DIR=test_data

test: main unit valgrind

unit: main
	@bash unit_test.sh $(TEST_DATA_DIR)

# build a parser that prints trace output to stderr
trace: main.c parser4_debug.o lexer4.o functions.o symboltable.o
	$(CC) $(CFLAGS) -DYYDEBUG=1 -o $@ $^

parser4_debug.o: y.tab.c
	$(CC) $(CFLAGS) -DYYDEBUG=1 -c -o $@ $<

y.tab.c: parser4.y
	$(YACC) $(YFLAGS) --verbose $<

# check for memory leaks
valgrind: main
	$@ $(VALGRIND_OPTS) ./$< < $(TEST_DATA_DIR)/simple.in > /dev/null

pretty:
	indent -bli0 -nut -i4 *.c *.h

main: main.c parser4.o lexer4.o functions.o symboltable.o

.PHONY: distclean clean tools unit valgrind

clean:
	$(RM) main trace *.tab.{c,h} lexer4.{c,h} *.o *~ y.output lex.yy.c y.tab.c

tools:
	cscope -bR
	ctags -R

# "clean enough for source distribution"
distclean: clean
	$(RM) tags cscope.out
