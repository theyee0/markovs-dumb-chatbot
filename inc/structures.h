#ifndef __STRUCTURES_H
#define __STRUCTURES_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

struct vector {
        int size;
        int alloc;
        int *items;
};

bool vector_init(struct vector*);
int vector_peek(struct vector);
bool vector_push(struct vector*, int);
bool vector_free(struct vector*);

struct table {
        int size;
        int **items;
};

bool table_free(struct table*);

#endif
