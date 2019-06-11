/*
openmp_inclusiveScan.c

Jordan Kremer
Dalton Bohning

Usage:
    gcc -fopenmp -DARRAY_SIZE=someNumber -DNUM_THREADS=someNumber -o openmp_inclusiveScan openmp_inclusiveScan.c
*/


#include <stdlib.h>
#include <stdio.h>
#include "omp.h"



//See:  https://www.cs.fsu.edu/~engelen/courses/HPC/Synchronous.pdf
void inclusive_scan(float *x, float *z)
{

int nthreads, tid, work, lo, hi, i, j, n;

n = ARRAY_SIZE;
omp_set_num_threads(NUM_THREADS);
#pragma omp parallel shared(n, nthreads, x, z) private(i, j, tid, work, lo, hi)
{
    #pragma omp single
    nthreads = omp_get_num_threads(); //assumes nthreads = 2^k
    tid = omp_get_thread_num();
    work = (n + nthreads-1) / nthreads;
    lo = work * tid;
    hi = lo + work;
    if (hi > n)
    hi = n;

    for (i = lo+1; i < hi; i++)
        x[i] = x[i] + x[i-1];
    z[tid] = x[hi-1];
    #pragma omp barrier
 
    for (j = 1; j < nthreads; j = 2*j)
    {
        if (tid >= j)
        z[tid] = z[tid] + z[tid - j];
        #pragma omp barrier
    }
    for (i = lo; i < hi; i++)
        x[i] = x[i] + z[tid] - x[hi-1];
}

}



int main(void)
{
    float *x = (float*) malloc(ARRAY_SIZE * sizeof(float));
    float *z = (float*) malloc(ARRAY_SIZE * sizeof(float));

    for(int i = 0; i < ARRAY_SIZE; ++i)
    {
        x[i] = i; //change
        z[i] = i;
    }

    inclusive_scan(x, z);

    for(int i = 0; i < ARRAY_SIZE; ++i)
    {
        printf("%.0f ", x[i]);
        if((i+1) % 10 == 0){
            printf("\n");
        }
    }
    printf("\n");
}
