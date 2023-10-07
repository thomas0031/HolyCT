#ifndef CONTEXT_H
#define CONTEXT_H

#include "../global/types.h"

typedef struct context {
    void (*insert)(struct context *self, str_t key, str_t value);
    void (*print)(struct context *self);
} context;

typedef context *context_t;

context_t context_new(void);

#endif // !CONTEXT_H
