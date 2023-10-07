#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdbool.h>

typedef struct hashmap hashmap;
typedef hashmap *hashmap_t;

/**
 * A function that hashes a key.
 *
 * @param  key The key to hash.
 *
 * @return The hash of the key.
 */
typedef size_t (*hash_func_t)(const void *);
/**
 * A function that compares two keys.
 *
 * @param  a The first key.
 * @param  b The second key.
 *
 * @return < 0 if a < b, 0 if a == b, > 0 if a > b.
 */
typedef int (*cmp_func_t)(const void *, const void *);

/**
 * Creates a new hashmap with the default hash and compare functions.
 *
 * @return A new hashmap.
 */
hashmap_t hashmap_default(void);


/**
 * Creates a new hashmap with the given hash and compare functions.
 *
 * @param  hash The hash function to use.
 * @param  cmp  The compare function to use.
 *
 * @return A new hashmap.
 */
hashmap_t hashmap_new(
        hash_func_t hash,
        cmp_func_t cmp
        );

void hashmap_free(hashmap_t map);

/**
 * Returns the number of key-value pairs in the hashmap.
 *
 * @param  map The hashmap to get the size of.
 *
 * @return The number of key-value pairs in the hashmap.
 */
size_t hashmap_size(hashmap_t map);

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
bool hashmap_put(hashmap_t map, const void *key, void *data);

/**
 * Gets the data associated with a key.
 *
 * @param  map The hashmap to get from.
 * @param  key The key to get.
 *
 * @return The data associated with the key, or NULL if the key does not exist.
 */
void *hashmap_get(hashmap_t map, const void *key);

#endif // HASHMAP_H
