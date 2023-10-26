#ifndef VECTOR_GENERIC_H
#define VECTOR_GENERIC_H

#include <stddef.h>
typedef struct Vector Vector;

struct Vector {
    size_t      (*len)  (const Vector *self);
    const void* (*get)  (const Vector *self, size_t index);
    void*       (*last) (const Vector *self);
    void        (*push) (Vector *self, const void *e);
    const void* (*pop)  (Vector *self);
};

Vector *vector_default(void);

#endif // !VECTOR_GENERIC_H
