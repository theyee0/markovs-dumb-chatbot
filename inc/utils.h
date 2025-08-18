#ifndef __UTILS_H
#define __UTILS_H

#include "structures.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

extern char **vocabulary;
extern int vocabulary_size;

int intcmp(const void*, const void*);
int strcmp_wrap(const void*, const void*);

int first_greater(int *v, int n, int t);

/* Load vocabulary from a dictionary containing one word per line */
bool load_vocabulary(FILE*);
bool free_vocabulary(void);
/* Check if a given word is in the vocabulary */
bool valid_word(char*);

/* Get the next word as a "token" from the filestream */
int fgettok(FILE*);

/* Given a word, find the corresponding token */
int lookup_tok(char*);
/* Given a token, find the corresponding word */
char* lookup_word(int);

#endif
