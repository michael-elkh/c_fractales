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
#include <png.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include "matrix.h"
#include "secured_alloc.h"

#include <time.h>
//Constants
#define BUFF_SIZE 50
#define MAX_CHARS_BY_LINE 70 //PGM file must have less than 70 chars by lines.
#define MAX_VALUE_FOR_PGM 65536
#define IS_NUMBER(CHAR) (CHAR > 47 && CHAR < 58)
#define PIXEL_SIZE 3

#ifndef THREADS
#define THREADS sysconf(_SC_NPROCESSORS_ONLN)
#endif

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
//Submatrix aren't suposed to be freed.
Matrix Sub_Matrix(Matrix *Original, int y, int rows)
{
    assert(y + rows <= Original->rows && "Submatrix out of original matrix.");
    //Matrix sub = {Original->data + y, rows, Original->columns, Original->max};
    return (Matrix){Original->data + y, rows, Original->columns, Original->max};
}
void *Sub_Matrix_To_png_byte(void *vargrp)
{
    void **vars = (void **)vargrp;
    Matrix *image = (Matrix *)vars[0];
    png_byte **png_data = *((png_byte ***)vars[1]);
    png_structp png_str = *((png_structp *)vars[2]);
    int origin = *((int *)vars[3]);
    int limit = *((int *)vars[4]);
    bool smooth = *((bool *)vars[5]);
    //Free parameters
    free(vars);

    double max, value, r_ang;
    max = image->max;

    for (int y = origin; y < limit; y++)
    {
        png_byte *row = png_malloc(png_str, sizeof(uint8_t) * image->columns * PIXEL_SIZE);
        png_data[y] = row;
        for (int x = 0; x < image->columns; x++)
        {
            value = image->data[y][x];
            if (!value)
            {
                //Red
                *row++ = 153;
                //Green
                *row++ = 255;
                //Blue
                *row++ = 204;
            }
            else if (value == max)
            {
                //Red
                *row++ = 0;
                //Green
                *row++ = 255;
                //Blue
                *row++ = 255;
            }
            else
            {
                if (smooth)
                {
                    value = log2(value);
                }
                r_ang = 2.75 * value + 1.0;
                //Red
                *row++ = (uint8_t)(256 * sin(1.25 * r_ang) * cos(0.75 * r_ang));
                //Green
                *row++ = (uint8_t)(256 * cos((0.375 * value + 1.0)));
                //Blue
                *row++ = (uint8_t)(256 * sin((3.25 * value + 1.0)));
            }
        }
    }

    return NULL;
}
//Source : https://www.lemoda.net/c/write-png/
//Modified by : El Kharroubi Michaël
int Save_Matrix_To_PNG(Matrix *image, bool smooth, const char *path)
{
    FILE *fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_byte **row_pointers = NULL;
    int step = image->rows / THREADS;
    int origins[THREADS], limits[THREADS];
    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int depth = 8;

    fp = fopen(path, "wb");
    if (!fp)
    {
        exit(EXIT_FAILURE);
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        exit(EXIT_FAILURE);
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        exit(EXIT_FAILURE);
    }

    /* Set up error handling. */

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        exit(EXIT_FAILURE);
    }

    /* Set image attributes. */

    png_set_IHDR(png_ptr,
                 info_ptr,
                 image->columns,
                 image->rows,
                 depth,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */
    row_pointers = png_malloc(png_ptr, image->rows * sizeof(png_byte *));
    pthread_t threads_ids[THREADS];
    void **vars[THREADS];
    for (int i = 0; i < THREADS; i++)
    {
        origins[i] = i * step;
        limits[i] = (i + 1) * step + (i == THREADS - 1 ? image->rows % THREADS : 0);
        //6 = number of arguments
        vars[i] = salloc(sizeof(void *) * 6);
        vars[i][0] = image;
        vars[i][1] = &row_pointers;
        vars[i][2] = &png_ptr;
        vars[i][3] = origins + i;
        vars[i][4] = limits + i;
        vars[i][5] = &smooth;
        pthread_create(threads_ids + i, NULL, Sub_Matrix_To_png_byte, (void *)(vars[i]));
    }
    for (int i = 0; i < THREADS; i++)
    {
        pthread_join(threads_ids[i], NULL);
    }
    /* Write the image data to "fp". */

    png_init_io(png_ptr, fp);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
       "status" to a value which indicates success. */

    status = 0;

    for (int y = 0; y < image->rows; y++)
    {
        png_free(png_ptr, row_pointers[y]);
    }
    png_free(png_ptr, row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return status;
}