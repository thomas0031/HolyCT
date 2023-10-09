#ifndef ENGINE_H
#define ENGINE_H

#include "../collections/String.h"
#include "../global/types.h"

typedef struct Context Context;
typedef Context *Context_t;
typedef struct Engine Engine;
typedef Engine *Engine_t;

struct Context {
    void (*insert)(struct Context *self, str_t key, str_t value);
    void (*print)(struct Context *self);
};

struct Engine {
    void (*update)(str_t path);
    String_t (*render)(struct Engine *self, Context_t ctx);
    void (*free)(void);
};

/**
 * Creates a new engine with the given template.
 *
 * @param  template The template to use.
 *
 * @return The new engine.
 */
Engine_t engine_new(str_t);

/**
 * Creates a new context.
 *
 * @return The new context.
 */
Context_t context_new(void);

#endif // !ENGINE_H
