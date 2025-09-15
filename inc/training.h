#ifndef __TRAINING_H
#define __TRAINING_H

#include "uthash.h"
#include "utarray.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void load_vocabulary_from_model(FILE *model, UT_array *vocabulary);
void load_model(UT_array **table, UT_array *vocabulary, FILE *model);
void rebase_model(UT_array *table, UT_array *vocabulary, FILE *model);
void write_model(UT_array*, UT_array*, FILE*);

void table_init(UT_array**, UT_array*);

void table_depfx(UT_array*);
void table_pfx(UT_array*);

void table_push(UT_array*, const int, const int, const int);
void table_inc(UT_array*, const int, const int);
void read_data(UT_array*, FILE*, const UT_array*);

#endif
