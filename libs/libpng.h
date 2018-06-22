//Based on : https://www.lemoda.net/c/write-png/
#ifndef _LIBPNG_
#define _LIBPNG_
#include <stdint.h>
#include <stdbool.h>

/* A coloured pixel. */
typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel_t;

/* A picture. */

typedef struct
{
    pixel_t *pixels;
    size_t width;
    size_t height;
} bitmap_t;

int save_png_to_file(bitmap_t *bitmap, const char *path);
pixel_t *pixel_at(bitmap_t *bitmap, int x, int y);
int pix(int value, int max);
void Set_RGB(pixel_t *pixel, double value, bool smooth);
#endif