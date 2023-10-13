#include "HashMap.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define DEFAULT_CAPACITY 16

typedef struct MapEntry_private MapEntry_private;
typedef struct MapIterator_private MapIterator_private;
typedef struct HashMap_private HashMap_private;

struct MapEntry_private {
    const void *key;
    void *value;
    MapEntry_private *next;
};

struct MapIterator_private {
    HashMap* hashmap;
    size_t current_bucket;
    MapEntry_private* current_entry;
};

struct HashMap_private {
    size_t size;                // number of elements
    size_t capacity;            // number of buckets
    hash_func_t hash;           // hash function
    cmp_func_t cmp;             // comparison function
    MapEntry_private **buckets; // array of buckets
};

static size_t default_hash(const void *key)
{
    return (size_t)key;
}

static int default_cmp(const void *a, const void *b)
{
    intptr_t ptr_a = (intptr_t)a;
    intptr_t ptr_b = (intptr_t)b;

    if (ptr_a < ptr_b) return -1;
    if (ptr_a > ptr_b) return 1;
    return 0;
}

// TODO remove when impl optional
static const void * get_or_default(const void *optional, const void *default_value)
{
    return optional ? optional : default_value;
}

size_t  hashmap_size(const HashMap *self)
{
    HashMap_private *private = (HashMap_private *)(self + 1);

    return private->size;
}

// TODO: resize buckets
bool hashmap_put(HashMap *self, const void *key, void *data)
{
    HashMap_private *private = (HashMap_private *)(self + 1);

    size_t index = private->hash(key) % private->capacity;
    MapEntry_private *entry = (MapEntry_private*)private->buckets[index];

    while (entry) {
        if (!private->cmp(entry->key, key)) {
            entry->value = data;
            return true;
        }
        entry = entry->next;
    }

    entry = malloc(sizeof(MapEntry_private));
    if (!entry) return false;

    entry->key = key;
    entry->value = data;
    entry->next = private->buckets[index];
    private->buckets[index] = entry;
    private->size++;

    return true;
}

void *hashmap_get(const HashMap *self, const void *key)
{
    HashMap_private *private = (HashMap_private *)(self + 1);

    size_t index = private->hash(key) % private->capacity;
    MapEntry_private *entry = private->buckets[index];

    while (entry) {
        if (!private->cmp(entry->key, key)) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

bool hashmap_iter_has_next(const MapIterator *self)
{
    MapIterator_private *self_private = (MapIterator_private *)(self + 1);

    // If there's a next item in the current chain, return true.
    MapEntry_private *current_entry = self_private->current_entry;
    if (current_entry && current_entry->next) {
        return true;
    }

    // Otherwise, check subsequent buckets.
    HashMap_private *private = (HashMap_private*)(self_private->hashmap + 1);
    for (size_t i = self_private->current_bucket + 1; i < private->capacity; i++) {
        if (private->buckets[i]) {
            return true;
        }
    }

    return false;
}

MapEntry *hashmap_iter_next(MapIterator *self)
{
    MapIterator_private *iter = (MapIterator_private *)(self + 1);

    MapEntry_private *current_entry = (MapEntry_private*)iter->current_entry;
    // If there's a next item in the current chain, move to it.
    if (current_entry && current_entry->next) {
        iter->current_entry = current_entry->next;
        return (MapEntry*)iter->current_entry;
    }

    // Otherwise, move to the next non-empty bucket.
    HashMap_private *private = (HashMap_private *)(iter->hashmap + 1);
    for (size_t i = iter->current_bucket + 1; i < private->capacity; i++) {
        if (private->buckets[i]) {
            iter->current_bucket = i;
            iter->current_entry = private->buckets[i];
            return (MapEntry*)iter->current_entry;
        }
    }

    // If no next item, return null or handle appropriately.
    return NULL;
}

MapIterator *hashmap_iter(HashMap *self)
{
    MapIterator *iter = malloc(sizeof(MapIterator) + sizeof(MapIterator_private));
    if (!iter) return NULL;

    iter->has_next = hashmap_iter_has_next;
    iter->next = hashmap_iter_next;

    MapIterator_private *private = (MapIterator_private *)(iter + 1);
    HashMap_private *map_private = (HashMap_private *)(self + 1);
    private->current_entry = map_private->buckets[0];
    private->hashmap = self;
    private->current_bucket = -1;

    return iter;
}

HashMap *hashmap_new(hash_func_t hash_f, cmp_func_t cmp_f)
{
    HashMap *map = malloc(sizeof(HashMap) + sizeof(HashMap_private));
    if (!map) return NULL;
    map->size = hashmap_size;
    map->put = hashmap_put;
    map->get = hashmap_get;
    map->iter = hashmap_iter;

    HashMap_private *private = (HashMap_private *)(map + 1);
    private->size = 0;
    private->capacity = DEFAULT_CAPACITY;
    private->hash = get_or_default(hash_f, default_hash);
    private->cmp = get_or_default(cmp_f, default_cmp);
    private->buckets = calloc(DEFAULT_CAPACITY, sizeof(MapEntry **));

    return map;
}

HashMap *hashmap_default()
{
    return hashmap_new(NULL, NULL);
}

void hashmap_free(HashMap *map)
{
    HashMap_private *private = (HashMap_private *)(map + 1);

    for (size_t i = 0; i < private->capacity; i++) {
        MapEntry_private *entry = private->buckets[i];
        while (entry) {
            MapEntry_private *next = entry->next;
            free(entry);
            entry = next;
        }
    }

    free(private->buckets);
    free(map);
}
