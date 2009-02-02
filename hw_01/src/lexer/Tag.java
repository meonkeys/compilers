package lexer;

/**
 * Are the values significant?
 * 
 * FIXME: this should really be an enum.
 */
public class Tag {
	public static final int AND = 256;
	/** Indicates a "basic" or "primitive" type. */
	public static final int BASIC = 257;
	public static final int BREAK = 258;
	public static final int DO = 259;
	public static final int ELSE = 260;
	public static final int EQ = 261;
	public static final int FALSE = 262;
	/** Greater than or equal. */
	public static final int GE = 263;
	public static final int ID = 264;
	public static final int IF = 265;
	/** Not a lexical token -- used in syntax trees. */
	public static final int INDEX = 266;
	/** Less than or equal. */
	public static final int LE = 267;
	/**
	 * Not a lexical token -- used in syntax trees. Unary minus, like for
	 * numeric negation(?).
	 */
	public static final int MINUS = 268;
	/** Not equals. */
	public static final int NE = 269;
	public static final int NUM = 270;
	public static final int OR = 271;
	public static final int REAL = 272;
	/** Not a lexical token -- used in syntax trees. */
	public static final int TEMP = 273;
	public static final int TRUE = 274;
	public static final int WHILE = 275;
}
