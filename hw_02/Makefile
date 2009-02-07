CC=gcc
CFLAGS=-g -Werror -pedantic-errors -ansi
# add -d to the next line to generate a debugging scanner
LEX=flex

scan: scan.o

scan.o: scan.l

clean:
	$(RM) *.o scan
