package inter;

import lexer.*;
import symbols.*;

public class Arith extends Op {
	public Expr expr1, expr2;

	public Arith(Token tok, Expr x1, Expr x2) {
		super(tok, null);
		expr1 = x1;
		expr2 = x2;
		type = Type.max(expr1.type, expr2.type);
		if (type == null)
			error("type error");
	}

	public Expr gen() {
		boolean isOp1int = true;
		boolean isOp2int = true;
		boolean isOp1float = true;
		boolean isOp2float = true;

		try{
			Integer.parseInt(expr1.toString());
		}catch(NumberFormatException nfe){
			isOp1int = false;
		}

		try{
			Integer.parseInt(expr2.toString());
		}catch(NumberFormatException nfe){
			isOp2int = false;
		}

		try{
			Double.parseDouble(expr1.toString());
		}catch(NumberFormatException nfe){
			isOp1float = false;
		}

		try{
			Double.parseDouble(expr2.toString());
		}catch(NumberFormatException nfe){
			isOp2float = false;
		}

		if(isOp1int && isOp2int){
			int result = 0;
			int op1 = Integer.parseInt(expr1.toString());
			int op2 = Integer.parseInt(expr2.toString());
			if(op.toString().equals("+")){
				result = op1 + op2;
			}
			else if(op.toString().equals("-")){
				result = op1 - op2;
			}
			else if(op.toString().equals("*")){
				result = op1 * op2;
			}
			else if(op.toString().equals("/")){
				result = op1 / op2;
			}
			return new Constant(new Num(result), Type.Int);
		}
		else if(isOp1float && isOp2float){
			float result = (float)0.0;
			float op1 = Float.parseFloat(expr1.toString());
			float op2 = Float.parseFloat(expr2.toString());
			if(op.toString().equals("+")){
				result = op1 + op2;
			}
			else if(op.toString().equals("-")){
				result = op1 - op2;
			}
			else if(op.toString().equals("*")){
				result = op1 * op2;
			}
			else if(op.toString().equals("/")){
				result = op1 / op2;
			}
			return new Constant(new Real(result), Type.Float);
		}
		return new Arith(op, expr1.reduce(), expr2.reduce());
	}

	public String toString() {
		return expr1.toString() + " " + op.toString() + " " + expr2.toString();
	}
}
