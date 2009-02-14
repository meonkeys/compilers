CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
# Not sure if this will work without GNU Flex.
# Add -d to the next line to generate a debugging scanner (or uncomment %option
# debug in the .l file).
LEX=flex

test: scan ctest valgrind
	bash unit_test.sh
	./ctest

ctest: list.o

valgrind: scan ctest
	$@ --quiet ./$< < tests/simple.in > /dev/null
	$@ --quiet ./ctest

# uses implicit rules to compile the scanner
scan:

.PHONY: clean

clean:
	$(RM) *.o scan ctest
