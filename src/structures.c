#include "structures.h"

bool vector_init(struct vector *v) {
        v->size = 0;
        v->alloc = 1;
        v->items = malloc(sizeof(*v->items));

        return true;
}

union entry vector_peek(struct vector v) {
        return v.items[v.size - 1];
}

bool vector_push(struct vector *v, union entry item) {
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

int fnv_string(union entry entry) {
        int hash = FNV_OFFSET;
        char *str = entry.ptr;

        while (*str != '\0') {
                hash ^= *str;
                hash *= FNV_PRIME;
                str++;
        }

        return hash;
}

int fnv_int(union entry entry) {
        int hash = FNV_OFFSET;
        int val = entry.val;
        size_t i;

        for (i = 0; i < sizeof(val); i++) {
                hash ^= (val & 0xFF);
                hash *= FNV_PRIME;
                val >>= 8;
        }

        return hash;
}

bool hashtable_init(struct hashtable* table,
                    int (*hash)(union entry),
                    int (*equal)(union entry, union entry)) {
        table->size = 0;
        table->alloc = 8;

        table->buckets = malloc(8 * sizeof(*table->buckets));

        table->hash = hash;
        table->equal = equal;

        return true;
}

union entry hashtable_get(struct hashtable* table, union entry key) {
        struct vector bucket = table->buckets[table->hash(key) % table->size];
        struct pair *hashentry;
        int i;

        for (i = 0; i < bucket.size; i++) {
                hashentry = bucket.items[i].ptr;

                if (table->equal(key, hashentry->x)) {
                        return hashentry->y;
                }
        }

        union entry ret;
        ret.val = -1;

        return ret;
}

bool hashtable_insert(struct hashtable *table,
                      union entry key,
                      union entry value) {
        struct vector *bucket = &table->buckets[table->hash(key) % table->size];
        struct pair *hashentry;
        union entry *obj = NULL;
        int i;

        for (i = 0; i < bucket->size; i++) {
                hashentry = bucket->items[i].ptr;

                if (table->equal(key, hashentry->x)) {
                        obj = &hashentry->y;
                }
        }

        if (obj != NULL) {
                (*obj) = value;
        } else {
                //                vector_push(bucket, {.x = key, .y = value});
        }

        hashtable_resize(table);

        return true;
}

bool hashtable_erase(struct hashtable *table, union entry key) {
        struct vector *bucket = &table->buckets[table->hash(key) % table->size];
        struct pair *hashentry;
        int i;

        for (i = 0; i < bucket->size; i++) {
                hashentry = bucket->items[i].ptr;

                if (table->equal(key, hashentry->x)) {
                        bucket->items[i] = bucket->items[bucket->size - 1];
                        bucket->size--;
                        break;
                }
        }

        hashtable_resize(table);

        return true;
}

bool hashtable_resize(struct hashtable *table) {
        const float load_minimum = 0.3;
        const float load_maximum = 0.7;
        float load = (float)table->size / (float)table->alloc;
        int i, j;
        int hash_index;
        int old_size = table->alloc;

        struct vector *old_buckets;

        if (load_minimum <= load && load <= load_maximum) {
                return true;
        } else if (load < load_minimum) {
                table->alloc /= 2;
        } else if (load > load_maximum) {
                table->alloc *= 2;
        }

        old_buckets = table->buckets;
        table->buckets = malloc(table->alloc * sizeof(*table->buckets));

        for (i = 0; i < table->alloc; i++) {
                vector_init(&(table->buckets[i]));
        }

        for (i = 0; i < old_size; i++) {
                for (j = 0; j < old_buckets[i].size; j++) {
                        hash_index = table->hash(old_buckets[i].items[j].ptr->x) % table->alloc;
                        vector_push(&table->buckets[hash_index], old_buckets[i].items[j]);
                }

                vector_free(&old_buckets[i]);
        }

        free(old_buckets);

        return true;
}

bool hashtable_free(struct hashtable* table) {
        int i;

        for (i = 0; i < table->size; i++) {
                vector_free(&table->buckets[i]);
        }

        free(table->buckets);
        table->size = 0;
        return true;
}
