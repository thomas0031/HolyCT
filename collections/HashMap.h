#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

typedef struct hashmap_entry hashmap_entry;
typedef hashmap_entry *hashmap_entry_t;
typedef struct hashmap_iterator hashmap_iterator;
typedef hashmap_iterator *hashmap_iterator_t;
typedef struct hashmap hashmap;
typedef hashmap *hashmap_t;

struct hashmap_entry {
    /**
     * Returns the key of the entry.
     *
     * @param  entry The entry to get the key from.
     *
     * @return The key of the entry.
     */
    void * (*key)(struct hashmap_entry *self);
    /**
     * Returns the value of the entry.
     *
     * @param  entry The entry to get the value from.
     *
     * @return The value of the entry.
     */
    void * (*value)(struct hashmap_entry *self);
};

struct hashmap_iterator {
    /**
     * Returns true if there are more entries in the hashmap.
     *
     * @param  iter The iterator to check.
     *
     * @return true if there are more entries in the hashmap, false otherwise.
     */
    bool (*has_next)(struct hashmap_iterator *self);

    /**
     * Returns the next entry in the hashmap.
     *
     * @param  iter The iterator to get the next entry from.
     *
     * @return The next entry in the hashmap.
     */
    hashmap_entry_t (*next)(struct hashmap_iterator *self);
};

struct hashmap {
    /**
     * Returns the number of key-value pairs in the hashmap.
     *
     * @param  map The hashmap to get the size of.
     *
     * @return The number of key-value pairs in the hashmap.
     */
    size_t (*size)(struct hashmap *self);

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
    bool (*put)(struct hashmap *self, const void *key, void *data);

    /**
     * Gets the data associated with a key.
     *
     * @param  map The hashmap to get from.
     * @param  key The key to get.
     *
     * @return The data associated with the key, or NULL if the key does not exist.
     */
    void * (*get)(struct hashmap *self, const void *key);

    /**
     * Prints the contents of the hashmap to stdout.
     *
     * @param map The hashmap to print.
     */
    void  (*print)(struct hashmap *self);

    /**
     * Returns iterator over the entries in the hashmap.
     *
     * @param  map The hashmap to iterate over.
     *
     * @return An iterator over the entries in the hashmap.
     */
    hashmap_iterator* (*iter)(struct hashmap *self);
};

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

// TODO move to utils.h
/**
 * A function that prints a key-value pair to stdout.
 *
 * @param key   The key to print.
 * @param value The value to print.
 */
typedef void (*print_2_func_t)(const void *, const void *);

// TODO move to utils.h
/**
 * A default hash function for strings.
 *
 * @param  key The string to hash.
 *
 * @return The hash of the string.
 */
static size_t hash_str(const void *key)
{
    size_t hash = 5381;
    const char *str = (const char *)key;
    int c;

    while ((c = *str++)) hash = ((hash << 5) + hash) + c;

    return hash;
}

// TODO move to utils.h
/**
 * A default compare function for strings.
 *
 * @param  a The first string.
 * @param  b The second string.
 *
 * @return < 0 if a < b, 0 if a == b, > 0 if a > b.
 */
static int cmp_str(const void *a, const void *b)
{
    return strcmp((char *)a, (char *)b);
}

// TODO move to utils.h
/**
 * A default print function for strings.
 *
 * @param key   The key to print.
 * @param value The value to print.
 */
static void print_str(const void *key, const void *value)
{
    printf("%s: %s\n", (const char *)key, (const char *)value);
}

/**
 * Creates a new hashmap with the default hash and compare functions.
 *
 * @return A new hashmap.
 */
hashmap_t hashmap_default(void);


/**
 * Creates a new hashmap with the given hash and compare functions.
 *
 * @param  hash_f The hash function to use.
 * @param  cmp_f  The compare function to use.
 * @param  print_f The print function to use.
 *
 * @return A new hashmap.
 */
hashmap_t hashmap_new(
        hash_func_t hash_f,
        cmp_func_t cmp_f,
        print_2_func_t print_f
        );

/**
 * Frees the memory used by the hashmap.
 *
 * @param map The hashmap to free.
 */
void hashmap_free(hashmap_t map);

#endif // HASHMAP_H
