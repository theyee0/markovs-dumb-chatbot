#ifndef __UTILS_H
#define __UTILS_H

#include "uthash.h"
#include "utarray.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

struct entry {
        int key;
        int count;
};

int intcmp(const void*, const void*);
int strcmp_wrap(const void*, const void*);

int first_greater(const UT_array *v, const int s);

/* Load vocabulary from a dictionary containing one word per line */
void load_vocabulary_file(FILE*, UT_array*);
bool free_vocabulary(void);

void remove_vocabulary_duplicates(UT_array *vocabulary);

/* Check if a given word is in the vocabulary */
bool valid_word(const char*, const UT_array*);

/* Get the next word as a "token" from the filestream */
int fgettok(FILE*, const UT_array*);

/* Given a word, find the corresponding token */
int lookup_tok(const char*, const UT_array*);
/* Given a token, find the corresponding word */
char* lookup_word(const int, const UT_array*);

#endif
