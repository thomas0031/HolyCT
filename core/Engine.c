#include "Engine.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdlib.h>
#include "../collections/HashMap.h"

typedef struct Context_private Context_private;
typedef struct Segment Segment;

struct Context_private {
    HashMap *map;
};

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
    String *var;
    String *data;
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

void context_insert(Context *self, String *key, ContextValue *value) 
{
    Context_private *data = (Context_private *)(self + 1);

    data->map->put(data->map, key, value);
}

ContextValue *context_get(const Context *self, String *key)
{
    Context_private *data = (Context_private *)(self + 1);

    return data->map->get(data->map, key);
}

Context *context_new(void) 
{
    Context *ctx = malloc(sizeof(Context) + sizeof(Context_private));
    if (!ctx) return NULL;

    ctx->insert = context_insert;
    ctx->get = context_get;

    Context_private *data = (Context_private *)(ctx + 1);
    data->map = hashmap_new(hash_string, cmp_string);

    return ctx;
}

typedef struct engine_private engine_private;

struct engine_private {
    String *template;
    Vector *segments;
};

str_t read_from_file(str_t path)
{
    FILE *fp = fopen(path, "r");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    str_t buffer = malloc(size + 1);
    if (!buffer) return NULL;

    fread(buffer, 1, size, fp);
    buffer[size] = '\0';

    fclose(fp);

    return buffer;
}

Segment *segment_new_loop(String *var, String *data, Vector *segments)
{
    Segment *segment = malloc(sizeof(Segment));
    if (!segment) return NULL;

    segment->type = SEGMENT_LOOP;
    segment->data.loopSegment.var = var;
    segment->data.loopSegment.data = data;
    segment->data.loopSegment.segments = segments;

    return segment;
}

Segment *segment_new_variable(String *value)
{
    Segment *segment = malloc(sizeof(Segment));
    if (!segment) return NULL;

    segment->type = SEGMENT_VARIABLE;
    segment->data.variableSegment.value = value;

    return segment;
}

Segment *segment_new_static(String *value)
{
    Segment *segment = malloc(sizeof(Segment));
    if (!segment) return NULL;

    segment->type = SEGMENT_STATIC;
    segment->data.staticSegment.value = value;

    return segment;
}

#define loop while (1)

void engine_preprocess(Engine *self)
{
    engine_private *private = (engine_private*)(self + 1);

    Vector *segments = private->segments;
    segments->clear(segments);
    Vector *stack = vector_default();
    stack->push(stack, segments);

    size_t pos = 0;
    while (pos < private->template->len(private->template)) {
        Slice *slice = private->template->get_slice(private->template, pos, pos + 2);
        if (slice_compare_cstr(slice, "{{") == 0) {
            slice_free(slice);
            slice = private->template->get_slice(private->template, pos, private->template->len(private->template));
            size_t end_pos = slice_find(slice, "}}") + 2;
            if (end_pos == 1) exit(1);  // TODO: error Handling
            slice_free(slice);
            end_pos += pos;
            slice = private->template->get_slice(private->template, pos + 2, end_pos - 2);
            String *directive = slice_to_string(slice_trim(slice));

            if (directive->starts_with(directive, "for ")) {
                Vector *tokens = directive->split(directive, " ");
                String *loop_var = tokens->get(tokens, 1);
                String *loop_data = tokens->get(tokens, 3);
                Vector *loop_segments = vector_default();
                Vector *last = stack->last(stack);
                last->push(last, segment_new_loop(loop_var, loop_data, loop_segments));
                stack->push(stack, loop_segments);
            } else if (directive->compare_cstr(directive, "endfor") == 0) {
                stack->pop(stack);
            } else {
                Vector *last = stack->last(stack);
                last->push(last, segment_new_variable(directive));
            }

            pos = end_pos;
        } else {
            slice = private->template->get_slice(private->template, pos, private->template->len(private->template));
            size_t next_pos = slice_find(slice, "{{");
            slice_free(slice);
            next_pos = next_pos == -1 ? private->template->len(private->template) : next_pos + pos;
            Vector *last = stack->last(stack);
            slice = private->template->get_slice(private->template, pos, next_pos);
            last->push(last, segment_new_static(slice_to_string(slice)));

            pos = next_pos;
        }
    }

    if (stack->len(stack) != 1) exit(1);
}

