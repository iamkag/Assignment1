# include <stdlib.h>
# include <stdio.h>
#include "mmio.h"
#include <omp.h>
#include <time.h>
#define NUMTHREADS 10



int main(int argc, char *argv[])
{
    clock_t begin = clock();
    int ret_code;
    MM_typecode matcode;
    FILE *f;
    int M, N, nz;
    int i, *I, *J;
    //double *val;

    if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
		exit(1);
	}
    else
    {
        if ((f = fopen(argv[1], "r")) == NULL)
            exit(1);
    }

    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }


    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */

    if (mm_is_complex(matcode) && mm_is_matrix(matcode) &&
            mm_is_sparse(matcode) )
    {
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
        exit(1);
    }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) !=0)
        exit(1);


    /* reseve memory for matrices */

    I = (int *) malloc(nz * sizeof(int));
    J = (int *) malloc(nz * sizeof(int));
    //val = (double *) malloc(nz * sizeof(double));


    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */
    int line = 0;
    for(i=0; i<nz; i++)
    {
    	fscanf(f, "%d %d \n", &J[i], &I[i]);
    	//fscanf(f, "%d %d %lg\n", &I[i], &J[i], &val[i]);
        //I[i]--;  /* adjust from 1-based to 0-based */
        //J[i]--;
    	line++;
    }

    if (f !=stdin) fclose(f);

    /************************/
    /* now write out matrix */
    /************************/

    mm_write_banner(stdout, matcode);
    mm_write_mtx_crd_size(stdout, M, N, nz);
    for(i=0; i<nz; i++)
        //fprintf(stdout, "%d %d %20.19g\n", I[i]+1, J[i]+1, val[i]);
    	fprintf(stdout, "%d %d \n", I[i], J[i]);

    printf("######################\n");

    //generate CSR matrix

    int *row, *col;
    row = malloc(nz * sizeof(int));
    col = malloc(nz * sizeof(int));


    int loc = I[0];
    row[0] = loc;
    col[0] = 0;

    //printf("row[0] %d \n",row[0]);
    //printf("col[0] %d \n",col[0]);
    printf("######################\n");

    int index = 1;
    int size_of_matrix= nz;
    printf("Size of matrix %d \n",size_of_matrix);

    int z;

    for(z=1 ;z <= size_of_matrix; z++)
    {
    	//printf("z %d \n",z);

    	if((I[z] != loc)&&(z<size_of_matrix))
    	{
    		row[index] = I[z];
    		col[index] = z;

    		loc = I[z];
    		index++;
    	}
    	if(z==size_of_matrix)
    	{

    		col[index] = z - 1;
			index++;
    	}
    }

    printf("Index is:%d \n",index);


    // Calculate triangles
    int j, k;
    int count = 0;
    int x1 = 0;
    int x2 = 1;

    int n = index;
    int li = n - 2;
    int lj = n - 1;

	int *C;
	C = malloc(nz * sizeof(int));
	printf("SIZE OF MATRIX: %d\n", size_of_matrix);

	for(i=0; i < col[li]; i++)
	{
		if(i == col[x2]){x1 = x2;x2++;}
#pragma omp parallel for private(j, k) reduction(+:count)
		for(j = col[x2]; j < col[lj]; j++)
		{

			for(k = i + 1; k < col[x2]; k++)
			{
				if((J[i] == I[j] || J[i] == J[j]) && (J[k] == I[j] || J[k] == J[j]))
				{
					//printf("%d, %d, %d\n",i, j, k);
					count++;
					C[I[i]]++;
					C[I[j]]++;
					C[J[j]]++;
					//printf("C[%d]=%d, C[%d]=%d, C[%d]=%d \n",I[i], C[I[i]], I[j], C[I[j]], J[j],C[J[j]]);

				}
				//printf("i = %d, j= %d, k=%d, threadId = %d \n", i, j,k, omp_get_thread_num());

			}
		}
		x2 = x1 + 1;
	}
    clock_t end = clock();
    double time_spent = (double)(end-begin) / CLOCKS_PER_SEC;
    printf("TIME: %f \n", time_spent);
    printf("NUMBER OF TRIANGLES: %d \n", count);

    int myInt;
    printf("Enter Node for checking: \n");
    scanf("%d", &myInt);
    printf("%d Triangles in Node %d \n", C[myInt], myInt);
    


	return 0;
}

