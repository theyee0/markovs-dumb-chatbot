#include "translation.h"

char **vocabulary = NULL;
int vocabulary_size = 0;

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

bool vector_init(struct vector *v) {
        v->size = 0;
        v->alloc = 1;
        v->items = malloc(sizeof(*v->items));

        return true;
}

int vector_peek(struct vector v) {
        return v.items[v.size - 1];
}

bool vector_push(struct vector *v, int item) {
        if (v->size >= v->alloc) {
                v->alloc *= 2;
                v->items = realloc(v->items, v->alloc * sizeof(*(v->items)));
        }

        v->items[v->size++] = item;
        return true;
}

bool vector_free(struct vector *v) {
        free(v->items);
        v->items = NULL;
        v->size = 0;
        v->alloc = 0;
        return true;
}

int first_greater(int *v, int n, int t) {
        int l = 0;
        int r = n - 1;
        int m;

        while (r - l > 1) {
                m = (r + l) / 2;

                if (v[m] > t) {
                        r = m;
                } else if (v[m] < t) {
                        l = m;
                } else {
                        return m;
                }
        }

        if (v[l] >= t) {
                return l;
        } else {
                return r;
        }
}

bool table_free(struct table* table) {
        int i;

        for (i = 0; i < table->size; i++) {
                free(table->items[i]);
        }

        free(table->items);
        table->size = 0;
        return true;
}

bool load_vocabulary(FILE* fp) {
        const int buf_size = 1024;
        char b[buf_size];

        while (fgets(b, buf_size, fp) != NULL) {
                vocabulary_size++;
                vocabulary = realloc(vocabulary, vocabulary_size * sizeof(*vocabulary));
                b[strcspn(b, "\n")] = '\0';
                vocabulary[vocabulary_size - 1] = strdup(b);
        }

        qsort(vocabulary, vocabulary_size, sizeof(*vocabulary), strcmp_wrap);

        return true;
}

bool free_vocabulary(void) {
        int i;

        for (i = 0; i < vocabulary_size; i++) {
                free(vocabulary[i]);
        }

        free(vocabulary);

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
                b[len++] = tolower(c);

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
