#include "utils.h"

static const char *reject = "\a\b\f\n\r\t\v ";

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

bool valid_word(const char *w, const UT_array *vocabulary) {
        return utarray_find(vocabulary, &w, strcmp_wrap);
}


char *fgetword(char *s, int size, FILE *stream) {
	int i = 0;
	char c = '\0';

	while (!feof(stream) && i + 1 < size) {
		c = fgetc(stream);

		s[i++] = c;

		if (strchr(reject, c) != NULL) {
			break;
		}
	}

	s[i] = '\0';

	if (i == 0 || s[0] == EOF) {
		return NULL;
	} else {
		return s;
	}
}

int fgettok(FILE *fp, const UT_array *vocabulary) {
        const int buf_size = 1024;
        char *b = malloc(buf_size * sizeof(*b));

        int token;

	fgetword(b, buf_size, fp);

        b[strcspn(b, reject)] = '\0';

        token = lookup_tok(b, vocabulary);

        free(b);

        return token;
}

int lookup_tok(const char* w, const UT_array *vocabulary) {
        /* Search for a word's index in the vocabulary */
        void *loc = utarray_find(vocabulary, &w, strcmp_wrap);

        if (loc != NULL) {
                return utarray_eltidx(vocabulary, loc);
        } else {
                return -1;
        }
}

char* lookup_word(const int t, const UT_array *vocabulary) {
        /* Return the word corresponding to an index in the vocabulary */
        if (0 <= t && t < (int)utarray_len(vocabulary))
                return *(char**)utarray_eltptr(vocabulary, (unsigned)t);
        else
                return "";
}
