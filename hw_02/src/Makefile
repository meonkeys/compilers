CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
# Add -d to the next line to generate a debugging scanner (or uncomment %option
# debug in the .l file).
LEX=flex

test: scan test_list valgrind
	bash unit_test.sh
	./test_list

test_list: list.o

valgrind: scan test_list
	$@ --quiet ./$< < tests/simple.in > /dev/null
	$@ --quiet ./test_list

# uses implicit rules to compile the scanner
scan: list.o

.PHONY: distclean clean

clean:
	$(RM) *.o scan test_list *~

# "clean enough for source distribution"
distclean: clean
	$(RM) tags cscope.out
