#ifndef ENGINE_H
#define ENGINE_H

#include "../collections/String.h"
//#include "../global/types.h"
#include "../collections/Vector_generic.h"

typedef struct Context Context;
typedef Context *Context_t;
typedef struct Engine Engine;
typedef Engine *Engine_t;
typedef struct Segment Segment;
typedef Segment *Segment_t;

typedef enum {
    STATIC,
    DYNAMIC,
} SegmentType;

struct Segment {
    SegmentType type;
    Slice_t value;
};

struct Context {
    void (*insert)(Context_t self, str_t key, str_t value);
    str_t (*get)(Context_t self, str_t key);
    void (*print)(Context_t self);
};

struct Engine {
    void (*update)(str_t path);
    String_t (*render)(Engine_t self, Context_t ctx);
    void (*free)(Engine_t self);
    void (*preprocess)(Engine_t self);
    String_t (*optimized_render)(Engine_t self, Context_t ctx);
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
