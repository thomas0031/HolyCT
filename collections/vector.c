#include "vector.h"
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_VEC_CAP 8

struct vec_u8 {
    size_t len;
    size_t cap;
    u8_t *data;
};

vec_u8_t vec_u8_default(void) {
    vec_u8_t vec = malloc(sizeof(struct vec_u8));

    vec->len = 0;
    vec->cap = DEFAULT_VEC_CAP;
    vec->data = malloc(sizeof(u8_t) * DEFAULT_VEC_CAP);

    return vec;
}

vec_u8_t vec_u8_with_capacity(size_t cap) {
    vec_u8_t vec = malloc(sizeof(struct vec_u8));

    vec->len = 0;
    vec->cap = cap;
    vec->data = malloc(sizeof(u8_t) * cap);

    return vec;
}

void vec_u8_free(vec_u8_t vec) {
    free(vec->data);
    free(vec);

    vec = NULL;
}

size_t vec_u8_len(vec_u8_t vec) {
    return vec->len;
}

size_t vec_u8_cap(vec_u8_t vec) {
    return vec->cap;
}

void vec_u8_push(vec_u8_t vec, u8_t val) {
    if (vec->len == vec->cap) {
        vec->cap *= 2;
        vec->data = realloc(vec->data, sizeof(u8_t) * vec->cap);
    }

    vec->data[vec->len++] = val;
}

u8_t vec_u8_get(vec_u8_t vec, size_t index) {
    // TODO: need to think about error handling (would be nice to add result/option types)
    if (index >= vec->len) return 0;

    return vec->data[index];
}

void *vec_u8_get_ptr(vec_u8_t vec, size_t index) {
    if (index >= vec->len) return NULL;

    return vec->data + index;
}

void *vec_u8_get_ptr_raw(vec_u8_t vec, size_t index)
{
    return vec->data + index;
}

void vec_u8_print(vec_u8_t vec)
{
    printf("vec_u8_t { len: %zu, cap: %zu, data: [", vec->len, vec->cap);
    for (size_t i = 0; i < vec->len; i++) {
        printf("%d", vec->data[i]);
        if (i != vec->len - 1) {
            printf(", ");
        }
    }
    printf("] }\n");
}
