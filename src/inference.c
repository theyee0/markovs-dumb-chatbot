#include "inference.h"

char* next_tok(UT_array *history, const UT_array *table, const UT_array *vocabulary) {
        const int prev = (utarray_len(history) > 0) ? *(int*)utarray_back(history) : rand() % (int)utarray_len(table);

        /* Previous token was not in vocabulary */
        if (prev < 0) {
                return NULL;
        }

        const UT_array *probabilities = *(UT_array**)utarray_eltptr(table, (unsigned)prev);
        const struct entry *last_item = utarray_back(probabilities);
        const int range = last_item->count;

        if (range <= 0) {
                return NULL;
        }

        const int selection = (range > 1) ? rand() % (range - 1) + 1 : 1;
        const int index = first_greater(probabilities, selection);

        utarray_push_back(history, &index);

        return lookup_word(index, vocabulary);
}
