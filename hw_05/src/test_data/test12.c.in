/* TestCase 12: Multiple scopes */

int a = 12 ;

void main()
{

  int a,b,c,d;
  float e,f;

  b = 5;
  a = b;
  c = a * b;
  d = c + a;
  
  e = 5.2;
  
  write(c);

  {
    int c;
    int d;

    c = b;
    d = c + 7;
    f = e * 2.6;

    write(c);
    write("\n");

    {
      
      d = d + c + b;
      write(d);
      write("\n");
      write(f);
      
    }
  }
}

/* Expected output : 25
                     5
		     22
		     13.52
*/
