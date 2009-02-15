CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
# Not sure if this will work without GNU Flex.
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

.PHONY: clean

clean:
	$(RM) *.o scan test_list
