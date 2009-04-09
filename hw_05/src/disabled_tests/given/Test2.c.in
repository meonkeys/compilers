int sunfunction (int a[], int b[][10], float c)
{
	int i,j,k;
	i = c;
	for (j=0; j<10; j = j+1)
	{
		k = k + a[j];
		k = kk + b[j][1];
	}
	return k;
}

int main () 
{
  int	xx,x,yy,y,j=1;
  int	xstep, ystep; 
  float	xmove, ymove;
  float   z[10];
  int     z1[10], z2[10][10];
  float wrong[10];

  while ((xx != x) || (yy != y))
  {
    xmove = xstep;
    if ( ymove ) 
        xmove = 0;
    
    if (xx < x) 
    {
	    int z[10];
	    float j=0.0;
      xstep = xstep - 1;
      xmove = - xstep;
      z[x] = xstep;
      z[10][5] = xmove;
      z[j] = 0;
    }
    
    xx = read();
    ymove = ystep;
    z[j] = 1;

    if (yy == y)
        ymove = 0;
    
    if (yy < y) 
    {
      ystep = ystep - 1;
      ymove = - ystep;
    }
    
    x = x + xmove;
    y = y + ymove;
    yy = sunfunction(z1, z2, ymove);
    yy = sunfunction(z1, z2[1], z1[1]);
    yy = sunfunction(z[1], z2, ymove);
    z[1] = xmove;
    z[2] = xx;
    z[8] = xstep;
    xstep = z[5];
  }
}

 
