#ifndef ENGINE_H
#define ENGINE_H

#include "../collections/Vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

typedef struct Context Context;
typedef struct Engine Engine;
typedef struct Segment Segment;
typedef struct Bool Bool;

struct Bool {
    bool value;
};
#define True ((Bool){true})
#define False ((Bool){false})

typedef enum {
    // static
    STATIC,

    // dynamic
    VARIABLE,
    LOOP,
    RANGE,
    IF,
    ELSE,
} SegmentType;

typedef struct {
    char *var;
} StaticSegment;

typedef struct {
    const char *var;
} VariableSegment;

typedef struct {
    const char *var;
    const char *data;
    Vector *segments;
} LoopSegment;

typedef struct {
    const char *var;
    size_t start;
    size_t end;
    Vector *segments;
} RangeSegment;

typedef struct {
    const char *key;
    const char *condition;
    Vector *segments;
} IfSegment;

typedef struct {
    Vector *segments;
} ElseSegment;

struct Segment {
    SegmentType type;
    union {
        StaticSegment *static_string;
        VariableSegment *variable;
        RangeSegment *range;
        LoopSegment *loop;
        IfSegment *if_condition;
        ElseSegment *else_condition;
    } value;
};

struct Context {
    void (*insert)(Context *self, const char *key, void *value);
    void *(*get)(const Context *self, const char *key);
};
Context *context_new(void);

struct Engine {
    void (*preprocess)(Engine *self);
    char* (*render)(const Engine *self, Context *ctx);
};
Engine *engine_new(const char *path);

#endif // !ENGINE_H
