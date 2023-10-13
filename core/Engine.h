#ifndef ENGINE_H
#define ENGINE_H

#include "../collections/String.h"
#include "../collections/Vector_generic.h"

typedef struct Context Context;
typedef struct Engine Engine;

typedef struct ContextValue {
    enum {
        CONTEXT_VALUE_STRING,
        CONTEXT_VALUE_VECTOR,
    } type;
    union {
        String *string;
        Vector *vector;
    } value;
} ContextValue;

struct Context {
    void (*insert)(Context *self, String *key, ContextValue *value);
    ContextValue* (*get)(const Context *self, String *key);
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

ContextValue *context_value_new_string(str_t);

ContextValue *context_value_new_vector(str_t, ...);

#endif // !ENGINE_H
