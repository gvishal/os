#include <stdio.h>
#include <stdlib.h>
#define V 100

int main()
{
   int **m1, **m2, **mult;
   m1 = (int **)(malloc(V*sizeof(int *)));
   m2 = (int **)(malloc(V*sizeof(int *)));
   mult = (int **)(malloc(V*sizeof(int *)));
   
    int  i, j, k;
    for(i=0;i<V;i++)
    {
      m1[i] = (int *)(malloc(V*sizeof(int)));
      m2[i] = (int *)(malloc(V*sizeof(int)));
      mult[i] = (int *)(malloc(V*sizeof(int)));
    }
        for(i=0;i<V;i++)
            for(j=0;j<V;j++)
                m1[i][j] = 2;
        for(i=0;i<V;i++)
            for(j=0;j<V;j++)
                m2[i][j] = 3;
        // printf("Multiplication of the Matrices:\n");
        for(i=0;i<V;i++)
        {
            for(j=0;j<V;j++)
            {
                mult[i][j]=0;
                for(k=0;k<V;k++)
                    mult[i][j]+=m1[i][k]*m2[k][j];
                // printf("%d\t",mult[i][j]);
            }
            // printf("\n");
        }
    return 0;
}
