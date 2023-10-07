#ifndef ENGINE_H
#define ENGINE_H

#include "../collections/String.h"
#include "../global/types.h"

typedef struct context {
    void (*insert)(struct context *self, str_t key, str_t value);
    void (*print)(struct context *self);
} context;

typedef context *context_t;

typedef struct engine {
    void (*update)(str_t);
    string_t (*render)(struct engine *self, context_t);
    void (*free)(void);
} engine;
typedef engine *engine_t;

/**
 * Creates a new engine with the given template.
 *
 * @param  template The template to use.
 *
 * @return The new engine.
 */
engine_t engine_new(str_t);

/**
 * Creates a new context.
 *
 * @return The new context.
 */
context_t context_new(void);

#endif // !ENGINE_H
