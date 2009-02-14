CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi -I.
# Not sure if this will work without GNU Flex.
# Add -d to the next line to generate a debugging scanner (or uncomment %option
# debug in the .l file).
LEX=flex

# XXX - remove
temp: ctest run_ctest

# XXX - remove
run_ctest: ctest
	./$<

test: scan ctest valgrind
	bash unit_test.sh

ctest: list.o

valgrind: scan
	$@ --quiet ./$< < tests/simple.in > /dev/null

# uses implicit rules to compile the scanner
scan:

.PHONY: clean

clean:
	$(RM) *.o scan ctest
