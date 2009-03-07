/* Test2 */
/* Can you handle Struct and Union */
/* At syntex check level, struct and union can be handled the same 
way by returning the token "STRUCT" for both struct and union */

/* structures and unions */

struct
{
	int length;
	union
	{
		int intvalue;
		float floatvalue;
	} value;
} node;

struct nlist {
	int i;
	float b;
	int c[10];
	};

struct nlist n1, n2;

union {
	struct {
		int type;
		} n;
	struct {
	        int type;
	        int intnode;
               } n1;
	struct {
	        int type;
	        float fnode;
               } nf;
	       } united;

int Matrix_Multiplication_Function (int V1[][10], int V2[][10])
{
 int i,j,k;
 int result[10][10];          /*multi-dimensional array declaration */

   for (i = 0; i < 10; i = i + 1) 
   {
      for(j = 0; j < 10; j = j + 1) 
      {
            result[i][j] = 0;
            for (k = 0; k < 10; k = k + 1) 
            {
				result[i][j] = result[i][j] + V1[i][k]*V2[k][j];
            }
      }
   }

  return 1;
}

