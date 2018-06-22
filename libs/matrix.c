/*
	El Kharroubi Michaël
	Description : Matrix management.
	<matrix.c>
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "libpng.h"
#include "matrix.h"
#include "secured_alloc.h"

//Constants
#define BUFF_SIZE 50
#define MAX_CHARS_BY_LINE 70 //PGM file must have less than 70 chars by lines.
#define MAX_VALUE_FOR_PGM 65536
#define IS_NUMBER(CHAR) (CHAR > 47 && CHAR < 58)

//External functions
/*
Description : Create a pixel matrix, and initialize it with 0.
	Warning : matrices use malloc, don't forget to call Free_Matrix.
Parameters :
	rows : Number of rows.
	columns : Number of columns.
Return : The address of the newly created matrix.
*/
Matrix *New_Matrix(int rows, int columns)
{
    Matrix *mat = salloc(sizeof(Matrix));
    mat->max = MAX_VALUE_FOR_PGM;
    mat->rows = rows;
    mat->columns = columns;
    mat->data = salloc(sizeof(int *) * rows);
    //Initialize one array for the content to ensure contiguous memory.
    int *content = salloc(sizeof(int) * rows * columns);

    //Initialize the matrix
    for (int i = 0; i < rows * columns; i++)
    {
        content[i] = 0;
    }
    for (int i = 0; i < rows; i++)
    {
        mat->data[i] = content + i * columns;
    }
    return mat;
}
/*
Description : Free the dynamic memory allocation for the matrix.
Parameters :
	matrix : Pointer to a matrix.
Return : void.
*/
void Free_Matrix(Matrix **matrix)
{
    free((*matrix)->data[0]);
    free((*matrix)->data);
    free(*matrix);
    *matrix = NULL;
}
/*
Description : Read a PGM image and store the result in a pixel matrix. 
	Warning : matrixes use malloc, don't forget to call Free_Matrix.
Parameters :
	Path : Image path.
Return : The address of the newly created pixel matrix.
*/
Matrix *Read_PGM(char *Path)
{
    FILE *image = fopen(Path, "r");
    assert(image && "Error while loading image");

    Matrix *result;
    char buffer[BUFF_SIZE];
    int lineNumber = 0, rows, columns;

    //While there are header lines left to read.
    while (lineNumber < 3)
    {
        if (fgets(buffer, BUFF_SIZE, image) != NULL)
        {
            NULL;
        }
        //Comments
        if (buffer[0] == '#')
        {
            continue;
        }
        switch (lineNumber)
        {
        case 0:
            assert(!strcmp(buffer, "P2\n") && "Error wrong format");
            break;
        case 1:
            columns = atoi(strtok(buffer, " "));
            rows = atoi(strtok(NULL, " "));
            assert(columns > 0 && rows > 0 && "Error invalid shape");
            result = New_Matrix(rows, columns);
            break;
        case 2:
            assert(atoi(buffer) < MAX_VALUE_FOR_PGM && atoi(buffer) > 0 && "Error max value {N} for pgm must be 0 < N < 65'536");
            result->max = atoi(buffer);
            break;
        default:
            exit(1);
            break;
        }
        lineNumber++;
    }

    //Storing pixels
    int i, x, y = 0;
    while (y < rows)
    {
        x = 0;
        while (x < columns)
        {
            i = -1;
            do
            {
                buffer[++i] = fgetc(image);
            } while (IS_NUMBER(buffer[i]));

            //If i > 0, it means that it's a number.
            if (i)
            {
                buffer[i] = '\0';
                result->data[y][x] = atoi(buffer);
                assert(result->data[y][x] <= result->max && "Error wrong pixel value.");
                x++;
            }
            else
            {
                assert(buffer[i] != EOF && "Error while reading the image, wrong shape, verify the image header.");
            }
        }
        y++;
    }
    fclose(image);
    return result;
}
/*
Description : Store a pixel matrix as a PGM image. 
Parameters :
	Image : The address of the pixel matrix.
    Path : Image path.
Return : nothing.
*/
void Write_PGM(Matrix *Image, char *Path)
{
    FILE *dest = fopen(Path, "w");
    assert(dest && "Error while opening the destination file.");
    //Header
    fprintf(dest, "P2\n");
    fprintf(dest, "# CREATOR: XV Version 3.10a  Rev: 12/29/94\n");
    fprintf(dest, "%d %d\n", Image->columns, Image->rows);
    fprintf(dest, "%d\n", Image->max);

    int written_numbers = 0;
    //Var for values alignement.
    int max_size = ceil(log10(Image->max));
    int max_values_per_lines = MAX_CHARS_BY_LINE / (max_size + 1);
    char formatted_val[max_size + 1];

    for (int i = 0; i < Image->rows; i++)
    {
        for (int j = 0; j < Image->columns; j++)
        {
            if (written_numbers == max_values_per_lines)
            {
                written_numbers = 0;
                fprintf(dest, "\n");
            }
            //Format value for alignement.
            sprintf(formatted_val, "%d", Image->data[i][j]);
            for (int i = strlen(formatted_val); i < max_size; i++)
            {
                formatted_val[i] = ' ';
            }
            formatted_val[max_size] = '\0';

            fprintf(dest, "%s ", formatted_val);
            written_numbers++;
        }
    }

    fclose(dest);
}
Matrix *Fuse_Martices(Matrix **Matrices, int elements)
{
    int new_rows, new_columns, rows, columns;
    rows = Matrices[0]->rows;
    columns = Matrices[0]->columns;
    if (rows < columns)
    {
        new_rows = rows * elements;
        new_columns = columns;
    }
    else
    {
        new_rows = rows;
        new_columns = columns * elements;
    }

    Matrix *res = New_Matrix(new_rows, new_columns);
    res->max = Matrices[0]->max;
    
    for (int k = 0; k < elements; k++)
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                if(rows < columns)
                {
                    res->data[i+k*rows][j] = Matrices[k]->data[i][j];
                }
                else
                {
                    res->data[i][j+k*columns] = Matrices[k]->data[i][j];
                }
            }
        }
        Free_Matrix(&(Matrices[k]));
    }
    free(Matrices);

    return res;
}
void Save_Matrix_To_PNG(Matrix *Image, char *Path, bool smooth)
{
    bitmap_t img;

    img.height = Image->rows;
    img.width = Image->columns;
    img.pixels = salloc(img.width * img.height * sizeof(pixel_t));

    for (int y = 0; y < (int)img.height; y++)
    {
        for (int x = 0; x < (int)img.width; x++)
        {
            pixel_t *pixel = pixel_at(&img, x, y);
            Set_RGB(pixel, Image->data[y][x], smooth);
        }
    }

    save_png_to_file(&img, Path);

    free(img.pixels);
}