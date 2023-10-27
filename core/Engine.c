#include "Engine.h"

#include "../common/Utils.h"
#include "../common/Regex.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_CONTEXT_SIZE 10
#define DEFAULT_SEGMENT_SIZE 10

typedef struct _Context _Context;
typedef struct Context_Entry Context_Entry;
typedef struct _Engine _Engine;

void print_segment_type(SegmentType type) {
    printf("Segment type: ");
    switch (type) {
        case STATIC:
            printf("STATIC");
            break;
        case VARIABLE:
            printf("VARIABLE");
            break;
        case LOOP:
            printf("LOOP");
            break;
        case RANGE:
            printf("RANGE");
            break;
        case IF:
            printf("IF");
            break;
        case ELSE:
            printf("ELSE");
            break;
    }
    printf("\n");
}
struct Context_Entry {
    const char *key;
    void *value;
};

struct _Context {
    Context_Entry *entries; // TODO change to hashmap
    size_t size;
    size_t capacity;
};

Segment *segment_new_static(char *var)
{
    Segment *segment = malloc(sizeof(Segment));

    segment->type = STATIC;
    segment->value.static_string = malloc(sizeof(StaticSegment));
    segment->value.static_string->var = var;

    return segment;
}

Segment *segment_new_var(const char *var)
{
    Segment *segment = malloc(sizeof(Segment));

    segment->type = VARIABLE;
    segment->value.variable = malloc(sizeof(VariableSegment));
    segment->value.variable->var = var;

    return segment;
}

Segment *segment_new_loop(const char *var, const char *data, Vector *segments)
{
    Segment *segment = malloc(sizeof(Segment));

    segment->type = LOOP;
    segment->value.loop = malloc(sizeof(LoopSegment));
    segment->value.loop->var = var;
    segment->value.loop->data = data;
    segment->value.loop->segments = segments;

    return segment;
}

Segment *segment_new_range(const char *var, size_t start, size_t end, Vector *segments)
{
    Segment *segment = malloc(sizeof(Segment));

    segment->type = RANGE;
    segment->value.range = malloc(sizeof(RangeSegment));
    segment->value.range->var = var;
    segment->value.range->start = start;
    segment->value.range->end = end;
    segment->value.range->segments = segments;

    return segment;
}

Segment *segment_new_if_condition(const char *key, const char *condition, Vector *segments)
{
    Segment *segment = malloc(sizeof(Segment));

    segment->type = IF;
    segment->value.if_condition = malloc(sizeof(IfSegment));
    segment->value.if_condition->key = key;
    segment->value.if_condition->condition = condition;
    segment->value.if_condition->segments = segments;

    return segment;
}

Segment *segment_new_else_condition(Vector *segments)
{
    Segment *segment = malloc(sizeof(Segment));

    segment->type = ELSE;
    segment->value.else_condition = malloc(sizeof(ElseSegment));
    segment->value.else_condition->segments = segments;

    return segment;
}

void segment_free(Segment *segment) {
    //printf("TODO segment_free\n");
}

void *context_get(const Context *ctx, const char *key)
{
    _Context *priv = (_Context *)(ctx + 1);
    for (size_t i = 0; i < priv->size; i++) {
        Context_Entry *entry = &priv->entries[i];
        // TODO possible buffer overflow but who cares ;)
        if (strcmp(entry->key, key) == 0) return entry->value;
    }
    return NULL;
}

void context_insert(Context *self, const char *key, void *value) {
    _Context *priv = (_Context *)(self + 1);

    void *get = self->get(self, key);
    if (get != NULL) {
        segment_free(get);
        get = value;
        return;
    }

    if (priv->size == priv->capacity) {
        priv->capacity *= 2;
        priv->entries = realloc(priv->entries, sizeof(Context_Entry) * priv->capacity);
    }
    priv->entries[priv->size++] = (Context_Entry) { .key = key, .value = value };
}

Context *context_new()
{
    Context *ctx = malloc(sizeof(Context) + sizeof(_Context));
    ctx->insert = context_insert;
    ctx->get = context_get;

    _Context *priv = (_Context *)(ctx + 1);
    priv->entries = malloc(sizeof(Context_Entry) * DEFAULT_CONTEXT_SIZE);
    priv->size = 0;
    priv->capacity = DEFAULT_CONTEXT_SIZE;

    return ctx;
}

struct _Engine {
    const char *template;
    size_t template_size;
    Vector *segments;
};

