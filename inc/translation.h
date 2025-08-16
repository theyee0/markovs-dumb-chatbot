#ifndef __TRANSLATION_H
#define __TRANSLATION_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

extern char **vocabulary;
extern int vocabulary_size;

int intcmp(const void*, const void*);
int strcmp_wrap(const void*, const void*);

struct vector {
        int size;
        int alloc;
        int *items;
};

int vector_peek(struct vector);
bool vector_push(struct vector*, int);

struct table {
        int size;
        int **items;
};


/* Load vocabulary from a dictionary containing one word per line */
bool load_vocabulary(FILE*);
/* Check if a given word is in the vocabulary */
bool valid_word(char*);

/* Get the next word as a "token" from the filestream */
int fgettok(FILE*);

/* Given a word, find the corresponding token */
int lookup_tok(char*);
/* Given a token, find the corresponding word */
char* lookup_word(int);

#endif
