#include "structures.h"

bool vector_init(struct vector *v) {
        v->size = 0;
        v->alloc = 1;
        v->items = malloc(sizeof(*v->items));

        return true;
}

int vector_peek(struct vector v) {
        return v.items[v.size - 1];
}

bool vector_push(struct vector *v, int item) {
        if (v->size >= v->alloc) {
                v->alloc *= 2;
                v->items = realloc(v->items, v->alloc * sizeof(*(v->items)));
        }

        v->items[v->size++] = item;
        return true;
}

bool vector_free(struct vector *v) {
        free(v->items);
        v->items = NULL;
        v->size = 0;
        v->alloc = 0;
        return true;
}

bool table_free(struct table* table) {
        int i;

        for (i = 0; i < table->size; i++) {
                free(table->items[i]);
        }

        free(table->items);
        table->size = 0;
        return true;
}
