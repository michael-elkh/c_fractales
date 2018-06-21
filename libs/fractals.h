/*
	El Kharroubi Michaël
	Description : Fractal headers.
	<fractals.h>
*/
#ifndef _FRACTALS_
#define _FRACTALS_
#include "tgmath.h"
#include "matrix.h"

Matrix *Get_Julia(int size, int iterations, double complex constant);
Matrix *Get_Mandelbrot(int size, int iterations, double complex center, double radius);
#endif