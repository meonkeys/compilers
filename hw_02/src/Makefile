CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
VALGRIND_OPTS=--quiet --leak-check=full --show-reachable=yes --error-exitcode=2
LDFLAGS=-lm
# Add -d to the next line to generate a debugging scanner
# (or uncomment %option debug in the .l file).
LEX=flex
TEST_DATA_DIR=test_data

test: scan test_list
	bash unit_test.sh $(TEST_DATA_DIR)
	./test_list

test_list: list.o

# check for memory leaks in scanner and unit tests
valgrind: scan test_list
	$@ $(VALGRIND_OPTS) ./$< < $(TEST_DATA_DIR)/simple.in > /dev/null
	$@ $(VALGRIND_OPTS) ./test_list

# uses implicit rules to compile the scanner
scan: list.o util.o hashtable.o hashtable_itr.o hashtable_utility.o

.PHONY: distclean clean tools valgrind

clean:
	$(RM) *.o scan test_list *~

tools:
	cscope -bR
	ctags -R

# "clean enough for source distribution"
distclean: clean
	$(RM) tags cscope.out
