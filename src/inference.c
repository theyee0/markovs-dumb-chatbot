#include "inference.h"

char* next_tok(UT_array *history, const UT_array *table, const UT_array *vocabulary) {
        int prev;
        UT_array *prob;
        struct entry *last_item, *selected_item;
        int range, selection, index;

	if (utarray_len(table) == 0) {
		return NULL;
	}

	prev = (utarray_len(history) > 0) ? *(int*)utarray_back(history) : rand() % (int)utarray_len(table);

        /* Previous token was not in vocabulary */
        if (prev < 0) {
                return NULL;
        }

        /* Get probability array (prefix array) and get its range */
        prob = *(UT_array**)utarray_eltptr(table, (unsigned)prev);
        last_item = utarray_back(prob);

        if (last_item == NULL || last_item->count == 0) {
                return NULL;
        }

        range = last_item->count;

        /* Pick a random item from the probability array and find the key */
        selection = (range > 1) ? rand() % (range - 1) + 1 : 1;
        index = first_greater(prob, selection);
        selected_item = utarray_eltptr(prob, (unsigned)index);

        /* Add to history */
        utarray_push_back(history, &selected_item->key);

        return lookup_word(selected_item->key, vocabulary);
}
