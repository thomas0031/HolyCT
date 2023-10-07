#ifndef VECTOR_H
#define VECTOR_H

#include "../global/types.h"
#include <stddef.h>

typedef struct vec_u8 vec_u8;
typedef vec_u8* vec_u8_t;

vec_u8_t vec_u8_default(void);
vec_u8_t vec_u8_with_capacity(size_t capacity);
void vec_u8_free(vec_u8_t vec);
size_t vec_u8_len(vec_u8_t vec);
size_t vec_u8_cap(vec_u8_t vec);
void vec_u8_push(vec_u8_t vec, u8_t item);
u8_t vec_u8_get(vec_u8_t vec, size_t index);
void *vec_u8_get_ptr(vec_u8_t vec, size_t index);
// TODO: SORRYYY
void *vec_u8_get_ptr_raw(vec_u8_t vec, size_t index);
void vec_u8_print(vec_u8_t vec);

#endif // !VECTOR_H
