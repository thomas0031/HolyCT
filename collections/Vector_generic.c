#include "Vector_generic.h"
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_VEC_CAP 8

typedef struct Vector_private vector_private;

struct Vector_private {
    size_t len;
    size_t cap;
    void **data;
};

size_t vector_generic_len(const Vector *self) {
    vector_private *private = (vector_private *)(self + 1);

    return private->len;
}

size_t vector_generic_cap(const Vector *self) {
    vector_private *private = (vector_private *)(self + 1);

    return private->cap;
}

void vector_generic_push(Vector *self, void *item) {
    vector_private *private = (vector_private *)(self + 1);

    if (private->len == private->cap) {
        private->cap *= 2;
        private->data = realloc(private->data, sizeof(void *) * private->cap);
    }

    private->data[private->len++] = item;
}

void *vector_generic_get(const Vector *self, size_t index) {
    vector_private *private = (vector_private *)(self + 1);

    // TODO: need to think about error handling (would be nice to add result/option types)
    if (index >= private->len) return 0;

    return private->data[index];
}

void *vector_generic_get_ptr(Vector *self, size_t index) {
    vector_private *private = (vector_private *)(self + 1);

    if (index >= private->len) return NULL;

    return private->data[index];
}

void *vector_generic_get_ptr_raw(Vector *self, size_t index)
{
    vector_private *private = (vector_private *)(self + 1);

    return private->data + index;
}

void vector_generic_clear(Vector *self)
{
    vector_private *private = (vector_private *)(self + 1);

    // TODO not correct
    for(int i = 0; i < private->len; ++i) free(private->data[i]);
}

// TODO remove when impl optional
// TODO move to utils or even better add optional type
static inline const void *get_or_default(const void *optional, const void *default_value) {
    return optional == NULL ? default_value : optional;
}

Vector *vector_default() {
    return vector_with_capacity(DEFAULT_VEC_CAP);
}

Vector *vector_with_capacity(size_t capacity) {
    Vector *vec = malloc(sizeof(Vector) + sizeof(vector_private));
    if (vec == NULL) return NULL;

    vec->len = vector_generic_len;
    vec->capacity = vector_generic_cap;
    vec->push = vector_generic_push;
    vec->get = vector_generic_get;
    vec->get_ptr = vector_generic_get_ptr;
    vec->get_ptr_raw = vector_generic_get_ptr_raw;
    vec->clear = vector_generic_clear;

    vector_private *private = (vector_private *)(vec + 1);
    private->len = 0;
    private->cap = capacity;
    private->data = malloc(sizeof(void*) * capacity);
    // TODO need also eq function

    return vec;
}

void vector_free(Vector *vec) {
    vector_private *private = (vector_private *)(vec + 1);

    // TODO not correct
    free(private->data);
    free(vec);
}
