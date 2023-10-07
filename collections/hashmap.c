#include "hashmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define DEFAULT_CAPACITY 16

typedef struct entry {
    const void *key;
    void *value;
    struct entry *next;
} entry_t;

struct hashmap {
    size_t size;        // number of elements
    size_t capacity;    // number of buckets
    hash_func_t hash;   // hash function
    cmp_func_t cmp;     // comparison function
    entry_t **buckets;  // array of buckets
};

static size_t default_hash(const void *key)
{
    return (size_t)key;
}

static size_t (*get_hash(hash_func_t hash))(const void *)
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

static int (*get_cmp(cmp_func_t cmp))(const void *, const void *)
{
    return cmp ? cmp : default_cmp;
}

hashmap_t hashmap_new(
        size_t (*hash)(const void *),
        int (*cmp)(const void *, const void *)
        )
{
    hashmap_t map = malloc(sizeof(struct hashmap));
    map->size = 0;
    map->capacity = DEFAULT_CAPACITY;
    map->hash = get_hash(hash);
    map->cmp = get_cmp(cmp);
    map->buckets = calloc(DEFAULT_CAPACITY, sizeof(entry_t *));
    return map;
}

hashmap_t hashmap_default()
{
    return hashmap_new(NULL, NULL);
}

void hashmap_free(hashmap_t map)
{
    for (size_t i = 0; i < map->capacity; i++) {
        entry_t *entry = map->buckets[i];
        while (entry) {
            entry_t *next = entry->next;
            free(entry);
            entry = next;
        }
    }
    free(map->buckets);
    free(map);
}

size_t hashmap_size(hashmap_t map)
{
    return map->size;
}

// TODO: resize buckets
bool hashmap_put(hashmap_t map, const void *key, void *data)
{
    size_t index = map->hash(key) % map->capacity;
    entry_t *entry = map->buckets[index];

    while (entry) {
        if (!map->cmp(entry->key, key)) {
            entry->value = data;
            return true;
        }
        entry = entry->next;
    }

    entry = malloc(sizeof(entry_t));
    if (!entry) return false;

    entry->key = key;
    entry->value = data;
    entry->next = map->buckets[index];
    map->buckets[index] = entry;
    map->size++;

    return true;
}

void *hashmap_get(hashmap_t map, const void *key)
{
    size_t index = map->hash(key) % map->capacity;
    entry_t *entry = map->buckets[index];

    while (entry) {
        if (!map->cmp(entry->key, key)) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}
