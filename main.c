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
double Get_Percentage(Matrix *Image)
{
    int count = 0;
    for(int i = 0; i<Image->rows; i++)
    {
        for(int j = 0; j<Image->columns; j++)
        {
            if(Image->data[i][j] == Image->max)
            {
                count++;
            }
        }
    }
    return (double)(count*100)/(double)(Image->rows * Image->columns);
}
double Unique_Colors_Percentage(Matrix *Image)
{
    int nb_colors = 0;
    int colors[Image->max+1];
    int treshold = (Image->rows*Image->columns)/10;
    for(int i = 0; i<Image->rows; i++)
    {
        for(int j = 0; j<Image->columns; j++)
        {
            colors[Image->data[i][j]]++;
        }
    }
    for(int i = 0; i<(Image->max+1); i++)
    {
        if (colors[i] && colors[i] < treshold)
        {
            nb_colors++;
        }
    }
    return (double)(nb_colors*100)/(double)(Image->max+1);
}
void Get_Selected_Julia_Set()
{
    Matrix *test, *julia;
    int size = 2048;
    int test_size = 512;
    int iterations = 1<<11;
    double complex constant;
    char name[37];
    for(int i = -100; i < 101; i++)
    {
        for(int j = 0; j < 101; j++)
        {
            constant = 0.01 * (i + j*I) - (i>0 ? 0.001 : 0);
            test = Get_Julia(test_size, iterations, constant);
            if(Get_Percentage(test) < 5.0 && Unique_Colors_Percentage(test) > 18.5)
            {
                julia = Get_Julia(size, iterations, constant);
                for(int k = 0; k<2; k++)
                {
                    sprintf(name, "Julia/%lf + %lf*I - %c.png", creal(constant), cimag(constant), (k ? 'S' : 'O'));
                    Save_Matrix_To_PNG(julia, (bool)k, name);
                }
                Free_Matrix(&julia);
            }
            Free_Matrix(&test);
        }
    }
}
int main()
{
    //Generate_Julia_Set();
    Get_Selected_Julia_Set();
    
    /*
    Matrix *res[2];
    int size = 512;
    int iterations = 1<<11;
    double complex constant = -1 + 0.28*I;
    //double complex center = -1.017838801 + -0.2830689082*I;
    //double zoom = 1.5/2132;

    int start = time(NULL);
    res[0] = Get_Julia(size, iterations, constant);
    //res[1] = Get_Mandelbrot(size, iterations, center, zoom);
    printf("Temps de calculs : %ld\n", time(NULL)-start);

    printf("Pourcentage : %lf, Couleurs uniques : %lf\n", Get_Percentage(res[0]), Unique_Colors_Percentage(res[0]));
    /*
    Save_Matrix_To_PNG(res[0], false, "Julia.png");
    Free_Matrix(res);
    Save_Matrix_To_PNG(res[1], false, "Mandelbrot.png");
    Free_Matrix(res + 1);
    */
    return 0;
}
