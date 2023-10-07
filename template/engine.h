#ifndef ENGINE_H
#define ENGINE_H

#include "context.h"
#include "../collections/String.h"

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

#endif // !ENGINE_H
