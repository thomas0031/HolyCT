#ifndef ENGINE_H
#define ENGINE_H

#include "../collections/String.h"
#include "../collections/Vector_generic.h"

typedef struct Context Context;
typedef struct Engine Engine;

typedef enum {
    SEGMENT_STATIC,
    SEGMENT_VARIABLE,
    SEGMENT_LOOP
} SegmentType;

typedef struct {
    String *value;
} StaticSegment;

typedef struct {
    String *value;
} VariableSegment;

typedef struct {
    char *start;
    char *end;
    size_t count;
    Vector *segments;
} LoopSegment;

typedef union {
    StaticSegment staticSegment;
    VariableSegment variableSegment;
    LoopSegment loopSegment;
} SegmentData;

typedef struct Segment {
    SegmentType type;
    SegmentData data;
} Segment;

struct Context {
    void (*insert)(Context *self, String *key, Segment *value);
    Segment* (*get)(const Context *self, String *key);
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

Segment *segment_new_static(str_t);

Segment *segment_new_variable(str_t);

Segment *segment_new_loop(str_t);

#endif // !ENGINE_H
