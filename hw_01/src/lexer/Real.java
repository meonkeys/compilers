package lexer;

/**
 * Floating point numbers.
 */
public class Real extends Token {
	public final float value;

	public Real(float v) {
		super(Tag.REAL);
		value = v;
	}

	public String toString() {
		return "" + value;
	}
}
