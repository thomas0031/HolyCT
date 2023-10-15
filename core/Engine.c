#include "Engine.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "../collections/HashMap.h"
#include "../common/Regex.h"
#include "../common/utils.h"

typedef struct Context_private Context_private;
typedef struct Segment Segment;

struct Context_private {
    HashMap *map;
};

typedef enum {
    SEGMENT_STATIC,
    SEGMENT_VARIABLE,
    SEGMENT_LOOP,
    SEGMENT_RANGE,
    SEGMENT_IF_CONDITION,
    SEGMENT_ELSE_CONDITION,
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

typedef struct {
    String *var;
    size_t start;
    size_t end;
    Vector *segments;
} RangeSegment;

typedef struct {
    str_t key;
    String *condition;
    Vector *segments;
} IfConditionSegment;

typedef struct {
    Vector *segments;
} ElseConditionSegment;

typedef union {
    StaticSegment staticSegment;
    VariableSegment variableSegment;
    LoopSegment loopSegment;
    RangeSegment rangeSegment;
    IfConditionSegment ifConditionSegment;
    ElseConditionSegment elseConditionSegment;
} SegmentData;

typedef struct Segment {
    SegmentType type;
    SegmentData data;
} Segment;

void context_insert(Context *self, String *key, void *value) 
{
    Context_private *data = (Context_private *)(self + 1);

    data->map->put(data->map, key, value);
}

void *context_get(const Context *self, const String *key)
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

Segment *segment_new_range(String *var, size_t start, size_t end, Vector *segments)
{
    Segment *segment = malloc(sizeof(Segment));
    if (!segment) return NULL;

    segment->type = SEGMENT_RANGE;
    segment->data.rangeSegment.var = var;
    segment->data.rangeSegment.start = start;
    segment->data.rangeSegment.end = end;
    segment->data.rangeSegment.segments = segments;

    return segment;
}

Segment *segment_new_if_condition(str_t key, String *condition, Vector *segments)
{
    Segment *segment = malloc(sizeof(Segment));
    if (!segment) return NULL;

    segment->type = SEGMENT_IF_CONDITION;
    segment->data.ifConditionSegment.key = key;
    segment->data.ifConditionSegment.condition = condition;
    segment->data.ifConditionSegment.segments = segments;

    return segment;
}

Segment *segment_new_else_condition(Vector *segments)
{
    Segment *segment = malloc(sizeof(Segment));
    if (!segment) return NULL;

    segment->type = SEGMENT_ELSE_CONDITION;
    segment->data.elseConditionSegment.segments = segments;

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
                Vector *loop_segments = vector_default();
                str_t *range_match = match("for ([[:alnum:]]+) in ([[:digit:]]+)\\.\\.([[:digit:]]+)", directive->as_cstr(directive), 3);
                if (range_match) {
                    str_t loop_var = range_match[0];
                    size_t start = atoi(range_match[1]);
                    size_t end = atoi(range_match[2]);
                    Vector *last = stack->last(stack);
                    last->push(last, segment_new_range(string_new_from_cstr(loop_var), start, end, loop_segments));
                } else {
                    Vector *tokens = directive->split(directive, " ");
                    String *loop_var = tokens->get(tokens, 1);
                    String *loop_data = tokens->get(tokens, 3);
                    Vector *last = stack->last(stack);
                    last->push(last, segment_new_loop(loop_var, loop_data, loop_segments));
                }
                stack->push(stack, loop_segments);
            } else if (directive->starts_with(directive, "if ")) {
                String *condition = slice_to_string(directive->get_slice(directive, 3, directive->len(directive)));
                Vector *condition_segments = vector_default();
                Vector *last = stack->last(stack);
                last->push(last, segment_new_if_condition("if", condition, condition_segments));
                stack->push(stack, condition_segments);
            } else if (directive->starts_with(directive, "elif ")) {
                String *condition = slice_to_string(directive->get_slice(directive, 5, directive->len(directive)));
                stack->pop(stack);
                Vector *condition_segments = vector_default();
                Vector *last = stack->last(stack);
                last->push(last, segment_new_if_condition("elif", condition, condition_segments));
                stack->push(stack, condition_segments);
            } else if (directive->compare_cstr(directive, "else") == 0) {
                stack->pop(stack);
                Vector *else_segments = vector_default();
                Vector *last = stack->last(stack);
                last->push(last, segment_new_else_condition(else_segments));
                stack->push(stack, else_segments);
            } else if (directive->compare_cstr(directive, "endfor") == 0 || directive->compare_cstr(directive, "endif") == 0) {
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

bool eval(const String *condition, const Context *ctx)
{
    void *get = ctx->get(ctx, condition);
    if (get == True) {
        return true;
    }
    //return true;
    return eval_condition(condition->as_cstr(condition), ctx);
}

String *engine_optimized_render(const Engine *self, Context *ctx)
{
    engine_private *private = (engine_private*)(self + 1);

    Vector *segments = private->segments;
    Vector *parts = vector_default();

    str_t skip_until = NULL;
    bool condition_met = false;

    for (size_t i = 0; i < segments->len(segments); ++i) {
        const Segment *segment = segments->get(segments, i);

        switch (segment->type) {
            case SEGMENT_STATIC: {
                parts->push(parts, segment->data.staticSegment.value);
            } break;
            case SEGMENT_VARIABLE: {
                parts->push(parts, ctx->get(ctx, segment->data.variableSegment.value));
            } break;
            case SEGMENT_IF_CONDITION: {
                str_t key = segment->data.ifConditionSegment.key;
                String *condition = segment->data.ifConditionSegment.condition;

                Vector *inner_segments = segment->data.ifConditionSegment.segments;

                if (strcmp(key, "if") == 0) {
                    if (eval(condition, ctx)) {
                        condition_met = true;
                        private->segments = inner_segments;
                        parts->push(parts, engine_optimized_render(self, ctx));
                    } else {
                        condition_met = false;
                    }
                } else if (strcmp(key, "elif") == 0) {
                    if (!condition_met) {
                        if (eval(condition, ctx)) {
                            condition_met = true;
                            private->segments = inner_segments;
                            parts->push(parts, engine_optimized_render(self, ctx));
                        }
                    }
                }

            } break;
            case SEGMENT_ELSE_CONDITION: {
                Vector *inner_segments = segment->data.elseConditionSegment.segments;

                if (!condition_met) {
                    private->segments = inner_segments;
                    parts->push(parts, engine_optimized_render(self, ctx));
                } else {
                    skip_until = "endif";
                }
            } break;
            // TODO do we need to handle endif?
            case SEGMENT_LOOP: {
                String *loop_var = segment->data.loopSegment.var;
                String *loop_data = segment->data.loopSegment.data;
                Vector *loop_segments = segment->data.loopSegment.segments;

                Vector *loop_data_vec = ctx->get(ctx, loop_data);
                for (size_t j = 0; j < loop_data_vec->len(loop_data_vec); ++j) {
                    Context *inner_ctx = ctx;
                    inner_ctx->insert(inner_ctx, loop_var, loop_data_vec->get(loop_data_vec, j));
                    private->segments = loop_segments;
                    String *loop_result = engine_optimized_render(self, inner_ctx);
                    parts->push(parts, loop_result);
                }

            } break;
            case SEGMENT_RANGE: {
                String *loop_var = segment->data.rangeSegment.var;
                size_t start = segment->data.rangeSegment.start;
                size_t end = segment->data.rangeSegment.end;
                Vector *loop_segments = segment->data.rangeSegment.segments;

                for (size_t j = start; j < end; ++j) {
                    Context *inner_ctx = ctx;
                    String *num = string_default();
                    num->push(num, '0' + j);
                    inner_ctx->insert(inner_ctx, loop_var, num);
                    private->segments = loop_segments;
                    String *loop_result = engine_optimized_render(self, inner_ctx);
                    parts->push(parts, loop_result);
                }
            } break;
            default:
                exit(1);
        }
    }

    return string_join(parts, "");
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
