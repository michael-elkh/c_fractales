#include <stdlib.h>
#include <pthread.h>
#include <tgmath.h>
#include <time.h>
#include <unistd.h>
#include "matrix.h"
#include "secured_alloc.h"

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
    int k;

    for (int i = 0; i < result->rows; i++)
    {
        for (int j = 0; j < result->columns; j++)
        {
            k = 0;
            temp = origin + j * real_step + i * imag_step * I;
            //Stop computation for point, if the element is eliminated
            while (cabs(temp) <= threshold && k < result->max)
            {
                temp = temp * temp + constant;
                k++;
            }
            result->data[i][j] = k;
        }
    }

    return NULL;
}
Matrix *Get_Julia(int size, int iterations, double complex constant)
{
    Matrix *result = New_Matrix(size, size);
    Matrix subs[THREADS];
    result->max = iterations;
    int sub_size = size / THREADS;

    double threshold = (1.0 + sqrt(1 + 4 * cabs(constant))) / 2;

    //Submatrix size for threads
    double chunk_size = 2.0 * threshold / (double)THREADS;
    double complex origin[THREADS], limit[THREADS];
    //Parameters for threads
    void ***vars = salloc(sizeof(void **) * THREADS);
    pthread_t threads_ids[THREADS];

    for (int i = 0; i < THREADS; i++)
    {
        vars[i] = salloc(sizeof(void *) * 4);
        subs[i] = Sub_Matrix(result, i * sub_size, sub_size);
        vars[i][0] = (void *)&subs[i];

        origin[i] = -1 * threshold + (-1 * threshold + i * chunk_size) * I;
        limit[i] = threshold + (threshold - (THREADS - (i + 1)) * chunk_size) * I;
        vars[i][1] = (void *)&(origin[i]);
        vars[i][2] = (void *)&(limit[i]);
        vars[i][3] = (void *)(&constant);

        //Multithreading
        pthread_create(&(threads_ids[i]), NULL, Compute_Julia_Plane_Chunk, (void *)vars[i]);
    }
    for (int i = 0; i < THREADS; i++)
    {
        pthread_join(threads_ids[i], NULL);
    }

    free(vars);
    return result;
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
        }
    }
    return NULL;
}
Matrix *Get_Mandelbrot(int size, int iterations, double complex center, double radius)
{
    Matrix *result = New_Matrix(size, size);
    Matrix subs[THREADS];
    result->max = iterations;
    int sub_size = size / THREADS;

    //Subplane size for threads
    double chunk_size = 2 * radius / THREADS;
    double complex origin[THREADS], limit[THREADS];
    //Parameters for threads
    void ***vars = salloc(sizeof(void **) * THREADS);
    pthread_t threads_ids[THREADS];

    for (int i = 0; i < THREADS; i++)
    {
        vars[i] = salloc(sizeof(void *) * 3);
        subs[i] = Sub_Matrix(result, i * sub_size, sub_size);
        vars[i][0] = (void *)&subs[i];

        origin[i] = creal(center) - radius + (cimag(center) - radius + i * chunk_size) * I;
        limit[i] = creal(center) + radius + (cimag(center) + radius - (THREADS - (i + 1)) * chunk_size) * I;
        vars[i][1] = (void *)&(origin[i]);
        vars[i][2] = (void *)&(limit[i]);

        pthread_create(&(threads_ids[i]), NULL, Compute_Mandelbrot_Plane_Chunk, (void *)(vars[i]));
    }
    for (int i = 0; i < THREADS; i++)
    {
        pthread_join(threads_ids[i], NULL);
    }

    free(vars);
    return result;
}