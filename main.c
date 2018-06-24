#include <stdio.h>
#include <tgmath.h>
#include <time.h>
#include "libs/matrix.h"
#include "libs/fractals.h"

double Square_Sm(double complex value)
{
    return creal(value) * creal(value) + cimag(value) * cimag(value);
}
int main()
{    
    Matrix *res[2];
    int size = 500;
    int iterations = 1<<10;
    double complex constant = -0.835 - 0.2321*I;
    double complex center = -1.017838801 + -0.2830689082*I;
    double zoom = 1.5/2132;
    
    int start = time(NULL);
    res[0] = Get_Julia(size, iterations, constant);
    res[1] = Get_Mandelbrot(size, iterations, center, zoom);
    printf("Temps de calculs : %ld\n", time(NULL)-start);
    
    Save_Matrix_To_PNG(res[0], true, "Julia.png");
    Free_Matrix(res);
    Save_Matrix_To_PNG(res[1], true, "Mandelbrot.png");
    Free_Matrix(res + 1);

    return 0;
}