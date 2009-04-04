/* a test program with no real logic and meaning */

typedef int myint;
int arr3[3][2];

/* structures */
typedef struct
{
	int data;
} node;

/* structures */
typedef struct 
{
	int data;
	node mynode;
} link;

/* structures */
typedef struct
{
	int data;
  	member mymember;
} member;

/* unions */
typedef union
{
	int intvalue;
	float floatvalue;
	node structvalue;
} anyvalue;

/* Check typedefs */

myint myfunc1(myint a)
{
  return a+1;
}

int myfunc2(int a, float b)
{
  node result;

  result.data = a * b;

  return result.data;
}

float myfunc3(int a[][2], float b)
{
  float ret;

 ret=a[0][0]+b;

  return ret;
}

/* incompatible return type */

int myfunc4(int a)
{
  node result;
 
  result.data = a + 1;
  return result;
}

int myfunc5(int a)
{
  return a+1;
}

int main()
{

  int arr[4];
  int arr2[3][2];
  int div, sum, loopvar;
  float sum;	/* double defined variable names */
  float result;
  myint n;

  node mynode1, mynode2;
  link mylink;
  anyvalue myunion;
  
  /* Valid assignment */

  mylink.mynode  = mynode1;
  

  /* incorrect comparison */
  for (loopvar = 1;loopvar <= mylink;loopvar = loopvar + 1)
  {
    sum = sum + loopvar;
  }

  /* checking correct assignment */
  arr[myfunc5(5)] = arr2[myfunc5(arr[sum])][5];

  /* parameter mismatch */
  sum = myfunc3(mynode1, arr3[1][1]);

  /* Invalid array */ 
  arr[2]= result - mylink[0];

  /* function call with unmatched number of parameters */ 
  arr[0] = myfunc1(n, 1);
  arr[1] = myfunc2(n);
  
  return 0;
}
