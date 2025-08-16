#include "translation.h"

char **vocabulary;
int vocabulary_size;

int intcmp(const void *_a, const void *_b) {
        const int *a = _a;
        const int *b = _b;

        return *a - *b;
}

int strcmp_wrap(const void *_a, const void *_b) {
        const char **a = (const char**)_a;
        const char **b = (const char**)_b;

        return strcmp(*a, *b);
}

int vector_peek(struct vector v) {
        return v.items[v.size - 1];
}

bool vector_push(struct vector *v, int item) {
        if (v->size >= v->alloc) {
                v->alloc *= 2;
                v->items = realloc(&(v->items), v->alloc);
        }

        v->items[v->size++] = item;
        return true;
}

bool load_vocabulary(FILE* fp) {
        const int buf_size = 1024;
        char b[buf_size];

        while (fgets(b, buf_size, fp) != NULL) {
                vocabulary_size++;
                vocabulary = realloc(&vocabulary, vocabulary_size * sizeof(*vocabulary));
                vocabulary[vocabulary_size++] = strdup(b);
        }

        qsort(vocabulary, vocabulary_size, sizeof(*vocabulary), strcmp_wrap);

        return true;
}

bool valid_word(char *w) {
        return bsearch(&w, vocabulary, vocabulary_size, sizeof(*vocabulary), strcmp_wrap);
}

int fgettok(FILE *fp) {
        const int buf_size = 1024;
        char b[buf_size];
        int len = 0;
        char c;

        while (!feof(fp) && strchr(" \n\0", c = fgetc(fp)) == NULL)
                b[len++] = c;

        b[len] = '\0';

        return lookup_tok(b);
}

int lookup_tok(char* w) {
        char **loc = bsearch(&w, vocabulary, vocabulary_size, sizeof(*vocabulary), strcmp_wrap);

        if (loc != NULL) {
                return loc - vocabulary;
        } else {
                return -1;
        }
}

char* lookup_word(int t) {
        return vocabulary[t];
}
