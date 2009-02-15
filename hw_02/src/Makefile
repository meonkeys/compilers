CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
# Add -d to the next line to generate a debugging scanner (or uncomment %option
# debug in the .l file).
LEX=flex

test: scan test_list test_hash valgrind
	bash unit_test.sh
	./test_list
	./test_hash

test_list: list.o

test_hash: hash.o

valgrind: scan test_list test_hash
	$@ --quiet ./$< < tests/simple.in > /dev/null
	$@ --quiet ./test_list
	$@ --quiet ./test_hash

# uses implicit rules to compile the scanner
scan: list.o hash.o

.PHONY: distclean clean

clean:
	$(RM) *.o scan test_list test_hash *~

# "clean enough for source distribution"
distclean: clean
	$(RM) tags cscope.out
