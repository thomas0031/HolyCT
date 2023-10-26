#ifndef ENGINE_H
#define ENGINE_H

#include "../collections/Vector.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

typedef struct Context Context;
typedef struct Engine Engine;
typedef struct Segment Segment;
typedef struct Bool Bool;
typedef struct ListString ListString;

struct Bool {
    bool value;
};
#define True ((Bool){true})
#define False ((Bool){false})

// TODO move somewhere else
struct ListString {
    char **strings;
    size_t size;
};

enum SegmentType {
    // static
    BOOLEAN,
    STRING,
    NUMBER,
    LIST_STRING,

    // custom dynamic
    RANGE,
    LOOP,
    IF,
    ELSE,

    // custom static
    VAR,
    STATIC,
};

typedef struct {
    const char *var;
    size_t start;
    size_t end;
    Vector *segments;
} RangeSegment;

typedef struct {
    const char *var;
    const char *data;
    Vector *segments;
} LoopSegment;

typedef struct {
    const char *key;
    const char *condition;
    Vector *segments;
} IfSegment;

typedef struct {
    Vector *segments;
} ElseSegment;

typedef struct {
    const char *var;
} VarSegment;

typedef struct {
    const char *string;
} StaticSegment;

struct Segment {
    enum SegmentType type;
    union {
        bool boolean;
        char *string;
        int number;
        ListString *list_string;
        RangeSegment *range;
        LoopSegment *loop;
        IfSegment *if_condition;
        ElseSegment *else_condition;
        VarSegment *var;
        StaticSegment *static_string;
    } value;
};


ListString create_list_string(const char* first, ...);

struct Context {
    void (*insert)(Context *self, const char *key, Segment *value);
    Segment* (*get)(const Context *self, const char *key);
};
#define insert(ctx, key, value) insert,\
    _Generic((value),\
        Bool: context_insert_boolean,\
        char*: context_insert_string,\
        int: context_insert_number,\
        ListString: context_insert_list_string\
    )(ctx, key, value)
void context_insert_boolean(Context *self, const char *key, Bool value);
void context_insert_string(Context *self, const char *key, char *value);
void context_insert_number(Context *self, const char *key, int value);
void context_insert_list_string(Context *self, const char *key, ListString value);
Context *context_new(void);

struct Engine {
    void (*preprocess)(Engine *self);
    const char* (*render)(const Engine *self, Context *ctx);
};
Engine *engine_new(const char *path);

#endif // !ENGINE_H
