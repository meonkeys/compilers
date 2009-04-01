SHELL=/bin/bash
CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
VALGRIND_OPTS=--quiet --leak-check=full --show-reachable=yes --error-exitcode=2
LEX=flex
YACC=bison
TEST_DATA_DIR=test_data

test: main valgrind
	@bash unit_test.sh $(TEST_DATA_DIR)

# build a parser that prints trace output to stderr
trace: main.c parser_debug.o lexer3.o symtab.o ourtypes.o util.o
	$(CC) $(CFLAGS) -DYYDEBUG=1 -o $@ $^

parser_debug.o: y.tab.c
	$(CC) $(CFLAGS) -DYYDEBUG=1 -c -o $@ $<

y.tab.c: parser.y
	$(YACC) $(YFLAGS) --verbose $<

# check for memory leaks
valgrind: main
	$@ $(VALGRIND_OPTS) ./$< < $(TEST_DATA_DIR)/simple.in > /dev/null

pretty:
	indent -bli0 -nut -i4 *.c

main: main.c parser.o lexer3.o ourtypes.o symtab.o util.o

.PHONY: distclean clean tools valgrind

clean:
	$(RM) main trace *.tab.{c,h} lexer3.{c,h} *.o y.output

tools:
	cscope -bR
	ctags -R

# "clean enough for source distribution"
distclean: clean
	$(RM) tags cscope.out
