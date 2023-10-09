#include "Vector_generic.h"
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_VEC_CAP 8

typedef struct vector_private vector_private;

struct vector_private {
    size_t len;
    size_t cap;
    void **data;
    print_1_func_t print;
};

size_t vector_generic_len(Vector_t self) {
    vector_private *private = (vector_private *)(self + 1);

    return private->len;
}

size_t vector_generic_cap(Vector_t self) {
    vector_private *private = (vector_private *)(self + 1);

    return private->cap;
}

void vector_generic_push(Vector_t self, void *item) {
    vector_private *private = (vector_private *)(self + 1);

    if (private->len == private->cap) {
        private->cap *= 2;
        private->data = realloc(private->data, sizeof(void *) * private->cap);
    }

    private->data[private->len++] = item;
}

void* vector_generic_get(Vector_t self, size_t index) {
    vector_private *private = (vector_private *)(self + 1);

    // TODO: need to think about error handling (would be nice to add result/option types)
    if (index >= private->len) return 0;

    return private->data[index];
}

void *vector_generic_get_ptr(Vector_t self, size_t index) {
    vector_private *private = (vector_private *)(self + 1);

    if (index >= private->len) return NULL;

    return private->data[index];
}

void *vector_generic_get_ptr_raw(Vector_t self, size_t index)
{
    vector_private *private = (vector_private *)(self + 1);

    return private->data + index;
}

void vector_generic_print(Vector_t self)
{
    vector_private *private = (vector_private *)(self + 1);

    putchar('[');
    for (size_t i = 0; i < private->len; i++) {
        private->print(private->data[i]);
        if (i != private->len - 1) {
            printf(", ");
        }
    }
    putchar(']');
}

// TODO remove when impl optional
static const void *get_or_default(const void *optional, const void *default_value) {
    return optional == NULL ? default_value : optional;
}

static void default_print(const void *item) {
    printf("%p", item);
}

Vector_t vector_default(print_1_func_t print_f) {
    Vector_t vec = malloc(sizeof(Vector) + sizeof(vector_private));
    if (vec == NULL) return NULL;

    vec->len = vector_generic_len;
    vec->cap = vector_generic_cap;
    vec->push = vector_generic_push;
    vec->get = vector_generic_get;
    vec->get_ptr = vector_generic_get_ptr;
    vec->get_ptr_raw = vector_generic_get_ptr_raw;
    vec->print = vector_generic_print;

    vector_private *private = (vector_private *)(vec + 1);
    private->len = 0;
    private->cap = DEFAULT_VEC_CAP;
    private->data = malloc(sizeof(void*) * DEFAULT_VEC_CAP);
    private->print = get_or_default(print_f, default_print);
    // TODO need also eq function

    return vec;
}

Vector_t vector_with_capacity(size_t capacity, print_1_func_t print_f) {
    Vector_t vec = malloc(sizeof(Vector) + sizeof(vector_private));
    if (vec == NULL) return NULL;

    vec->len = vector_generic_len;
    vec->cap = vector_generic_cap;
    vec->push = vector_generic_push;
    vec->get = vector_generic_get;
    vec->get_ptr = vector_generic_get_ptr;
    vec->get_ptr_raw = vector_generic_get_ptr_raw;
    vec->print = vector_generic_print;

    vector_private *private = (vector_private *)(vec + 1);
    private->len = 0;
    private->cap = capacity;
    private->data = malloc(sizeof(void*) * capacity);
    private->print = get_or_default(print_f, default_print);

    return vec;
}

void vector_free(Vector_t vec) {
    vector_private *private = (vector_private *)(vec + 1);

    free(private->data);
    free(vec);
}
