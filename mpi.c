/**
 * Created on - Sat july 18 2020 11:15:32
 * @author Berkant Gunes 20160807013
 * Write an MPI application that performs serial and parallel matrix multiplication and checks the results for equality
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>

int rank, size;
MPI_Status status;


int* serialMult(int* matrixA, int* matrixB, int n);
int* parallelMult(int* matrixA, int* matrixB, int n);
bool isMatrixEqual(int* CSerial, int* CParallel);

int main(int argc , char * argv[])
{   
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    int *matrixA, *matrixB, *CSerial, *CParallel;
    int n = 0;
    double serialTime, parallelTime, equalityTime, totalTime;
    

    if(rank == 0) {
        int i, j, k=0;
        printf("Please Enter N: ");
        scanf("%d", &n);
        totalTime = MPI_Wtime();//Time starts after entering the value of N.
        matrixA = (int*)malloc(n * 16 * sizeof(int));
        matrixB = (int*)malloc(16 * n * sizeof(int));

    
    //---------------------- MATRIX A---------------------------------------
    	for(int i = 0; i < n; ++i)
        	for(int j = 0; j < 16; ++j)
            	matrixA[i * 16 + j] = rand() % 100;//Matrix A is filled randomly
        
    	FILE *fileA = fopen("a.txt", "w+");
    		for (int i = 0 ; i < n ; i++)
   			 {
        		for (int j = 0 ; j < 16 ; j++)
        		{
           			 fprintf(fileA, "%d ", matrixA[i * 16 + j]);//A matrix content NX16
       				 }
        				fprintf(fileA, "\n");
    					}
    					fclose(fileA);
        
        
    //---------------------- MATRIX B---------------------------------------
    	for(int i = 0; i < 16; ++i)
        	for(int j = 0; j < n; ++j)
            	matrixB[i * n + j] = rand() % 100;//Matrix B is filled randomly

    	FILE *fileB = fopen("b.txt", "w+");
    		for (int i = 0 ; i < 16 ; i++)
   			 {
        		for (int j = 0 ; j < n ; j++)
        		{
           			 fprintf(fileB, "%d ", matrixB[i * n + j]);//B matrix content 16xN
       				 }
        				fprintf(fileB, "\n");
    					}
    					fclose(fileB);
    					
    	
	//---------------------- MATRIX CSerial Result ---------------------------------------				
        serialTime = MPI_Wtime();
        CSerial = serialMult(matrixA, matrixB, n);
        serialTime = MPI_Wtime() - serialTime;        
    	FILE *fileSerial = fopen("CSerial.txt", "w+");
    		for (int i = 0 ; i < n ; i++)
   			 {
        		for (int j = 0 ; j < n ; j++)
        		{
           			 fprintf(fileSerial, "%d ", CSerial[i * n + j]);//CSerial result matrix content
       				 }
        				fprintf(fileSerial, "\n");
    					}
    					fclose(fileSerial);
        
    }

    if (rank == 0)
    {
        parallelTime = MPI_Wtime();
    }

    CParallel = parallelMult(matrixA, matrixB, n);

    if (rank == 0)
    {
	
		//---------------------- MATRIX CParallel Result ---------------------------------------			
        parallelTime = MPI_Wtime() - parallelTime;
        FILE *fileParallel = fopen("CParallel.txt", "w+");
    		for (int i = 0 ; i < n ; i++)
   			 {
        		for (int j = 0 ; j < n ; j++)
        		{
           			 fprintf(fileParallel, "%d ", CParallel[i * n + j]);//CParallel result matrix content
       				 }
        				fprintf(fileParallel, "\n");
    					}
    					fclose(fileParallel);
    					
        equalityTime = MPI_Wtime();
	

    
    	bool isEqual = isMatrixEqual(CSerial, CParallel);

		
		
        equalityTime = MPI_Wtime() - equalityTime;


        if(isEqual){
        	printf("\nMatrices equal\n\n");
        	
		}
            
        else{
        	printf("\nMatrices not equal\n\n");
		}

		printf("Serial Time        : %f seconds\n", serialTime);
        printf("Parallel Time      : %f seconds\n", parallelTime);
        printf("Equality Time      : %f seconds\n", equalityTime);
        totalTime = MPI_Wtime() - totalTime;
        printf("Total Computation Time : %f seconds\n", totalTime);
}
        

    MPI_Finalize();
    return 0;
}

int* serialMult(int* matrixA, int* matrixB, int n)
{
    int i, j, k;

    int* CSerial = (int*)malloc(n * n * sizeof(int));
    for(i = 0; i < n; ++i)
    {
        for(j = 0; j < n; ++j)
        {
            CSerial[i * n + j] = 0;
            for(k = 0; k < 16; ++k)
            {
                CSerial[i * n + j] += matrixA[i * 16 + k] * matrixB[k * n + j];
            }
        }
    }
    return CSerial;
}

int* parallelMult(int* matrixA, int* matrixB, int n)
{
    int i, j, k=0, l, u;
    int* CParallel;

    if(rank == 0) {
        int section, worked = 0;
        section = n / (size-1);
        if(section == 0)
            section++;
        CParallel = (int*)malloc(n * n * sizeof(int*));

        for (i = 1; i < size ; i++)
        {
            l = (i-1) * section;
            if(worked == n)
                u = l;
            else if(((i + 1) == size) && (n % (size-1) != 0)) {
                u = n;
            }
            else {
                u = l + section;
            }
            worked += u - l;
            MPI_Send(&l, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&u, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
            MPI_Send(&n, 1, MPI_INT, i, 3, MPI_COMM_WORLD);
            MPI_Send(&matrixA[l * 16], (u - l) * 16, MPI_INT, i, 4, MPI_COMM_WORLD);
            MPI_Send(matrixB, 16 * n, MPI_INT, i, 5, MPI_COMM_WORLD);
        }
        for (i = 1; i < size ; i++)
        {
            MPI_Recv(&l, 1, MPI_INT, i, 6, MPI_COMM_WORLD, &status);
            MPI_Recv(&u, 1, MPI_INT, i, 7, MPI_COMM_WORLD, &status);
            MPI_Recv(&CParallel[l * n], (u - l) * n, MPI_INT, i, 8, MPI_COMM_WORLD, &status);
        }
        return CParallel;
    }
    else if(rank > 0){
        MPI_Recv(&l, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&u, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&n, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
        matrixA = (int*)malloc(u * 16 * sizeof(int));
        MPI_Recv(&matrixA[l * 16], (u - l) * 16, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);
        matrixB = (int*)malloc(16 * n * sizeof(int));
        MPI_Recv(matrixB, 16 * n, MPI_INT, 0, 5, MPI_COMM_WORLD, &status);

        CParallel = (int*)malloc(u * n * sizeof(int));
        for (i = l * n ; i < u * n ; i++) {
            CParallel[i] = 0;
        }

        for (i = l; i < u; i++)
            for (j = 0; j < n; j++)
                for (k = 0; k < 16; k++)
                    CParallel[i * n + j] += matrixA[i * 16 + k] * matrixB[k * n + j];

        MPI_Send(&l, 1, MPI_INT, 0, 6, MPI_COMM_WORLD);
        MPI_Send(&u, 1, MPI_INT, 0, 7, MPI_COMM_WORLD);
        MPI_Send(&CParallel[l * n], (u - l) * n, MPI_INT, 0, 8, MPI_COMM_WORLD);
        return CParallel;
    }
}
bool isMatrixEqual(int* CSerial, int* CParallel)
{
	int i, j,n=16; 
    for (i = 0; i < n; i++) 
        for (j = 0; j < n; j++) 
            if (CSerial[i * n + j] !=CParallel[i * n + j]) 
                return 0; 
    return 1; 

}


