#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

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

#endif // !UTILS_H
