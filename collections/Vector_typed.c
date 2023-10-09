#include "Vector_typed.h"
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_VEC_CAP 8

#define PRINT_VALUE(value) _Generic((value),\
    u8: print_u8,\
    i8: print_i8,\
    u16: print_u16,\
    i16: print_i16,\
    u32: print_u32,\
    i32: print_i32,\
    u64: print_u64,\
    i64: print_i64,\
    f32: print_f32,\
    f64: print_f64\
)(value)

#define DEFINE_VECTOR_FUNCTIONS_FOR_TYPE(T)\
typedef struct vector_##T##_private vector_##T##_private;\
\
struct vector_##T##_private {\
    size_t len;\
    size_t cap;\
    T * data;\
};\
\
size_t vector_##T##_len(Vector_##T##_t self) {\
    vector_##T##_private *private = (vector_##T##_private *)(self + 1);\
\
    return private->len;\
}\
\
size_t vector_##T##_cap(Vector_##T##_t self) {\
    vector_##T##_private *private = (vector_##T##_private *)(self + 1);\
\
    return private->cap;\
}\
\
void vector_##T##_push(Vector_##T##_t self, T item) {\
    vector_##T##_private *private = (vector_##T##_private *)(self + 1);\
\
    if (private->len == private->cap) {\
        private->cap *= 2;\
        private->data = realloc(private->data, sizeof(void *) * private->cap);\
    }\
\
    private->data[private->len++] = item;\
}\
\
T vector_##T##_get(Vector_##T##_t self, size_t index) {\
    vector_##T##_private *private = (vector_##T##_private *)(self + 1);\
\
    if (index >= private->len) return 0;\
\
    return private->data[index];\
}\
\
T *vector_##T##_get_ptr(Vector_##T##_t self, size_t index) {\
    vector_##T##_private *private = (vector_##T##_private *)(self + 1);\
\
    if (index >= private->len) return NULL;\
\
    return &private->data[index];\
}\
\
T *vector_##T##_get_ptr_raw(Vector_##T##_t self, size_t index)\
{\
    vector_##T##_private *private = (vector_##T##_private *)(self + 1);\
\
    return &private->data[index];\
}\
\
void vector_##T##_print(Vector_##T##_t self)\
{\
    vector_##T##_private *private = (vector_##T##_private *)(self + 1);\
\
    putchar('[');\
    for (size_t i = 0; i < private->len; i++) {\
        PRINT_VALUE(private->data[i]);\
        if (i != private->len - 1) {\
            printf(", ");\
        }\
    }\
    putchar(']');\
}\
\
Vector_##T##_t vector_##T##_default(void) {\
    Vector_##T##_t vec = malloc(sizeof(Vector_##T) + sizeof(vector_##T##_private));\
    if (vec == NULL) return NULL;\
\
    vec->len = vector_##T##_len;\
    vec->cap = vector_##T##_cap;\
    vec->push = vector_##T##_push;\
    vec->get = vector_##T##_get;\
    vec->get_ptr = vector_##T##_get_ptr;\
    vec->get_ptr_raw = vector_##T##_get_ptr_raw;\
    vec->print = vector_##T##_print;\
\
    vector_##T##_private *private = (vector_##T##_private *)(vec + 1);\
    private->len = 0;\
    private->cap = DEFAULT_VEC_CAP;\
    private->data = malloc(sizeof(void*) * DEFAULT_VEC_CAP);\
\
    return vec;\
}\
\
Vector_##T##_t vector_##T##_with_capacity(size_t capacity) {\
    Vector_##T##_t vec = malloc(sizeof(Vector_##T) + sizeof(vector_##T##_private));\
    if (vec == NULL) return NULL;\
\
    vec->len = vector_##T##_len;\
    vec->cap = vector_##T##_cap;\
    vec->push = vector_##T##_push;\
    vec->get = vector_##T##_get;\
    vec->get_ptr = vector_##T##_get_ptr;\
    vec->get_ptr_raw = vector_##T##_get_ptr_raw;\
    vec->print = vector_##T##_print;\
\
    vector_##T##_private *private = (vector_##T##_private *)(vec + 1);\
    private->len = 0;\
    private->cap = capacity;\
    private->data = malloc(sizeof(void*) * capacity);\
\
    return vec;\
}\
\
void vector_##T##_free(Vector_##T##_t vec) {\
    vector_##T##_private *private = (vector_##T##_private *)(vec + 1);\
\
    free(private->data);\
    free(vec);\
}\

void print_u8(u8 value)
{
    printf("%u", value);
}

void print_i8(i8 value)
{
    printf("%d", value);
}

void print_u16(u16 value)
{
    printf("%u", value);
}

void print_i16(i16 value)
{
    printf("%d", value);
}

void print_u32(u32 value)
{
    printf("%u", value);
}

void print_i32(i32 value)
{
    printf("%d", value);
}

void print_u64(u64 value)
{
    printf("%lu", value);
}

void print_i64(i64 value)
{
    printf("%ld", value);
}

void print_f32(f32 value)
{
    printf("%f", value);
}

void print_f64(f64 value)
{
    printf("%lf", value);
}

DEFINE_VECTOR_FUNCTIONS_FOR_TYPE(u8)
DEFINE_VECTOR_FUNCTIONS_FOR_TYPE(i8)
DEFINE_VECTOR_FUNCTIONS_FOR_TYPE(u16)
DEFINE_VECTOR_FUNCTIONS_FOR_TYPE(i16)
DEFINE_VECTOR_FUNCTIONS_FOR_TYPE(u32)
DEFINE_VECTOR_FUNCTIONS_FOR_TYPE(i32)
DEFINE_VECTOR_FUNCTIONS_FOR_TYPE(u64)
DEFINE_VECTOR_FUNCTIONS_FOR_TYPE(i64)
DEFINE_VECTOR_FUNCTIONS_FOR_TYPE(f32)
DEFINE_VECTOR_FUNCTIONS_FOR_TYPE(f64)
