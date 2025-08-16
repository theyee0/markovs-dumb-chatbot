#include "training.h"

bool table_depfx(struct table* table) {
        int i, j;
        for (i = 0; i < table->size; i++) {
                for (j = 1; j < table->size; j++) {
                        table->items[i][j] -= table->items[i][j - 1];
                }
        }

        return true;
}

bool table_pfx(struct table* table) {
        int i, j;
        for (i = 0; i < table->size; i++) {
                for (j = 1; j < table->size; j++) {
                        table->items[i][j] += table->items[i][j - 1];
                }
        }

        return true;
}

bool table_push(struct table* table, const int parent, const int child) {
        table->items[parent][child]++;

        return true;
}

bool read_data(struct table* table, FILE* input) {
        int prev = -1;
        int cur;

        while (!feof(input)) {
                cur = fgettok(input);

                if (cur != -1 && prev != -1) {
                        table_push(table, prev, cur);
                }

                prev = cur;
        }

        return true;
}
