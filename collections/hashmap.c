#include "hashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DEFAULT_CAPACITY 16

typedef struct entry {
    const void *key;
    void *value;
    struct entry *next;
} entry_t;

typedef struct hashmap_iterator_private {
    hashmap_t hashmap;
    size_t current_bucket;
    hashmap_entry_t current_entry;
} hashmap_iterator_private;

typedef struct hashmap_private {
    size_t size;            // number of elements
    size_t capacity;        // number of buckets
    hash_func_t hash;       // hash function
    cmp_func_t cmp;         // comparison function
    print_func_t print;     // print function
    entry_t **buckets;      // array of buckets
} hashmap_private;

static size_t default_hash(const void *key)
{
    return (size_t)key;
}

static size_t (*get_hash_or_default(hash_func_t hash))(const void *)
{
    return hash ? hash : default_hash;
}

static int default_cmp(const void *a, const void *b)
{
    intptr_t ptr_a = (intptr_t)a;
    intptr_t ptr_b = (intptr_t)b;

    if (ptr_a < ptr_b) return -1;
    if (ptr_a > ptr_b) return 1;
    return 0;
}

static int (*get_cmp_or_default(cmp_func_t cmp))(const void *, const void *)
{
    return cmp ? cmp : default_cmp;
}

static void default_print(const void *key, const void *value)
{
    printf("%p -> %p\n", key, value);
}

static void (*get_print_or_default(print_func_t print))(const void *, const void *)
{
    return print ? print : default_print;
}

size_t size(hashmap_t self)
{
    hashmap_private *private = (hashmap_private *)(self + 1);

    return private->size;
}

// TODO: resize buckets
bool put(hashmap_t self, const void *key, void *data)
{
    hashmap_private *private = (hashmap_private *)(self + 1);

    size_t index = private->hash(key) % private->capacity;
    entry_t *entry = private->buckets[index];

    while (entry) {
        if (!private->cmp(entry->key, key)) {
            entry->value = data;
            return true;
        }
        entry = entry->next;
    }

    entry = malloc(sizeof(entry_t));
    if (!entry) return false;

    entry->key = key;
    entry->value = data;
    entry->next = private->buckets[index];
    private->buckets[index] = entry;
    private->size++;

    return true;
}

void *get(hashmap_t self, const void *key)
{
    hashmap_private *private = (hashmap_private *)(self + 1);

    size_t index = private->hash(key) % private->capacity;
    entry_t *entry = private->buckets[index];

    while (entry) {
        if (!private->cmp(entry->key, key)) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

void print(hashmap_t self)
{
    hashmap_private *private = (hashmap_private *)(self + 1);

    for (size_t i = 0; i < private->capacity; i++) {
        entry_t *entry = private->buckets[i];
        while (entry) {
            private->print(entry->key, entry->value);
            entry = entry->next;
        }
    }
}

bool iter_has_next(hashmap_iterator_t self)
{
    hashmap_iterator_private *self_private = (hashmap_iterator_private *)(self + 1);

    // If there's a next item in the current chain, return true.
    entry_t * current_entry = (entry_t *)self_private->current_entry;
    if (current_entry && current_entry->next) {
        return true;
    }

    // Otherwise, check subsequent buckets.
    hashmap_private *private = (hashmap_private *)(self_private->hashmap + 1);
    for (size_t i = self_private->current_bucket + 1; i < private->capacity; i++) {
        if (private->buckets[i]) {
            return true;
        }
    }

    return false;
}

hashmap_entry_t iter_next(hashmap_iterator_t self)
{
    hashmap_iterator_private *iter = (hashmap_iterator_private *)(self + 1);

    entry_t * current_entry = (entry_t *)iter->current_entry;
    // If there's a next item in the current chain, move to it.
    if (current_entry && current_entry->next) {
        iter->current_entry = current_entry->next;
        return iter->current_entry;
    }

    // Otherwise, move to the next non-empty bucket.
    hashmap_private *private = (hashmap_private *)(iter->hashmap + 1);
    for (size_t i = iter->current_bucket + 1; i < private->capacity; i++) {
        if (private->buckets[i]) {
            iter->current_bucket = i;
            iter->current_entry = private->buckets[i];
            return iter->current_entry;
        }
    }

    // If no next item, return null or handle appropriately.
    return NULL;
}

hashmap_iterator_t iter(hashmap_t self)
{
    hashmap_iterator_t iter = malloc(sizeof(hashmap_iterator) + sizeof(hashmap_iterator_private));
    if (!iter) return NULL;

    iter->has_next = iter_has_next;
    iter->next = iter_next;

    hashmap_iterator_private *private = (hashmap_iterator_private *)(iter + 1);
    hashmap_private *map_private = (hashmap_private *)(self + 1);
    private->current_entry = map_private->buckets[0];
    private->hashmap = self;
    private->current_bucket = -1;

    return iter;
}

hashmap_t hashmap_new(
        size_t (*hash_f)(const void *),
        int (*cmp_f)(const void *, const void *),
        void (*print_f)(const void *, const void *)
        )
{
    hashmap_t map = malloc(sizeof(hashmap) + sizeof(hashmap_private));
    if (!map) return NULL;
    map->size = size;
    map->put = put;
    map->get = get;
    map->print = print;
    map->iter = iter;

    hashmap_private *private = (hashmap_private *)(map + 1);
    private->size = 0;
    private->capacity = DEFAULT_CAPACITY;
    private->hash = get_hash_or_default(hash_f);
    private->cmp = get_cmp_or_default(cmp_f);
    private->print = get_print_or_default(print_f);
    private->buckets = calloc(DEFAULT_CAPACITY, sizeof(entry_t *));

    return map;
}

hashmap_t hashmap_default()
{
    return hashmap_new(NULL, NULL, NULL);
}

void hashmap_free(hashmap_t map)
{
    hashmap_private *private = (hashmap_private *)(map + 1);

    for (size_t i = 0; i < private->capacity; i++) {
        entry_t *entry = private->buckets[i];
        while (entry) {
            entry_t *next = entry->next;
            free(entry);
            entry = next;
        }
    }

    free(private->buckets);
    free(map);
}
