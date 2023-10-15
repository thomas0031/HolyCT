#ifndef ENGINE_H
#define ENGINE_H

#include "../common/types.h"
#include "../collections/String.h"
#include "../collections/Vector_generic.h"

typedef struct Context Context;
typedef struct Engine Engine;

struct Context {
    void (*insert)(Context *self, String *key, void *value);
    void* (*get)(const Context *self, const String *key);
};

struct Engine {
    void (*update)(str_t path);
    void (*free)(Engine *self);
    void (*preprocess)(Engine *self);
    String* (*optimized_render)(const Engine *self, Context *ctx);
};

/**
 * Creates a new engine with the given template.
 *
 * @param  template The template to use.
 *
 * @return The new engine.
 */
Engine *engine_new(str_t);

/**
 * Creates a new context.
 *
 * @return The new context.
 */
Context *context_new(void);

#endif // !ENGINE_H
