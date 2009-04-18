/* TestCase 4: simple if-else statements */

void main()
{
 int a,b,c;

 b = read();
 c = read();

 if (b > c) 
   {
     b = b+c;
   } 
 else 
   {
     b = b-c;
   }
 write("The answer is ");
 write(b);
}

/*
  Expected output (for input b = 3, c = 2): The answer is 5
*/
