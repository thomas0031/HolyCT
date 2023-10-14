#ifndef VECTOR_GENERIC_H
#define VECTOR_GENERIC_H

#include "../common/types.h"
#include <stddef.h>

typedef struct Vector Vector;

struct Vector {
    /**
     * Returns the length of the vector.
     *
     * @param  self The vector.
     *
     * @return The length of the vector.
     */
    size_t (*len)(const Vector *self);

    /**
     * Returns the capacity of the vector.
     *
     * @param  self The vector.
     *
     * @return The capacity of the vector.
     */
    size_t (*capacity)(const Vector *self);

    /**
     * Pushes the given item to the end of the vector.
     *
     * @param  self  The vector.
     * @param  item The item to push.
     */
    void (*push)(Vector *self, void *item);

    /**
     * Returns the item at the given index.
     *
     * @param  vec   The vector.
     * @param  index The index of the item.
     *
     * @return The item at the given index.
     */
    void * (*get)(const Vector *self, size_t index);

    /**
     * Returns the pointer to the item at the given index.
     *
     * @param  self  The vector.
     * @param  index The index of the item.
     *
     * @return The pointer to the item at the given index.
     */
    void * (*get_ptr)(Vector *self, size_t index);

    // TODO: SORRYYY
    /**
     * Returns the pointer to the item at the given index without bounds checking.
     *
     * @param  self   The vector.
     * @param  index The index of the item.
     *
     * @return The pointer to the item at the given index.
     */
    void * (*get_ptr_raw)(Vector *self, size_t index);

    void * (*last)(Vector *self);

    void * (*pop)(Vector *self);

    /**
    * Clear the vector.
    *
    * @param self the vector
    */
    void (*clear)(Vector *self);
};

/**
 * Creates a new vector with the default capacity.
 *
 * @return The new vector.
 */
Vector *vector_default();

/**
 * Creates a new vector with the given capacity.
 *
 * @param  capacity The capacity of the vector.
 *
 * @return The new vector.
 */
Vector *vector_with_capacity(size_t capacity);

/**
 * Frees the vector.
 *
 * @param  vec The vector.
 */
void vector_free(Vector *vec);

#endif // !VECTOR_GENERIC_H
