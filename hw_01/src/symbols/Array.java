package symbols;

import lexer.*;

public class Array extends Type {
	public Type of; // array *of* type
	public int size = 1; // number of elements

	public Array(int sz, Type type) {
		super("[]", Tag.INDEX, sz * type.width);
		size = sz;
		of = type;
	}

	public String toString() {
		return "[" + size + "]" + of.toString();
	}
}
