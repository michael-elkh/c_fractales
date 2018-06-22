#include <stdio.h>
#include <tgmath.h>
#include <time.h>
#include "libs/matrix.h"
#include "libs/fractals.h"

int main()
{   
    /*
    double complex constant = -0.7269 + 0.1889 * I;
    Matrix *res = Get_Julia(1000, 16384, constant);
    Save_Matrix_To_PNG(res,  true, "Julia.png");
    Free_Matrix(&res);
    */
    double complex center = -0.745428 - 0.113009 * I;
    Matrix *res = Get_Mandelbrot(15000, 100, center, 42e-06);
    
    Save_Matrix_To_PNG(res, true, "Mandelbrot.png");
    
    //Save_Matrix_To_PNG(res, "Mandelbrot.png", true);
    Free_Matrix(&res);
    return 0;
}