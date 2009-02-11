CC=gcc
CFLAGS=-g -Wall -Werror -pedantic-errors -ansi
# Not sure if this will work without GNU Flex.
# Add -d to the next line to generate a debugging scanner (or uncomment %option
# debug in the .l file).
LEX=flex
LFLAGS=-t --yylineno

test: scan
	bash unit_test.sh

# uses implicit rules to compile the scanner
scan:

.PHONY: clean

clean:
	$(RM) *.o scan
