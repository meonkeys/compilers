CC=gcc
CFLAGS=-g -Wall -pedantic-errors -ansi
# add -d to the next line to generate a debugging scanner
LEX=flex

run: word
	./$< < input.txt

# uses implicit rules to compile the scanner
word:

.PHONY: clean

clean:
	$(RM) *.o word