void engine_preprocess(Engine *engine)
{
    _Engine *priv = (_Engine *)(engine + 1);

    Vector *segments = priv->segments;

    Vector *stack = vector_default();
    stack->push(stack, segments);

    size_t pos = 0;
    while (pos < priv->template_size) {
        if (strncmp(priv->template + pos, "{{", 2) == 0) {
            const char *find = strstr(priv->template + pos, "}}");
            if (find == NULL) exit(1); // TODO error handling
            size_t end_pos = pos + (find - (priv->template + pos)) + 2;
            char *slice = str_slice(priv->template, pos + 2, end_pos - 2);
            char *directive = str_trim(slice);
            free(slice);

            if (strncmp(directive, "for ", 4) == 0) {
                Vector *loop_segments = vector_default();
                char **range_match = match("for ([[:alnum:]]+) in ([[:digit:]]+)\\.\\.([[:digit:]]+)", directive, 3);
                
                if (range_match) {
                    const char *loop_var = range_match[0];
                    size_t start = atoi(range_match[1]);
                    size_t end = atoi(range_match[2]);
                    Vector *last = stack->last(stack);
                    last->push(last, segment_new_range(loop_var, start, end, loop_segments));
                } else {
                    char **tokens = str_split(directive, " ");
                    assert(str_arr_size(tokens) == 4); // for, $var, in, $data
                    const char *loop_var = tokens[1];
                    const char *loop_data = tokens[3];
                    Vector *last = stack->last(stack);
                    last->push(last, segment_new_loop(loop_var, loop_data, loop_segments));
                }
                stack->push(stack, loop_segments);
            } else if (strncmp(directive, "if ", 3) == 0) {
                const char* condition = strdup(directive + 3);
                Vector *condition_segments = vector_default();
                Vector *last = stack->last(stack);
                last->push(last, segment_new_if_condition("if", condition, condition_segments));
                stack->push(stack, condition_segments);
            } else if (strncmp(directive, "elif ", 5) == 0) {
                const char* condition = strdup(directive + 5);
                stack->pop(stack);
                Vector *condition_segments = vector_default();
                Vector *last = stack->last(stack);
                last->push(last, segment_new_if_condition("elif", condition, condition_segments));
                stack->push(stack, condition_segments);
            } else if (strncmp(directive, "else", 4) == 0) {
                stack->pop(stack);
                Vector *else_segments = vector_default();
                Vector *last = stack->last(stack);
                last->push(last, segment_new_else_condition(else_segments));
                stack->push(stack, else_segments);
            } else if (strncmp(directive, "endfor", 6) == 0 || strncmp(directive, "endif", 5) == 0) {
                stack->pop(stack);
            } else {
                Vector *last = stack->last(stack);
                last->push(last, segment_new_var(strdup(directive)));
            }

            free(directive);

            pos = end_pos;
        } else {
            const char *find = strstr(priv->template + pos, "{{");
            size_t next_pos = find == NULL ? priv->template_size : find - priv->template;
            Vector *last = stack->last(stack);
            char* slice = str_slice(priv->template, pos, next_pos);
            last->push(last, segment_new_static(slice));

            pos = next_pos;
        }
    }

    if (stack->len(stack) != 1) exit(1); // TODO error handling
}


bool eval(const char *condition, const Context *ctx)
{
    void *get = ctx->get(ctx, condition);
    printf("evaluating \"%s\"\n", condition);
    return eval_condition(condition, ctx);
}

char *engine_render(const Engine *self, Context *ctx)
{
    _Engine *priv = (_Engine *)(self + 1);

    Vector *segments = priv->segments;
    Vector *parts = vector_default();

    char *skip_until = NULL;
    bool condition_met = false;
    //printf("\n\n\nRendering...\n");

    for (size_t i = 0; i < segments->len(segments); ++i) {
        const Segment *segment = segments->get(segments, i);
        //print_segment_type(segment->type);

        switch (segment->type) {
        case STATIC: {
            parts->push(parts, segment->value.static_string->var);
        } break;
        case VARIABLE: {
            parts->push(parts, ctx->get(ctx, segment->value.variable->var));
        } break;
        case IF: {
            const char *key = segment->value.if_condition->key;
            const char *condition = segment->value.if_condition->condition;
            printf("key: %s, condition: \"%s\"\n", key, condition);

            Vector *inner_segments = segment->value.if_condition->segments;

            if (strcmp(key, "if") == 0) {
                if (eval(condition, ctx)) {
                    condition_met = true;
                    priv->segments = inner_segments;
                    parts->push(parts, engine_render(self, ctx));
                } else {
                    condition_met = false;
                }
            } else if (strcmp(key, "elif") == 0) {
                if (!condition_met) {
                    if (eval(condition, ctx)) {
                        condition_met = true;
                        priv->segments = inner_segments;
                        parts->push(parts, engine_render(self, ctx));
                    }
                }
            }

        } break;
        case ELSE: {
            Vector *inner_segments = segment->value.else_condition->segments;

            if (!condition_met) {
                priv->segments = inner_segments;
                parts->push(parts, engine_render(self, ctx));
            } else {
                skip_until = "endif";
            }
        } break;
        case LOOP: {
            const char *loop_var = segment->value.loop->var;
            const char *loop_data = segment->value.loop->data;
            Vector *loop_segments = segment->value.loop->segments;

            Vector *loop_data_vec = ctx->get(ctx, loop_data);
            for (size_t j = 0; j < loop_data_vec->len(loop_data_vec); ++j) {
                Context *inner_ctx = ctx;
                inner_ctx->insert(inner_ctx, loop_var, loop_data_vec->get(loop_data_vec, j));
                priv->segments = loop_segments;
                char *loop_result = engine_render(self, inner_ctx);
                parts->push(parts, loop_result);
            }
        } break;
        case RANGE: {
            const char *loop_var = segment->value.range->var;
            size_t start = segment->value.range->start;
            size_t end = segment->value.range->end;
            Vector *loop_segments = segment->value.range->segments;

            for (size_t j = start; j < end; ++j) {
                Context *inner_ctx = ctx;
                char num[2] = { j + '0', '\0' };
                inner_ctx->insert(inner_ctx, loop_var, num);
                priv->segments = loop_segments;
                char *loop_result = engine_render(self, inner_ctx);
                parts->push(parts, loop_result);
            }
        } break;
        }
    }
    //printf("Rendered %zu parts\n", parts->len(parts));

    char *result = str_join(parts, "");
    //vector_free(parts); // TODO free parts

    return result;
}

const char* read_file(const char* path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) exit(1); // can't do anything without a template

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';

    fclose(file);

    return buffer;
}

Engine *engine_new(const char* path) {
    if (path == NULL) exit(1); // can't do anything without a template

    Engine *engine = malloc(sizeof(Engine) + sizeof(_Engine));
    engine->preprocess = engine_preprocess;
    engine->render = engine_render;

    _Engine *priv = (_Engine *)(engine + 1);
    priv->template = read_file(path);
    priv->template_size = strlen(priv->template);
    priv->segments = vector_default();

    return engine;
}
