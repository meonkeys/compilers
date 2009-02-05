package main;

import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;

public class Test {
	public static void main(String[] args) throws IOException {
		InputStream old_stdin = System.in;
		PrintStream old_stdout = System.out;
		boolean somethingBroke = false;

		for (String s : findTests(args[0])) {
			// setUp
			boolean isError = false;
			System.setIn(new FileInputStream(args[0] + File.separator + s));
			StringSavePrintStream temp = new StringSavePrintStream();
			PrintStream pstream = new PrintStream(temp);
			System.setOut(pstream);

			// run test
			System.err.print("test: " + s + " ...");
			try {
				Main.compile();
			} catch (Error e) {
				somethingBroke = true;
				isError = true;
				System.err.println("ERROR");
				e.printStackTrace(System.err);
			}

			// tearDown
			temp.flush();
			String actual = temp.read();
			System.setIn(old_stdin);
			System.setOut(old_stdout);

			if (isError)
				continue;

			// report
			String expected = slurp(args[0] + File.separator + s + ".i");
			if (!expected.equals(actual)) {
				System.out.println("FAILURE");
				System.out.println("expected:\n" + expected);
				System.out.println("actual:\n" + actual);
				somethingBroke = true;
				continue;
			}

			System.err.println(" OK");
		}

		if (somethingBroke)
			System.exit(1);
		else
			System.exit(0);
	}

	private static String[] findTests(String searchDir) {
		String[] rv;
		ArrayList tests = new ArrayList();
		for (File f : new File(searchDir).listFiles()) {
			if (!f.isFile())
				continue;
			if (f.getName().matches("^(\\.|_)"))
				continue;
			if (f.getName().matches("^.+\\.i$"))
				continue;
			tests.add(f.getName());
		}
		rv = (String[]) tests.toArray(new String[0]);
		Arrays.sort(rv);
		return rv;
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
