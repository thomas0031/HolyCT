#include "hashmap.h"

#include <stdlib.h>
#include <stdint.h>

#define DEFAULT_CAPACITY 16

typedef struct entry {
    const void *key;
    void *value;
    struct entry *next;
} entry_t;

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
