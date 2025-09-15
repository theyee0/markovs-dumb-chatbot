#include "training.h"
#include <assert.h>

static int cmp_tabkey(const void *_a, const void *_b) {
        const struct entry *a = _a;
        const struct entry *b = _b;

        return a->key - b->key;
}

static void utarray_copy(void *_dst, const void *_src) {
        const UT_array *src = *(UT_array**)_src;
        UT_array **dst = _dst;
        /* TODO: Investigate cause of memory leak in original code
        int n = src->n * src->icd.sz;
        *dst = malloc(sizeof(UT_array));
        **dst = *src;
        (*dst)->d = malloc(n);
        memcpy((*dst)->d, src->d, n);
        */

        void *obj = NULL;

        utarray_new(*dst, &src->icd);

        while ((obj = utarray_next(src, obj))) {
                utarray_push_back(*dst, obj);
        }
}

static void utarray_dtor(void *_elt) {
        UT_array **elt = _elt;

        utarray_free(*elt);
}

void load_vocabulary_from_model(FILE *model, UT_array *vocabulary) {
        const int buf_size = 1024;
        char c;
        int len;
        char *b = malloc(buf_size * sizeof(*b));

        while (!feof(model)) {
                len = 0;
                while (!feof(model) && strchr(" \n", c = fgetc(model)) == NULL) {
                        b[len++] = tolower(c);
                }

                b[len] = '\0';

                utarray_push_back(vocabulary, &b);

                if (c == '\n') {
                        break;
                }
        }

        free(b);
}

void load_model(UT_array **table, UT_array *vocabulary, FILE *model) {
        const int max_buffer = 8192;
        int parent, child;
        UT_array **row = NULL;
        char *line = malloc(max_buffer * sizeof(*line));
        char *tok = NULL;
        struct entry e;

        freopen(NULL, "r", model);

        load_vocabulary_from_model(model, vocabulary);
        table_init(table, vocabulary);

        while ((row = utarray_next(*table, row)) && !feof(model)) {
                fgets(line, max_buffer, model);

                tok = strtok(line, " ");

                while (tok != NULL) {
                        parent = atoi(tok);
                        tok = strtok(NULL, " ");
                        if (tok == NULL) {
                                break;
                        }

                        child = atoi(tok);

                        e.key = parent;
                        e.count = child;
                        utarray_push_back(*row, &e);
                        tok = strtok(NULL, " ");
                }
        }

        free(line);
}

void rebase_model(UT_array *table, UT_array *vocabulary, FILE *model) {
        const int buf_size = 1024;
        char *line = malloc(buf_size * sizeof(*line));
        UT_array *model_vocab = NULL;
        int parent, child;
        char *tok = NULL;

        utarray_new(model_vocab, &ut_str_icd);

        load_vocabulary_from_model(model, model_vocab);

        parent = 0;

        while ((fgets(line, buf_size, model))) {
                tok = strtok(line, " ");

                /* TODO: Add error correction and detection */
                while (tok != NULL) {
                        child = lookup_tok(*(char**)utarray_eltptr(model_vocab, (unsigned)atoi(tok)), vocabulary);
                        tok = strtok(NULL, " ");

                        if (tok == NULL) {
                                break;
                        }

                        table_push(table, parent, child, atoi(tok));
                        tok = strtok(NULL, " ");
                }

                parent++;
        }

        free(line);
        utarray_free(model_vocab);
}

void write_model(UT_array *table, UT_array *vocabulary, FILE *model) {
        UT_array **row = NULL;
        char **word = NULL;

        struct entry *e = NULL;

        while ((word = utarray_next(vocabulary, word))) {
                fprintf(model, "%s ", *word);
        }

        while ((row = utarray_next(table, row))) {
                fprintf(model, "\n");

                while ((e = utarray_next(*row, e))) {
                        assert(e->key >= 0 && e->count > 0);
                        fprintf(model, "%d %d ", e->key, e->count);
                }
        }
}

void table_init(UT_array **table, UT_array *vocabulary) {
        int i;
        UT_icd table_icd = {sizeof(UT_array*), NULL, utarray_copy, utarray_dtor};
        UT_icd entry_icd = {sizeof(struct entry), NULL, NULL, NULL};
        int vocabulary_size;
        UT_array *row = NULL;

        remove_vocabulary_duplicates(vocabulary);
        vocabulary_size = utarray_len(vocabulary);

        utarray_new(*table, &table_icd);
        utarray_reserve(*table, vocabulary_size);

        for (i = 0; i < vocabulary_size; i++) {
                row = NULL;
                utarray_new(row, &entry_icd);
                utarray_push_back(*table, &row);
                utarray_free(row);
        }
}

void table_depfx(UT_array *table) {
        UT_array **arr = NULL;
        struct entry *e;
        int prev;
        int tmp;

        while ((arr = utarray_next(table, arr))) {
                e = NULL;
                prev = 0;
                while ((e = utarray_next(*arr, e))) {
                        tmp = e->count;
                        e->count -= prev;
                        prev = tmp;
                }
        }
}

void table_pfx(UT_array *table) {
        UT_array **arr = NULL;
        struct entry *e;
        int prev;
        while ((arr = utarray_next(table, arr))) {
                e = NULL;
                prev = 0;

                while ((e = utarray_next(*arr, e))) {
                        prev = (e->count += prev);
                }
        }
}

/*
static int cmp_tabcnt(const void *_a, const void *_b) {
        const struct entry *a = _a;
        const struct entry *b = _b;

        return a->count - b->count;
}
*/

void table_inc(UT_array *table, const int parent, const int child) {
        table_push(table, parent, child, 1);
}

void table_push(UT_array *table, const int parent, const int child, const int val) {
        UT_array *row = *(UT_array**)utarray_eltptr(table, (unsigned)parent);
        struct entry e;
        struct entry *p = NULL;

        assert(child >= 0 && val > 0);

        e.key = child;
        e.count = val;

        p = utarray_find(row, &e, cmp_tabkey);

        if (p == NULL) {
                utarray_push_back(row, &e);
                utarray_sort(row, cmp_tabkey);
        } else {
                p->count += val;
        }
}

void read_data(UT_array *table, FILE* input, const UT_array *vocabulary) {
        int prev = -1;
        int cur;

        rewind(input);

        while (!feof(input)) {
                cur = fgettok(input, vocabulary);

                if (cur != -1 && prev != -1) {
                        table_inc(table, prev, cur);
                }

                prev = cur;
        }
}
