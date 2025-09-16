#ifndef __TRAINING_H
#define __TRAINING_H

#include "uthash.h"
#include "utarray.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


/* Load vocabulary from a dictionary containing one word per line */
void load_vocabulary_file(FILE*, UT_array*);

/* Find words from training file and load them into vocabulary */
void load_vocabulary_from_file(FILE*, UT_array*);

/* Load vocabulary from a model */
void load_vocabulary_from_model(FILE *model, UT_array *vocabulary);

/* Remove re-entries of the same word */
void remove_vocabulary_duplicates(UT_array *vocabulary);


/* Load model for inference, with no external vocabulary or table data */
void load_model(UT_array **table, UT_array *vocabulary, FILE *model);

/* Load model with an existing vocabulary */
void rebase_model(UT_array *table, UT_array *vocabulary, FILE *model);

/* Write model out to file */
void write_model(UT_array*, UT_array*, FILE*);


/* Initialize table for training */
void table_init(UT_array**, UT_array*);

/* Convert all rows in table into prefix sum arrays */
void table_pfx(UT_array*);

/* Convert all rows from prefix sum arrays back to counts */
void table_depfx(UT_array*);

/* Add an arbitrary number to a key/count pair in table */
void table_push(UT_array*, const int, const int, const int);

/* Increment a key/count pair by 1 */
void table_inc(UT_array*, const int, const int);


/* Read data from training file and add to table */
void read_data(UT_array*, FILE*, const UT_array*);

#endif
