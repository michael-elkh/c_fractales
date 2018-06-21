#include <stdlib.h>
#include <pthread.h>
#include <tgmath.h>
#include <time.h>
#include "matrix.h"
#include "secured_alloc.h"
#include <unistd.h>
#define THREADS sysconf(_SC_NPROCESSORS_ONLN)

//https://www.geeksforgeeks.org/multithreading-c-2/
void *Compute_Julia_Plane_Chunk(void *vargp)
{
    //Recover parameters
    void **vars = (void **)vargp;
    Matrix *result = (Matrix *)vars[0];
    double complex origin = *((double complex *)vars[1]);
    double complex limit = *((double complex *)vars[2]);
    double complex constant = *((double complex *)vars[3]);
    //Free parameters
    free(vargp);

    //Init
    double threshold = (1.0 + sqrt(1 + 4 * cabs(constant))) / 2;
    double complex temp;
    double real_step = fabs(creal(origin) - creal(limit)) / result->columns;
    double imag_step = fabs(cimag(origin) - cimag(limit)) / result->rows;
    int k = 0;

    for (int i = 0; i < result->rows; i++)
    {
        for (int j = 0; j < result->columns; j++)
        {
            temp = origin + j * real_step + i * imag_step * I;
            //Stop computation for point, if the element is eliminated
            while (cabs(temp) <= threshold && k < result->max)
            {
                result->data[i][j]++;
                temp = temp * temp + constant;
                k++;
            }
            result->data[i][j] = (int)log2(1 + result->data[i][j]);
            k = 0;
        }
    }
    result->max = (int)log2(result->max);

    return NULL;
}
Matrix *Get_Julia(int size, int iterations, double complex constant)
{
    Matrix **results = salloc(sizeof(Matrix *) * THREADS);
    double threshold = (1.0 + sqrt(1 + 4 * cabs(constant))) / 2;
    //Submatrix size for threads
    double chunk_size = 2 * threshold / THREADS;
    //Parameters for threads
    void **vars;
    pthread_t threads_ids[THREADS];

    for (int i = 0; i < THREADS; i++)
    {
        vars = salloc(sizeof(void *) * 4);
        results[i] = New_Matrix(size, size / THREADS);
        results[i]->max = iterations;
        vars[0] = (void *)(results[i]);

        double complex origin = -1 * threshold + (i * chunk_size) - threshold * I;
        double complex limit = threshold - ((THREADS - (i + 1)) * chunk_size) + threshold * I;
        vars[1] = (void *)(&origin);
        vars[2] = (void *)(&limit);
        vars[3] = (void *)(&constant);

        //Multithreading
        pthread_create(&(threads_ids[i]), NULL, Compute_Julia_Plane_Chunk, (void *)vars);
    }
    for (int i = 0; i < THREADS; i++)
    {
        pthread_join(threads_ids[i], NULL);
    }

    return Fuse_Martices(results, THREADS);
}
void *Compute_Mandelbrot_Plane_Chunk(void *vargp)
{
    //Recover parameters
    void **vars = (void **)vargp;
    Matrix *result = (Matrix *)vars[0];
    double complex origin = *((double complex *)vars[1]);
    double complex limit = *((double complex *)vars[2]);
    int k;
    //Free parameters
    free(vargp);

    //Init
    double complex temp, original;
    double real_step = fabs(creal(origin) - creal(limit)) / result->columns;
    double imag_step = fabs(cimag(origin) - cimag(limit)) / result->rows;

    for (int i = 0; i < result->rows; i++)
    {
        for (int j = 0; j < result->columns; j++)
        {
            k = 0;
            original = origin + j * real_step + i * imag_step * I;
            temp = original;
            //Stop computation for point, if the element is eliminated
            while (cabs(temp) <= 2 && k < result->max)
            {
                k++;
                temp = temp * temp + original;
            }
            result->data[i][j] = k;
            //result->data[i][j] = (int)log2(1 + k);
        }
    }
    //result->max = (int)log2(result->max);

    return NULL;
}
Matrix *Get_Mandelbrot(int size, int iterations, double complex center, double radius)
{
    Matrix **results = salloc(sizeof(Matrix *) * THREADS);
    //Submatrix size for threads
    double chunk_size = 2 * radius / THREADS;
    //Parameters for threads
    void **vars;
    pthread_t threads_ids[THREADS];

    for (int i = 0; i < THREADS; i++)
    {
        vars = salloc(sizeof(void *) * 4);
        results[i] = New_Matrix(size / THREADS, size);
        results[i]->max = iterations;
        vars[0] = (void *)(results[i]);

        double complex origin = creal(center) - radius + ((cimag(center) - radius) + i * chunk_size) * I;
        double complex limit = creal(center) + radius + ((cimag(center) + radius) - (((THREADS - 1) - i) * chunk_size)) * I;
        vars[1] = (void *)(&origin);
        vars[2] = (void *)(&limit);

        //Multithreading
        pthread_create(&(threads_ids[i]), NULL, Compute_Mandelbrot_Plane_Chunk, (void *)vars);
    }
    for (int i = 0; i < THREADS; i++)
    {
        pthread_join(threads_ids[i], NULL);
    }

    return Fuse_Martices(results, THREADS);
}