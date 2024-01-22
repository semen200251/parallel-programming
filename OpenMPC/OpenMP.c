#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>



int main(){
    
        int N=3000;
        int **A = (int**)malloc(N * sizeof(int*));
        int **B = (int**)malloc(N * sizeof(int*));
        int **C = (int**)malloc(N * sizeof(int*));
        for (int i = 0; i < N; i++)
        {
            A[i] = (int*)malloc(N * sizeof(int));
            B[i] = (int*)malloc(N * sizeof(int));
            C[i] = (int*)malloc(N * sizeof(int));
        }
        FILE *fileA = fopen("a.txt", "r");
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                fscanf(fileA, "%d", &A[i][j]);
        fclose(fileA);

        // Считываем матрицу B из файла b.txt
        FILE *fileB = fopen("b.txt", "r");
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                fscanf(fileB, "%d", &B[i][j]);
        fclose(fileB);
        time_t begin, end;
        double time_spent;
        time(&begin);
        int i,j,k;
        #pragma omp parallel for shared(A, B, C) private(i, j, k)
        for(i = 0; i < N; i++){
            for(j = 0; j < N; j++)
            {
                C[i][j] = 0;
                for(k = 0; k < N; k++)
                    C[i][j] += A[i][k] * B[k][j];
            }
            
        }
        time(&end);
        time_spent = difftime(end, begin);
        printf("Секунд для n = 3000: %f\n", time_spent);
        FILE *fileC = fopen("c.txt", "w");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                fprintf(fileC, "%d ", C[i][j]);
            }
            fprintf(fileC, "\n");
        }
        fclose(fileC);
        for (int i = 0; i < N; i++)
        {
            free(A[i]);
            free(B[i]);
            free(C[i]);
        }
        free(A);
        free(B);
        free(C);
}
