/*
	El Kharroubi Michaël
	Description : Matrix header.
	<matrix.h>
*/
#ifndef _MATRIX_
#define _MATRIX_
typedef struct Matrix Matrix;
struct Matrix
{
	int **data;
	int rows;
	int columns;
	int max;
};

Matrix *New_Matrix(int rows, int columns);
void Free_Matrix(Matrix **matrix);
Matrix *Read_PGM(char *Path);
void Write_PGM(Matrix *Image, char *Path);
Matrix *Fuse_Martices(Matrix **Matrices, int elements);

#endif