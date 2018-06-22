#include <stdio.h>
#include <tgmath.h>
#include <time.h>
#include "libs/matrix.h"
#include "libs/fractals.h"

int main()
{   
    int start = time(NULL);
    double complex center = -0.745428 - 0.113009 * I;
    double complex constant = -0.7269 + 0.1889 * I;

    Matrix *res = Get_Julia(2000, 16384, constant);
    Save_Matrix_To_PNG(res, "Julia.png", false);
    Free_Matrix(&res);

    res = Get_Mandelbrot(2000, 16384, center, 42e-06);
    Save_Matrix_To_PNG(res, "Mandelbrot.png", false);
    Free_Matrix(&res);
    
    printf("Temps de calcul : %ld\n", time(NULL) - start);
    
    return 0;
}
/*
    Matrix *res;
    char name[26];
    for(int i = 10; i<210 ; i++)
    {
        sprintf(name, "Result/Mandelbrot_%d.png", i);
        res = Get_Mandelbrot(500, 2048, center, 2/(double)i);
        Save_Matrix_To_PNG(res, name, false);
        Free_Matrix(&res);
    }
*/