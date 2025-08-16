#include "inference.h"

char* next_tok(struct vector* history, const struct table table) {
        const int prev = vector_peek(*history);
        const int range = table.items[prev][table.size - 1];
        const int selection = rand() % range;
        const int index =
                (int**)bsearch(&selection,
                               table.items[prev],
                               table.size,
                               sizeof(selection),
                               intcmp) -
                table.items;

        vector_push(history, index);

        return lookup_word(index);
}
