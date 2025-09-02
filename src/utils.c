#include "utils.h"

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

int first_greater(const UT_array *v, const int t) {
        unsigned int l = 0;
        unsigned int r = utarray_len(v);
        unsigned int m;

        struct entry *e;

        while (r - l > 1) {
                m = (r + l) / 2;

                e = utarray_eltptr(v, m);

                if (e->count >= t) {
                        r = m;
                } else {
                        l = m;
                }
        }

        e = utarray_eltptr(v, l);

        if (e->count >= t) {
                return l;
        } else {
                return r;
        }
}

void load_vocabulary_file(FILE* fp, UT_array* vocabulary) {
        const int buf_size = 1024;
        char *b = malloc(buf_size * sizeof(*b));

        while (fgets(b, buf_size, fp) != NULL) {
                b[strcspn(b, "\n")] = '\0';
                utarray_push_back(vocabulary, &b);
        }

        free(b);
        utarray_sort(vocabulary, strcmp_wrap);
}

void load_vocabulary_from_file(FILE *fp, UT_array *vocabulary) {
        const int buf_size = 1024;
        char b[buf_size];
        int len = 0;
        char c;

        while (!feof(fp)) {
                if (strchr(" \n", c = fgetc(fp)) == NULL) {
                        b[len++] = tolower(c);
                } else {
                        b[len] = '\0';
                        utarray_push_back(vocabulary, &b);
                        len = 0;
                }
        }

        if (len > 0) {
                b[len] = '\0';
                utarray_push_back(vocabulary, &b);
        }
}

void remove_vocabulary_duplicates(UT_array *vocabulary) {
        struct {
                char *key;
                UT_hash_handle hh;
        } *tmp, *word, *words = NULL;

        char **p = NULL;

        while ((p = utarray_next(vocabulary, p))) {
                HASH_FIND_STR(words, *p, word);

                if (word == NULL) {
                        word = malloc(sizeof(*word));
                        word->key = strdup(*p);

                        HASH_ADD_KEYPTR(hh, words, word->key, strlen(word->key), word);
                }
        }

        utarray_clear(vocabulary);

        HASH_ITER(hh, words, word, tmp) {
                utarray_push_back(vocabulary, &word->key);
                HASH_DEL(words, word);
                free(word->key);
                free(word);
        }

        utarray_sort(vocabulary, strcmp_wrap);
}

bool valid_word(const char *w, const UT_array *vocabulary) {
        return utarray_find(vocabulary, &w, strcmp_wrap);
}

int fgettok(FILE *fp, const UT_array *vocabulary) {
        const int buf_size = 1024;
        char b[buf_size];
        int len = 0;
        char c;

        while (!feof(fp) && strchr(" \n", c = fgetc(fp)) == NULL)
                b[len++] = tolower(c);

        b[len] = '\0';

        return lookup_tok(b, vocabulary);
}

int lookup_tok(const char* w, const UT_array *vocabulary) {
        void *loc = utarray_find(vocabulary, &w, strcmp_wrap);

        if (loc != NULL) {
                return utarray_eltidx(vocabulary, loc);
        } else {
                return -1;
        }
}

char* lookup_word(const int t, const UT_array *vocabulary) {
        if (0 <= t && t < (int)utarray_len(vocabulary))
                return *(char**)utarray_eltptr(vocabulary, (unsigned)t);
        else
                return "";
}
