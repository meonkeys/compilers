CC=gcc
#CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
CFLAGS=-g -Wall -Werror -I.
VALGRIND_OPTS=--quiet --leak-check=full --show-reachable=yes --error-exitcode=2
LDFLAGS=-lm
# Add -d to the next line to generate a debugging scanner (or uncomment %option
# debug in the .l file).
LEX=flex

test: scan test_list test_hash valgrind
	bash unit_test.sh
	./test_list
	#./test_hash

test_list: list.o

#test_hash: hash.o

valgrind: scan test_list #test_hash
	$@ $(VALGRIND_OPTS) ./$< < tests/simple.in > /dev/null
	$@ $(VALGRIND_OPTS) ./test_list
	#$@ $(VALGRIND_OPTS) ./test_hash

# uses implicit rules to compile the scanner
scan: list.o hashtable.o hashtable_itr.o hashtable_utility.o

.PHONY: distclean clean

clean:
	$(RM) *.o scan test_list *~ #test_hash *~

# "clean enough for source distribution"
distclean: clean
	$(RM) tags cscope.out
