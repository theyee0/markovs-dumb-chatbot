#include "training.h"
#include <assert.h>

static const char *reject = "\a\b\f\n\r\t\v ";
static const int buf_size = 1024;

/* Sort entries in ascending order of key */
static int cmp_tabkey(const void *_a, const void *_b) {
        const struct entry *a = _a;
        const struct entry *b = _b;

        return a->key - b->key;
}

/* Perform deep copy of utarray of utarrays */
static void utarray_copy(void *_dst, const void *_src) {
        const UT_array *src = *(UT_array**)_src;
        UT_array **dst = _dst;

        /*
          TODO: Investigate cause of memory leak in original code
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

/* Wrapper to free utarray */
static void utarray_dtor(void *_elt) {
        UT_array **elt = _elt;

        utarray_free(*elt);
}

void load_vocabulary_file(FILE *fp, UT_array *vocabulary) {
        char *b = malloc(buf_size * sizeof(*b));

        freopen(NULL, "r", fp);

        while (fgetword(b, buf_size, fp)) {
                /* Keep parsing words and adding them to vocab until EOF */
		b[strcspn(b, reject)] = '\0';
                utarray_push_back(vocabulary, &b);
        }

        free(b);
}

void load_vocabulary_from_model(FILE *model, UT_array *vocabulary) {
        char *b = malloc(buf_size * sizeof(*b));
	int i = 0;

	char c;

        freopen(NULL, "r", model);

        while (!feof(model) && (c = fgetc(model)) != '\n') {
		if (i + 1 == buf_size || strchr(reject, c) != NULL) {
			b[i] = '\0';
                	/* Once word has been found, add to array */
                	utarray_push_back(vocabulary, &b);
			i = 0;
		} else {
			b[i++] = c;
		}
        }

        free(b);
}

void remove_vocabulary_duplicates(UT_array *vocabulary) {
        struct {
                char *key;
                UT_hash_handle hh;
        } *tmp, *word, *words = NULL;

        char **p = NULL;

        /* Iterate over vocab list and add items to hash set if unique */
        while ((p = utarray_next(vocabulary, p))) {
                HASH_FIND_STR(words, *p, word);

                if (word == NULL) {
                        word = malloc(sizeof(*word));

                        /* Duplicate string */
                        word->key = malloc((strlen(*p) + 1) * sizeof(*word->key));
                        strcpy(word->key, *p);

                        HASH_ADD_KEYPTR(hh, words, word->key, strlen(word->key), word);
                }
        }

        utarray_clear(vocabulary);

        /* Write unique words from set back to vocabulary array */
        HASH_ITER(hh, words, word, tmp) {
                utarray_push_back(vocabulary, &word->key);
                HASH_DEL(words, word);
                free(word->key);
                free(word);
        }

        utarray_sort(vocabulary, strcmp_wrap);
}

void load_model(UT_array **table, UT_array *vocabulary, FILE *model) {
        char *b = malloc(buf_size * sizeof(*b));

        UT_array **row = NULL;

        struct entry e;
        int key, count;

	int eol;

        char *endptr = NULL;
	char *tok;

        freopen(NULL, "r", model);

        load_vocabulary_from_model(model, vocabulary);
        table_init(table, vocabulary);

        /* Iterate over table to fill rows with model data */
        while ((row = utarray_next(*table, row))) {
		while (fgetword(b, buf_size, model)) {
			if (b[0] == '\n') {
				break;
			} else if (strchr(reject, b[0])) {
				continue;
			}

			if (strchr(b, ',') == NULL) {
				fprintf(stderr, "WARNING: Malformed model data; unmatched key/count pair\n");
				break;
			}

			eol = (strchr(b, '\n') != NULL);

			/* Iteratively parse all key/count pairs and add to table */
			tok = strtok(b, ",");
			assert(tok != NULL);
			key = strtol(tok, &endptr, 10);
			if (*endptr != '\0' && *endptr != ' ' && *endptr != '\n') {
				fprintf(stderr, "WARNING: Non-numerical model data\n");
			}

			tok = strtok(NULL, ",");
			assert(tok != NULL);
			count = strtol(tok, &endptr, 10);
			if (*endptr != '\0' && *endptr != ' ' && *endptr != '\n') {
				fprintf(stderr, "WARNING: Non-numerical model data\n");
			}

			e.key = key;
			e.count = count;

			utarray_push_back(*row, &e);

			if (eol) {
				break;
			}
		}
        }

	write_model(*table, vocabulary, fopen("out.txt", "w"));

        free(b);
}

void rebase_model(UT_array *table, UT_array *vocabulary, FILE *model) {
        char *b = malloc(buf_size * sizeof(*b));

        UT_array *model_vocab = NULL;

        char *endptr = NULL;

	int eol;

	char *tok;

        int key, count, parent;

        utarray_new(model_vocab, &ut_str_icd);

        freopen(NULL, "r", model);
        load_vocabulary_from_model(model, model_vocab);
        remove_vocabulary_duplicates(vocabulary);

        parent = 0;

        for (parent = 0; (unsigned)parent < utarray_len(vocabulary); parent++) {
		while (fgetword(b, buf_size, model)) {
			if (b[0] == '\n') {
				break;
			} else if (strchr(reject, b[0])) {
				continue;
			}

			if (strchr(b, ',') == NULL) {
				fprintf(stderr, "WARNING: %s | Malformed model data; unmatched key/count pair\n", b);
				break;
			}

			eol = (strchr(b, '\n') != NULL);

			/* Iteratively parse all key/count pairs and add to table */
			tok = strtok(b, ",");
			assert(tok != NULL);
			key = strtol(tok, &endptr, 10);
			if (*endptr != '\0' && *endptr != ' ' && *endptr != '\n') {
				fprintf(stderr, "WARNING: Non-numerical model data\n");
			}

			key = lookup_tok(*(char**)utarray_eltptr(model_vocab, (unsigned)key), vocabulary);

			tok = strtok(NULL, ",");
			assert(tok != NULL);
			count = strtol(tok, &endptr, 10);
			if (*endptr != '\0' && *endptr != ' ' && *endptr != '\n') {
				fprintf(stderr, "WARNING: Non-numerical model data\n");
			}

			table_push(table, parent, key, count);

			if (eol) {
				break;
			}
		}
        }

        free(b);
        utarray_free(model_vocab);
}

void write_model(UT_array *table, UT_array *vocabulary, FILE *model) {
        UT_array **row = NULL;
        char **word = NULL;

        struct entry *e = NULL;

        freopen(NULL, "w", model);

        /* Write all vocabulary words to first line of file */
        while ((word = utarray_next(vocabulary, word))) {
                fprintf(model, "%s ", *word);
        }

        /* Iterate over table and write space-separated key/count pairs */
        while ((row = utarray_next(table, row))) {
                fprintf(model, "\n");

                while ((e = utarray_next(*row, e))) {
                        assert(e->key >= 0 && e->count > 0);
                        fprintf(model, "%d,%d ", e->key, e->count);
                }
        }
}

void table_init(UT_array **table, UT_array *vocabulary) {
        int i;
        UT_icd table_icd = {sizeof(UT_array*), NULL, utarray_copy, utarray_dtor};
        UT_icd entry_icd = {sizeof(struct entry), NULL, NULL, NULL};
        int vocabulary_size;
        UT_array *row = NULL;

        /* Clean up vocabulary */
        remove_vocabulary_duplicates(vocabulary);
        vocabulary_size = utarray_len(vocabulary);

        /* Prepare memory */
        utarray_new(*table, &table_icd);

        /* Iterate over table and add new (empty) rows */
        for (i = 0; i < vocabulary_size; i++) {
                row = NULL;
                utarray_new(row, &entry_icd);
                utarray_push_back(*table, &row);
                utarray_free(row);
        }
}

void table_pfx(UT_array *table) {
        UT_array **arr = NULL;
        struct entry *e;
        int prev;

        /* Iterate over table and convert row counts into prefix array */
        while ((arr = utarray_next(table, arr))) {
                e = NULL;
                prev = 0;

                while ((e = utarray_next(*arr, e))) {
                        prev = (e->count += prev);
                }
        }
}

void table_depfx(UT_array *table) {
        UT_array **arr = NULL;
        struct entry *e;
        int prev;
        int tmp;

        /* Iterate over table and convert each row's prefix array back to counts */
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

void table_push(UT_array *table, const int parent, const int key, const int count) {
        UT_array *row = *(UT_array**)utarray_eltptr(table, (unsigned)parent);
        struct entry e;
        struct entry *p = NULL;

        e.key = key;
        e.count = count;

        /* Check if the given key already exists */
        p = utarray_find(row, &e, cmp_tabkey);

        /* If key not found, add key; otherwise, just increment it */
        if (p == NULL) {
                utarray_push_back(row, &e);
                utarray_sort(row, cmp_tabkey);
        } else {
                p->count += count;
        }
}

void table_inc(UT_array *table, const int parent, const int key) {
        /* Adds one to a given item in the table */
        table_push(table, parent, key, 1);
}

void read_data(UT_array *table, FILE* input, const UT_array *vocabulary) {
        int prev = -1;
        int cur;

        rewind(input);

        /* Iterate over successive words and increment the table */
        while (!feof(input)) {
                cur = fgettok(input, vocabulary);

                if (cur != -1 && prev != -1) {
                        table_inc(table, prev, cur);
                }

                prev = cur;
        }
}
