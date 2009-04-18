
/* TestCase 1: Mixing globals and locals */

int globalint;
int g;

void main()
{
  int localint, a, b, c;
  int g;
	
  globalint = 100;
  localint = globalint * 20 - 5;
  g = localint / 5;
  write(g);
} 

/*
  Expected ouput: 399
*/

