#ifndef __UTILS_H
#define __UTILS_H

#include "utarray.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/* key/count entry pairs */
struct entry {
        int key;
        int count;
};

int intcmp(const void*, const void*);
int strcmp_wrap(const void*, const void*);

/* Find first item greater than given integer in an int array */
int first_greater(const UT_array *v, const int s);

/* Picks characters in words that are not escape sequences or spaces */
char *fgetword(char *s, int size, FILE *stream);

/* Check if a given word is in the vocabulary */
bool valid_word(const char*, const UT_array*);

/* Get the next word as a "token" from the filestream */
int fgettok(FILE*, const UT_array*);

/* Given a word, find the corresponding token */
int lookup_tok(const char*, const UT_array*);
/* Given a token, find the corresponding word */
char* lookup_word(const int, const UT_array*);

#endif
