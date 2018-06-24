/*
	El Kharroubi Michaël
	Description : Matrix header.
	<matrix.h>
*/
#ifndef _MATRIX_
#define _MATRIX_
#include <stdbool.h>
typedef struct Matrix Matrix;
struct Matrix
{
	int **data;
	int rows;
	int columns;
	int max;
};

Matrix *New_Matrix(int rows, int columns, bool init);
void Free_Matrix(Matrix **matrix);
Matrix *Read_PGM(char *Path);
void Write_PGM(Matrix *Image, char *Path);
int Save_Matrix_To_PNG(Matrix *image, bool smooth, const char *path);
Matrix Sub_Matrix(Matrix *Original, int y, int rows);

#endif