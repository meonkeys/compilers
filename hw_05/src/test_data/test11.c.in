/* TestCase 11: Multiple procedure calls */ 

int n;
int currentnum;

int fact()
{
  int localnum;

  localnum = currentnum;
 
  if (localnum == 1)
    {
      return localnum;
    }
  else
    {
      currentnum = currentnum - 1;
      return ( localnum * fact( ) );
    }
}

void allfactorial()
{
  int i, temp;	

  i = 1;
  
  while( i <= n )
    {
      currentnum = i;
  
      write("Factorial of ");
      write(currentnum);
      write("is");
      temp = fact();
      write(temp);
      write("\n");

      i = i + 1;
    }
}

void main()
{
  
  write("Enter a number of factorials needed:");
  n = read();

  allfactorial();

}

/*
Expected output for 6

Factorial of 1 is 1
Factorial of 2 is 2
Factorial of 3 is 6
Factorial of 4 is 24
Factorial of 5 is 120
Factorial of 6 is 720

*/

