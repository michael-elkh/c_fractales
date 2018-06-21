/*
	El Kharroubi Michaël
    Description : Malloc wrapper body.
	<secured_alloc.c>
*/
#include "secured_alloc.h"
#include "assert.h"

void *salloc(int size)
{
    void *temp = malloc(size);
    if(!temp)
    {
        exit(EXIT_FAILURE);
    }
    return temp;
}