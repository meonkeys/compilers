package main;

import java.io.IOException;
import java.io.OutputStream;

public class StringSavePrintStream extends OutputStream {
	StringBuilder t = new StringBuilder();
	
	public StringSavePrintStream() {
	}

	public void write(int b) throws IOException {
		byte[] buf = new byte[1];
        buf[0] = (byte)b;
        String S = new String(buf);
        t.append(S);
	}

	public String read() {
		return t.toString();
	}
}
