package main;

import java.io.*;

public class Test {
	static String[] tests = { "array", "fromBook", "precedence", "simple" };

	public static void main(String[] args) throws IOException {
		InputStream old_stdin = System.in;
		PrintStream old_stdout = System.out;
		boolean isError = false;

		for (String s : tests) {
			// setUp
			System.setIn(new FileInputStream(args[0] + File.separator + s));
			StringSavePrintStream temp = new StringSavePrintStream();
			PrintStream pstream = new PrintStream(temp);
			System.setOut(pstream);

			// run test
			Main.compile();

			// tearDown
			String actual = temp.read();
			System.setIn(old_stdin);
			System.setOut(old_stdout);

			// report
			String expected = slurp(args[0] + File.separator + s + ".i");
			if (!expected.equals(actual)) {
				System.out.println("ERROR in test: " + s);
				System.out.println("EXPECTED:\n" + expected);
				System.out.println("BUT WAS:\n" + actual);
				isError = true;
			}
		}

		if (isError)
			System.exit(1);
		else
			System.exit(0);
	}

	private static String slurp(String filePath) throws IOException {
		StringBuffer fileData = new StringBuffer(1000);
		BufferedReader reader = new BufferedReader(new FileReader(filePath));
		char[] buf = new char[1024];
		int numRead = 0;
		while ((numRead = reader.read(buf)) != -1) {
			fileData.append(buf, 0, numRead);
		}
		reader.close();
		return fileData.toString();
	}

}
