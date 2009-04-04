/* a test program with no real logic and meaning */

/* structures */
typedef struct
{
	int data;
} node;

/* structures */
typedef struct
{
	int data;
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
	struct node structvalue;
} anyvalue;

/* TYPEDEF declared more than once */
typedef float mytype;
typedef int mytype;

int myfunc1(int a)
{
  return a+1;
}

/* incompatible return type */
float myfunc2(int a, float b)
{
  node result;

  result.data = a * b;

  return result;
}

float myfunc3(int a[][2], float b)
{
  float ret;

  ret=a[0][0]+b;

  return ret;
}

int main()
{

  int arr[4];
  int arr2[3][2];
  int div, sum;
  float sum;	/* double defined variable names */
  float result;
  int	n;

  node mynode1, mynode2;
  link mylink;
  anyvalue myunion;
  
  mynode.data = myunion.structvalue.data + mynode.data + myunion.floatvalue;

  /* Valid assignment */
  mynode1 = mynode2;

  /* Invalid assignment */

  mylink = mynode2;
  
  /* Invalid assignment */
  mynode.data = myunion.structvalue;

  /* Invalid data member */
  mynode.data = myunion.novalue;
  
  result = 4;
  while(n < result)
  {
    arr[n] = n;
    n = n + 1;
  }

  /* variable not declared before usage */
  for (loopvar = 1;loopvar <= n;loopvar = loopvar + 1)
  {
    sum = sum + loopvar;
  }

  write("enter a divider");
  div=read();

  /* Logical operations (!, ||, &&) must have integer operands and integer results */
  if ((div!=0) && (result))
  {
    result = sum/div;
    if ((result>=10) && !div)
    {
      /* function call with unmatched number of parameters */ 
      arr[0] = myfunc1(sum, 1);
      arr[1] = myfunc2(sum);
    }
    else
    {
      /* Array subscript must be integer type */
      arr[1.2] = myfunc2(sum, 1.5);    
    }
  }
  else
  {
    return 0; 
  }

  arr[2]=arr[0]*arr[1]+result+sum;
  
  /* Scalar can't be passed to array parameter <name> */
  result=myfunc3(arr2[3][2],result);

  /* Incompatible array dimensions. */
  arr[0][0]=sum;
  arr2[1]=result;

  /* undefined function before usage */
  myfunc4(2.5);

  return 0;
}
 

