#ifndef VECTOR_TYPED_H
#define VECTOR_TYPED_H

#include "../global/types.h"
#include <stddef.h>

#define DEFINE_VECTOR_FOR_TYPE(T)\
typedef struct Vector_##T Vector_##T;\
typedef Vector_##T * Vector_##T##_t;\
\
struct Vector_##T {\
    /**\
     * Returns the length of the vector.\
     *\
     * @param  self The vector.\
     *\
     * @return The length of the vector.\
     */\
    size_t (*len)(Vector_##T##_t self);\
\
    /**\
     * Returns the capacity of the vector.\
     *\
     * @param  self The vector.\
     *\
     * @return The capacity of the vector.\
     */\
    size_t (*cap)(Vector_##T##_t self);\
\
    /**\
     * Pushes the given item to the end of the vector.\
     *\
     * @param  self  The vector.\
     * @param  item The item to push.\
     */\
    void (*push)(Vector_##T##_t self, T item);\
\
    /**\
     * Returns the item at the given index.\
     *\
     * @param  vec   The vector.\
     * @param  index The index of the item.\
     *\
     * @return The item at the given index.\
     */\
    T (*get)(Vector_##T##_t self, size_t index);\
\
    /**\
     * Returns the pointer to the item at the given index.\
     *\
     * @param  self  The vector.\
     * @param  index The index of the item.\
     *\
     * @return The pointer to the item at the given index.\
     */\
    T * (*get_ptr)(Vector_##T##_t self, size_t index);\
\
    /**\
     * Returns the pointer to the item at the given index without bounds checking.(TODO remove this) \
     *\
     * @param  self   The vector.\
     * @param  index The index of the item.\
     *\
     * @return The pointer to the item at the given index.\
     */\
    T * (*get_ptr_raw)(Vector_##T##_t self, size_t index);\
\
    /**\
     * Prints the vector.\
     *\
     * @param  self  The vector.\
     */\
    void (*print)(Vector_##T##_t vec);\
};\
\
/**\
 * Creates a new vector with the default capacity.\
 *\
 * @return The new vector.\
 */\
Vector_##T##_t vector_##T##_default();\
\
/**\
 * Creates a new vector with the given capacity.\
 *\
 * @param  capacity The capacity of the vector.\
 *\
 * @return The new vector.\
 */\
Vector_##T##_t vector_##T##_with_capacity(size_t capacity);\
\
/**\
 * Frees the vector.\
 *\
 * @param  vec The vector.\
 */\
void vector_##T##_free(Vector_##T##_t vec);\

DEFINE_VECTOR_FOR_TYPE(u8)
DEFINE_VECTOR_FOR_TYPE(i8)
DEFINE_VECTOR_FOR_TYPE(u16)
DEFINE_VECTOR_FOR_TYPE(i16)
DEFINE_VECTOR_FOR_TYPE(u32)
DEFINE_VECTOR_FOR_TYPE(i32)
DEFINE_VECTOR_FOR_TYPE(u64)
DEFINE_VECTOR_FOR_TYPE(i64)
DEFINE_VECTOR_FOR_TYPE(f32)
DEFINE_VECTOR_FOR_TYPE(f64)

#endif // !VECTOR_TYPED_H
