#ifndef VECTOR_GENERIC_H
#define VECTOR_GENERIC_H

#include "../global/types.h"
#include <stddef.h>

typedef struct Vector Vector;
typedef Vector* Vector_t;

struct Vector {
    /**
     * Returns the length of the vector.
     *
     * @param  self The vector.
     *
     * @return The length of the vector.
     */
    size_t (*len)(Vector_t self);

    /**
     * Returns the capacity of the vector.
     *
     * @param  self The vector.
     *
     * @return The capacity of the vector.
     */
    size_t (*cap)(Vector_t self);

    /**
     * Pushes the given item to the end of the vector.
     *
     * @param  self  The vector.
     * @param  item The item to push.
     */
    void (*push)(Vector_t self, void *item);

    /**
     * Returns the item at the given index.
     *
     * @param  vec   The vector.
     * @param  index The index of the item.
     *
     * @return The item at the given index.
     */
    void * (*get)(Vector_t self, size_t index);

    /**
     * Returns the pointer to the item at the given index.
     *
     * @param  self  The vector.
     * @param  index The index of the item.
     *
     * @return The pointer to the item at the given index.
     */
    void * (*get_ptr)(Vector_t self, size_t index);

    // TODO: SORRYYY
    /**
     * Returns the pointer to the item at the given index without bounds checking.
     *
     * @param  self   The vector.
     * @param  index The index of the item.
     *
     * @return The pointer to the item at the given index.
     */
    void * (*get_ptr_raw)(Vector_t self, size_t index);

    /**
     * Prints the vector.
     *
     * @param  self  The vector.
     */
    void (*print)(Vector_t vec);
};

// TODO move to utils
/**
 * Prints elements of the vector.
 *
 * @param  item The item to print.
 */
typedef void (*print_1_func_t)(const void * item);

/**
 * Creates a new vector with the default capacity.
 *
 * @return The new vector.
 */
Vector_t vector_default(
        print_1_func_t print_func
        );

/**
 * Creates a new vector with the given capacity.
 *
 * @param  capacity The capacity of the vector.
 *
 * @return The new vector.
 */
Vector_t vector_with_capacity(
        size_t capacity,
        print_1_func_t print_func
        );

/**
 * Frees the vector.
 *
 * @param  vec The vector.
 */
void vector_free(Vector_t vec);

#endif // !VECTOR_GENERIC_H
