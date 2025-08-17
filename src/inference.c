#include "inference.h"

char* next_tok(struct vector* history, const struct table table) {
        const int prev = (history->size > 0) ? vector_peek(*history) : rand() % vocabulary_size;
        const int range = table.items[prev][table.size - 1];

        if (range <= 0) {
                return NULL;
        }

        const int selection = rand() % (range - 1) + 1;
        const int index = first_greater(table.items[prev], table.size, selection);

        vector_push(history, index);

        return lookup_word(index);
}
