SHELL=/bin/bash
CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
VALGRIND_OPTS=--quiet --leak-check=full --show-reachable=yes --error-exitcode=2
LEX=flex
YACC=bison
LDFLAGS=-lm
TEST_DATA_DIR=test_data

test: main valgrind
	@bash unit_test.sh $(TEST_DATA_DIR)

main: main.c parser.o scanner.o

trace: main.c parser_debug.o scanner.o
	$(CC) $(CFLAGS) $(LDFLAGS) -DYYDEBUG=1 -o $@ $^

clean:
	$(RM) *.o y.tab.h scanner.h y.output y.tab.c lex.yy.c main trace

parser_debug.o: y.tab.c
	$(CC) $(CFLAGS) -DYYDEBUG=1 -c -o $@ $<

y.tab.c: parser.y
	$(YACC) $(YFLAGS) --verbose $<

# check for memory leaks
valgrind: main
	@# $@ $(VALGRIND_OPTS) ./$< < $(TEST_DATA_DIR)/01simple.in > /dev/null
	$@ $(VALGRIND_OPTS) ./$< < $(TEST_DATA_DIR)/02variables.in > /dev/null
	@# $@ $(VALGRIND_OPTS) ./$< < $(TEST_DATA_DIR)/03keyword_lvalue.in > /dev/null
