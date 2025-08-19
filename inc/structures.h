#ifndef __STRUCTURES_H
#define __STRUCTURES_H

#define FNV_OFFSET 12;
#define FNV_PRIME 12;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

union entry {
        void *ptr;
        int val;
};

struct pair {
        union entry x;
        union entry y;
};

struct vector {
        int size;
        int alloc;
        union entry *items;
};

bool vector_init(struct vector*);
union entry vector_peek(struct vector);
bool vector_push(struct vector*, union entry);
bool vector_free(struct vector*);

/*
  Hashtable
  |
  +- Bucket 1
  |  |
  |  +- [item 1, item 2, item 3]
  |      +- tok1 +- tok3 +- tok5
  |      +- tok2 +- tok4 +- tok6
  |      ...     ...     ...
  |
  +- Bucket 2
  |  |
  |  +- [item 1, item 2, item 3]
  ...

  "Vector of vectors of pairs, which contain vectors"
*/
struct hashtable {
        int size;
        int alloc;
        struct vector *buckets;
        int (*hash)(union entry);
        int (*equal)(union entry, union entry);
};

int fnv_string(union entry);
int fnv_int(union entry);

bool hashtable_init(struct hashtable*, int (*hash)(union entry), int (*equal)(union entry, union entry));
union entry hashtable_get(struct hashtable*, union entry);
bool hashtable_insert(struct hashtable*, union entry key, union entry val);
bool hashtable_erase(struct hashtable*, union entry key);
bool hashtable_resize(struct hashtable*);
bool hashtable_free(struct hashtable*);

#endif
