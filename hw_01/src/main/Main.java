package main;

import inter.Node;
import inter.Stmt;
import inter.Temp;

import java.io.*;
import lexer.*;
import parser.*;

public class Main {
	public static void main(String[] args) throws IOException {
		InputStream old_stdin = System.in;
		/* Assume first arg is file to read from, instead of standard input. */
		if (args.length > 0) {
			System.setIn(new FileInputStream(args[0]));
		}
		compile();
		System.setIn(old_stdin);
	}

	public static void compile() throws IOException {
		Lexer lex = new Lexer();
		Parser parse = new Parser(lex);
		parse.program();
		System.out.write('\n');
		reset();
	}

	/** Clean up static state for the benefit of unit tests. */
	private static void reset() {
		Lexer.reset();
		Node.reset();
		Temp.reset();
		Stmt.reset();
	}
}
