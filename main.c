#include <stdio.h>
#include <tgmath.h>
#include <time.h>
#include "libs/matrix.h"
#include "libs/fractals.h"

int main()
{
    int start = time(NULL);
    //double complex constant = -0.7269 + 0.1889 * I;
    double complex center = -0.745428 - 0.113009 * I;
    center = -1.25066 + 0.02012*I;
    //Matrix *res = Get_Julia(10000, 2048, constant);
    Matrix *res = Get_Mandelbrot(2000, 4096, center, 0.00022);
    printf("Temps de calcul : %ld\n", time(NULL) - start);
    Save_Matrix_To_PNG(res, "Mandelbrot.png", false);
    //Write_PGM(res, "Mandelbrot.pgm");
    Free_Matrix(&res);
    return 0;
}