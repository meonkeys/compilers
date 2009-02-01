package main;

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
		Lexer lex = new Lexer();
		Parser parse = new Parser(lex);
		parse.program();
		System.out.write('\n');
		System.setIn(old_stdin);
	}
}