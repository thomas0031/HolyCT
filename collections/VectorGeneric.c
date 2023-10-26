#include "VectorGeneric.h"
#include <stddef.h>
#include <stdlib.h>

#define DEFAULT_VECTOR_SIZE 10

typedef struct _Vector _Vector;

struct _Vector {
    const void **items;
    size_t size;
    size_t capacity;
};

size_t vector_len(const Vector *self) {
    _Vector *priv = (_Vector *)(self + 1);
    return priv->size;
}

const void *vector_get(const Vector *self, size_t index) {
    _Vector *priv = (_Vector *)(self + 1);
    return priv->items[index];
}

const void *vector_last(const Vector *self) {
    _Vector *priv = (_Vector *)(self + 1);
    if (priv->size == 0) return NULL;
    return priv->items[priv->size - 1];
}

void vector_push(Vector  *self, const void *e) {
    _Vector *priv = (_Vector *)(self + 1);
    if (priv->size == priv->capacity) {
        priv->capacity *= 2;
        priv->items = realloc(priv->items, sizeof(void *) * priv->capacity);
    }
    priv->items[priv->size++] = e;
}

const void *vector_pop(Vector *self) {
    _Vector *priv = (_Vector *)(self + 1);
    return priv->items[--priv->size];
}

Vector *vector_default(void) {
    Vector *vector = malloc(sizeof(Vector) + sizeof(_Vector));
    vector->len = vector_len;
    vector->get = vector_get;
    vector->last = vector_last;
    vector->push = vector_push;
    vector->pop = vector_pop;

    _Vector *priv = (_Vector *)(vector + 1);
    priv->items = malloc(sizeof(void *) * DEFAULT_VECTOR_SIZE);
    priv->size = 0;
    priv->capacity = DEFAULT_VECTOR_SIZE;

    return vector;
}
