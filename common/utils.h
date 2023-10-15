#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include "../core/Engine.h"
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

/**
 * Python eval function.
 *
 * @param  expr The expression to evaluate.
 * 
 * @return The result of the evaluation.
 */
bool eval_condition(const str_t expr, const Context *context);

#endif // !UTILS_H
