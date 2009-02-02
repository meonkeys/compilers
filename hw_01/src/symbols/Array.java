package symbols;

import lexer.*;

public class Array extends Type {
	public Type of;
	public int size = 1;

	public Array(int sz, Type type) {
		super("[]", Tag.INDEX, sz * type.width);
		size = sz;
		of = type;
	}

	public String toString() {
		return "[" + size + "]" + of.toString();
	}
}
