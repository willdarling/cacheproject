/*
 * trans.k - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"
 #include <stdlib.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* transpose_32x32 - matrix transpose function optimized for a 32x32 matrix */
void transpose_32x32(int M, int N, int A[N][M], int B[M][N]) 
{
    /* w/ diagonal support */
    int jj, kk, j, k, temp, i, iflag;

    iflag = 0;
    for(jj = 0; jj < N; jj += 8) {
        for(kk = 0; kk < M; kk += 8) {
            for(j = jj; j < jj+8; j++) {
                for(k = kk; k < kk+8; k++) {
                    if(j != k) {
                        B[k][j] = A[j][k];                        
                    }
                    else {
                        i = j;
                        temp = A[j][k];
                        iflag = 1;
                    }
                }
                if(iflag) {
                    B[i][i] = temp;
                    iflag = 0;
                }
            }
        }
    }
    /* w/ loop unroll 
    int jj, kk, j, t1, t2, t3, t4, t5, t6, t7, t8;

    for(jj = 0; jj < N; jj += 8) {
        for(kk = 0; kk < M; kk += 8) {
            for(j = jj; j < jj+8; j++) {
                t1 = A[j][kk];
                t2 = A[j][kk+1];
                t3 = A[j][kk+2];
                t4 = A[j][kk+3];
                t5 = A[j][kk+4];
                t6 = A[j][kk+5];
                t7 = A[j][kk+6];
                t8 = A[j][kk+7];
                B[kk][j] = t1;
                B[kk+1][j] = t2;
                B[kk+2][j] = t3;
                B[kk+3][j] = t4;
                B[kk+4][j] = t5;
                B[kk+5][j] = t6;
                B[kk+6][j] = t7;
                B[kk+7][j] = t8;                
            }
        }
    }
    */
}
/* transpose_32x64 - matrix transpose function optimized for a 32x64 matrix */
void transpose_32x64(int M, int N, int A[N][M], int B[M][N])
{
    int jj, kk, j, k, temp, i, iflag;

    iflag = 0;
    for(jj = 0; jj < N; jj += 8) {
        for(kk = 0; kk < M; kk += 4) {
            for(j = jj; j < jj+8; j++) {
                for(k = kk; k < kk+4; k++) {
                    if(j != k) {
                        B[k][j] = A[j][k];                        
                    }
                    else {
                        i = j;
                        temp = A[j][k];
                        iflag = 1;
                    }
                }
                if(iflag) {
                    B[i][i] = temp;
                    iflag = 0;
                }
            }
        }
    }
}
/* transpose_64x64 - matrix transpose function optimized for a 64x64 matrix */
void transpose_64x64(int M, int N, int A[N][M], int B[M][N]) 
{
    int jj, kk, j, t1, t2, t3, t4;

    for(jj = 0; jj < N; jj += 4) {
        for(kk = 0; kk < M; kk += 4) {
            for(j = jj; j < jj+4; j++) {
                t1 = A[j][kk];
                t2 = A[j][kk+1];
                t3 = A[j][kk+2];
                t4 = A[j][kk+3];
                B[kk][j] = t1;
                B[kk+1][j] = t2;
                B[kk+2][j] = t3;
                B[kk+3][j] = t4;                
            }
        }
    }
}

/* min: trivial min function */
int min(int a, int b) {
    return a<b?a:b;
}

/* transpose_61x67 - matrix transpose function optimized for a 61x67 matrix 
 * Richard Pan gave me the idea to guess and check my block size
 */
void transpose_61x67(int M, int N, int A[N][M], int B[M][N]) 
{
    int jj, kk, j, k, temp, i, iflag;

    iflag = 0;
    for(jj = 0; jj < N; jj += 14) {
        for(kk = 0; kk < M; kk += 3) {
            for(j = jj; j < min(jj+14,67); j++) {
                for(k = kk; k < min(kk+3,61); k++) {
                    if(j != k) {
                        B[k][j] = A[j][k];                        
                    }
                    else {
                        i = j;
                        temp = A[j][k];
                        iflag = 1;
                    }
                }
                if(iflag) {
                    B[i][i] = temp;
                    iflag = 0;
                }
            }
        }
    }
}


/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if(M==32 && N==32)
        transpose_32x32(M, N, A, B);
    else if(M==32 && N==64)
        transpose_32x64(M, N, A, B);
    else if(M==64 && N==64)
        transpose_64x64(M, N, A, B);
    else if(M==61 && N==67)
        transpose_61x67(M, N, A, B);
    else {
        fprintf(stderr, "error(transpose_submit): unoptimized case\n");
        exit(1);
    }
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    //registerTransFunction(trans, trans_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

