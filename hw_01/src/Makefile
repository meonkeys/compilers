OBJECT = main/Main.class inter/*.class lexer/*.class parser/*.class symbols/*.class
JavaCompiler = /usr/bin/javac
JavaVirtualMachine = /usr/bin/java

all: main test

main: $(OBJECT)

main/Main.class : main/Main.java 
	$(JavaCompiler) main/*.java

inter/*.class : inter/*.java 
	$(JavaCompiler) inter/*.java

lexer/*.class : lexer/*.java 
	$(JavaCompiler) lexer/*.java

parser/*.class : parser/*.java 
	$(JavaCompiler) parser/*.java

symbols/*.class : symbols/*.java 
	$(JavaCompiler) symbols/*.java

.PHONY: test clean

test: main
	$(JavaVirtualMachine) main.Test ../tests

clean:
	rm -f */*.class */*~ *~

