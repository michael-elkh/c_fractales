#include <stdio.h>
#include <tgmath.h>
#include <time.h>
#include "libs/matrix.h"
#include "libs/fractals.h"

void Generate_Julia_Set()
{
    Matrix *julia;
    int size = 2048;
    int iterations = 1<<11;
    double complex constant;
    char name[37];
    for(int i = -10; i < 11; i++)
    {
        for(int j = 0; j < 11; j++)
        {
            constant = 0.1 * (i + j*I) - (i>0 ? 0.01 : 0);
            julia = Get_Julia(size, iterations, constant);
            for(int k = 0; k<2; k++)
            {
                sprintf(name, "Julia/%lf + %lf*I - %c.png", creal(constant), cimag(constant), (k ? 'S' : 'O'));
                Save_Matrix_To_PNG(julia, (bool)k, name);
            }
            Free_Matrix(&julia);
        }
    }
}
int main()
{
    Generate_Julia_Set();
    /*
    Matrix *res[2];
    int size = 2000;
    int iterations = 1<<10;
    double complex constant = -0.624 + 0.435*I;
    double complex center = -1.017838801 + -0.2830689082*I;
    double zoom = 1.5/2132;

    int start = time(NULL);
    res[0] = Get_Julia(size, iterations, constant);
    res[1] = Get_Mandelbrot(size, iterations, center, zoom);
    printf("Temps de calculs : %ld\n", time(NULL)-start);

    Save_Matrix_To_PNG(res[0], false, "Julia.png");
    Free_Matrix(res);
    Save_Matrix_To_PNG(res[1], false, "Mandelbrot.png");
    Free_Matrix(res + 1);
    */
    return 0;
}
