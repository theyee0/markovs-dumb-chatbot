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
        int n = src->n * src->icd.sz;

        *dst = malloc(sizeof(**dst));

        **dst = *src;

        (*dst)->d = malloc(n);
        if ((*dst)->d == NULL) {
                utarray_oom();
        }

        memcpy((*dst)->d, src->d, n);
}

static void utarray_dtor(void *_elt) {
        UT_array **elt = _elt;

        utarray_free(*elt);
}

void table_init(UT_array **table, UT_array *vocabulary) {
        int i;
        UT_icd table_icd = {sizeof(UT_array*), NULL, utarray_copy, utarray_dtor};
        UT_icd entry_icd = {sizeof(struct entry), NULL, NULL, NULL};
        const int vocabulary_size = utarray_len(vocabulary);
        UT_array *row = NULL;

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

void table_push(UT_array *table, const int parent, const int child) {
        UT_array *row = *(UT_array**)utarray_eltptr(table, (unsigned)parent);
        struct entry e = {.key = child, .count = 1};
        struct entry *p = NULL;

        p = utarray_find(row, &e, cmp_tabkey);

        if (p == NULL) {
                utarray_push_back(row, &e);
                utarray_sort(row, cmp_tabkey);
        } else {
                p->count++;
        }
}

void read_data(UT_array *table, FILE* input, const UT_array *vocabulary) {
        int prev = -1;
        int cur;

        while (!feof(input)) {
                cur = fgettok(input, vocabulary);

                if (cur != -1 && prev != -1) {
                        table_push(table, prev, cur);
                }

                prev = cur;
        }
}
