#ifndef VECTOR_H
#define VECTOR_H

#include "../global/types.h"
#include <stddef.h>

typedef struct vec_u8 vec_u8;
typedef vec_u8* vec_u8_t;

/**
 * Creates a new vector with the default capacity.
 *
 * @return The new vector.
 */
vec_u8_t vec_u8_default(void);

/**
 * Creates a new vector with the given capacity.
 *
 * @param  capacity The capacity of the vector.
 *
 * @return The new vector.
 */
vec_u8_t vec_u8_with_capacity(size_t capacity);

/**
 * Creates a new vector from the given array.
 *
 * @param  arr The array to create the vector from.
 * @param  len The length of the array.
 *
 * @return The new vector.
 */
void vec_u8_free(vec_u8_t vec);

/**
 * Returns the length of the vector.
 *
 * @param  vec The vector.
 *
 * @return The length of the vector.
 */
size_t vec_u8_len(vec_u8_t vec);

/**
 * Returns the capacity of the vector.
 *
 * @param  vec The vector.
 *
 * @return The capacity of the vector.
 */
size_t vec_u8_cap(vec_u8_t vec);

/**
 * Returns the item at the given index.
 *
 * @param  vec   The vector.
 * @param  index The index of the item.
 *
 * @return The item at the given index.
 */
void vec_u8_push(vec_u8_t vec, u8_t item);

/**
 * Returns the item at the given index.
 *
 * @param  vec   The vector.
 * @param  index The index of the item.
 *
 * @return The item at the given index.
 */
u8_t vec_u8_get(vec_u8_t vec, size_t index);

/**
 * Returns the item at the given index.
 *
 * @param  vec   The vector.
 * @param  index The index of the item.
 *
 * @return The item at the given index.
 */
void *vec_u8_get_ptr(vec_u8_t vec, size_t index);

// TODO: SORRYYY
/**
 * Returns the item at the given index.
 *
 * @param  vec   The vector.
 * @param  index The index of the item.
 *
 * @return The item at the given index.
 */
void *vec_u8_get_ptr_raw(vec_u8_t vec, size_t index);

/**
 * Sets the item at the given index to the given item.
 *
 * @param  vec   The vector.
 * @param  index The index of the item.
 * @param  item  The item to set.
 */
void vec_u8_print(vec_u8_t vec);

#endif // !VECTOR_H
