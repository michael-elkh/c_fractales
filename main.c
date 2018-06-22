#include <stdio.h>
#include <tgmath.h>
#include <time.h>
#include "libs/matrix.h"
#include "libs/fractals.h"

int main()
{   
    int start = time(NULL);
    double complex constant = -0.7269 + 0.1889 * I;
    Matrix *res = Get_Julia(1000, 16384, constant);
    Save_Matrix_To_PNG(res, "Julia.png", true);
    Free_Matrix(&res);

    double complex center = -0.745428 - 0.113009 * I;
    res = Get_Mandelbrot(1000, 16384, center, 42e-06);
    Save_Matrix_To_PNG(res, "Mandelbrot.png", true);
    Free_Matrix(&res);
    
    printf("Temps de calcul : %ld\n", time(NULL) - start);
    
    return 0;
}