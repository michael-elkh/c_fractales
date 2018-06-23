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
    
    double complex constant = -0.7269 + 0.1889 * I;
    double complex center = -0.745428 - 0.113009 * I;

    int start = time(NULL);
    res[0] = Get_Julia(5000, 1<<10, constant);
    res[1] = Get_Mandelbrot(5000, 1<<10, center, 42e-06);
    printf("Temps de calculs : %ld\n", time(NULL)-start);
    
    Save_Matrix_To_PNG(res[0], false, "Julia.png");
    Free_Matrix(res);
    Save_Matrix_To_PNG(res[1], false, "Mandelbrot.png");
    Free_Matrix(res + 1);

    return 0;
}