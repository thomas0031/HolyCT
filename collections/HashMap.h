#ifndef HASHMAP_H
#define HASHMAP_H

#include "../common/utils.h"
#include "String.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

typedef struct MapEntry MapEntry;
typedef struct MapIterator MapIterator;
typedef struct HashMap HashMap;

struct MapEntry {
    /**
     * Returns the key of the entry.
     *
     * @param  entry The entry to get the key from.
     *
     * @return The key of the entry.
     */
    void * (*key)(const MapEntry *self);
    /**
     * Returns the value of the entry.
     *
     * @param  entry The entry to get the value from.
     *
     * @return The value of the entry.
     */
    void * (*value)(const MapEntry *self);
};

struct MapIterator {
    /**
     * Returns true if there are more entries in the hashmap.
     *
     * @param  iter The iterator to check.
     *
     * @return true if there are more entries in the hashmap, false otherwise.
     */
    bool (*has_next)(const MapIterator *self);

    /**
     * Returns the next entry in the hashmap.
     *
     * @param  iter The iterator to get the next entry from.
     *
     * @return The next entry in the hashmap.
     */
    MapEntry* (*next)(MapIterator *self);
};

struct HashMap {
    /**
     * Returns the number of key-value pairs in the hashmap.
     *
     * @param  map The hashmap to get the size of.
     *
     * @return The number of key-value pairs in the hashmap.
     */
    size_t (*size)(const HashMap *self);

    /**
     * Inserts a new key-value pair into the hashmap.
     * If the key already exists, the data is overwritten.
     *
     * @param  map  The hashmap to insert into.
     * @param  key  The key to insert.
     * @param  data The data to insert.
     *
     * @return true if the insertion/overwrite was successful, false otherwise.
     */
    bool (*put)(HashMap *self, const void *key, void *data);

    /**
     * Gets the data associated with a key.
     *
     * @param  map The hashmap to get from.
     * @param  key The key to get.
     *
     * @return The data associated with the key, or NULL if the key does not exist.
     */
    void * (*get)(const HashMap *self, const void *key);

    /**
     * Returns iterator over the entries in the hashmap.
     *
     * @param  map The hashmap to iterate over.
     *
     * @return An iterator over the entries in the hashmap.
     */
    MapIterator* (*iter)(HashMap *self);
};

/**
 * Creates a new hashmap with the default hash and compare functions.
 *
 * @return A new hashmap.
 */
HashMap *hashmap_default(void);


/**
 * Creates a new hashmap with the given hash and compare functions.
 *
 * @param  hash_f The hash function to use.
 * @param  cmp_f  The compare function to use.
 * @param  print_f The print function to use.
 *
 * @return A new hashmap.
 */
HashMap *hashmap_new(hash_func_t hash_f, cmp_func_t cmp_f);

/**
 * Frees the memory used by the hashmap.
 *
 * @param map The hashmap to free.
 */
void hashmap_free(HashMap *map);

#endif // HASHMAP_H