ContextValue *context_value_from_segment(Segment *s)
{
    printf("creating context value from segment\n");
    switch (s->type) {
        case SEGMENT_STATIC: {
            printf("static: value: %s\n", s->data.staticSegment.value->as_cstr(s->data.staticSegment.value));
            return context_value_new_string(s->data.staticSegment.value->as_cstr(s->data.staticSegment.value));
        } break;
        case SEGMENT_VARIABLE: {
            printf("variable: value: %s\n", s->data.variableSegment.value->as_cstr(s->data.variableSegment.value));
            return context_value_new_string(s->data.variableSegment.value->as_cstr(s->data.variableSegment.value));
        } break;
        case SEGMENT_LOOP: {
            printf("loop:");
            return NULL;
        } break;
        default:
            exit(1);
    }
}

String *engine_optimized_render(const Engine *self, Context *ctx)
{
    engine_private *private = (engine_private*)(self + 1);

    Vector *segments = private->segments;

    Vector *parts = vector_default();
    printf("segments len: %zu\n", segments->len(segments));
    for (size_t i = 0; i < segments->len(segments); ++i) {
        const Segment *segment = segments->get(segments, i);
        printf("processing segment %zu\n", i);
        printf("segment type: %d\n", segment->type);

        switch (segment->type) {
            case SEGMENT_STATIC: {
                printf("static: \n");
                parts->push(parts, segment->data.staticSegment.value);
            } break;
            case SEGMENT_VARIABLE: {
                printf("variable: \n");
                parts->push(parts, ctx->get(ctx, segment->data.variableSegment.value));
            } break;
            case SEGMENT_LOOP: {
                String *loop_var = segment->data.loopSegment.var;
                String *loop_data = segment->data.loopSegment.data;
                Vector *loop_segments = segment->data.loopSegment.segments;

                printf("loop: %s in %s\n", loop_var->as_cstr(loop_var), loop_data->as_cstr(loop_data));

                Vector *arr = ctx->get(ctx, loop_data)->value.vector;
                size_t arr_len = arr->len(arr);
                printf("loop_arr len: %zu\n", arr_len);
                for (size_t j = 0; j < arr_len; ++j) {
                   Context* inner_ctx = ctx;
                   Segment *lopp_arr_el = arr->get(arr, j);
                   printf("loop_arr_el->type: %d\n", lopp_arr_el->type);
                   ContextValue *value_to_insert = context_value_from_segment(lopp_arr_el);
                   inner_ctx->insert(inner_ctx, loop_var, value_to_insert); // TODO should be reset
                   private->segments = loop_segments;
                   parts->push(parts, engine_optimized_render(self, inner_ctx));
                }
            } break;
            default:
                exit(1);
        }

        //sleep(1);
    }

    printf("parts len: %zu\n", parts->len(parts));
    for (size_t i = 0; i < parts->len(parts); ++i) {
        const String *part = parts->get(parts, i);
        printf("part %zu: %s\n", i, part->as_cstr(part));
    }
    String *result = string_join(parts, "");
    printf("result: %s\n", result->as_cstr(result));
    return result;
}

Engine *engine_new(str_t path)
{
    Engine *eng = malloc(sizeof(Engine) + sizeof(engine_private));
    if (!eng) return NULL;

    eng->preprocess = engine_preprocess;
    eng->optimized_render = engine_optimized_render;

    engine_private *private = (engine_private*)(eng + 1);
    str_t template = read_from_file(path);
    private->template = string_new_from_cstr(template);
    private->segments = vector_default();

    return eng;
}

ContextValue *context_value_new_string(str_t value)
{
    ContextValue *ctx_value = malloc(sizeof(ContextValue));
    if (!ctx_value) return NULL;

    ctx_value->type = CONTEXT_VALUE_STRING;
    ctx_value->value.string = string_new_from_cstr(value);

    return ctx_value;
}

ContextValue *context_value_new_vector(str_t value, ...)
{
    ContextValue *ctx_value = malloc(sizeof(ContextValue));
    if (!ctx_value) return NULL;

    ctx_value->type = CONTEXT_VALUE_VECTOR;
    ctx_value->value.vector = vector_default();

    va_list args;
    va_start(args, value);
    str_t arg = value;
    while (arg) {
        ctx_value->value.vector->push(ctx_value->value.vector, context_value_new_string(arg));
        arg = va_arg(args, str_t);
    }
    va_end(args);

    return ctx_value;
}
