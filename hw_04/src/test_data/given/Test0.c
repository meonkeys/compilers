int myfunc1(int a)
/* some variable undefined */
{
  int b,c;
  b = a + 1;
  c = a - 1;
  return d+1;
}

float myfunc2(int a, float b)
/* some variable redefined */
{
  float a, result;

  result = a * b;

  return result;
}

float myfunc3(int a, float b)
/* some variable redefined */
{
  float result;

  {
  int a;
  /* EDITED ++ operator is not in the C-- manual. */
  for (a=1; a < 10; a=a+1) {
  result = result + a * b;
  }
  }

  return result;
}

