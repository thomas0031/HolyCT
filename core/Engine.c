#include "Engine.h"

#include "../common/Utils.h"
#include "../common/Regex.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CONTEXT_SIZE 10
#define DEFAULT_SEGMENT_SIZE 10

typedef struct _Context _Context;
typedef struct Context_Entry Context_Entry;
typedef struct _Engine _Engine;
typedef struct ListSegments ListSegments;

struct Context_Entry {
    const char *key;
    Segment *value;
};

struct _Context {
    Context_Entry *entries; // TODO change to hashmap
    size_t size;
    size_t capacity;
};

ListString create_list_string(const char* first, ...) {
    ListString list;
    list.size = 0;
    list.strings = NULL;

    va_list args;
    va_start(args, first);
    
    const char* item = first;
    while (item) {
        // Resize the array to hold one more string
        list.strings = realloc(list.strings, (list.size + 1) * sizeof(char*));
        
        // Copy the string to the array
        list.strings[list.size] = strdup(item);
        list.size++;

        // Get the next string
        item = va_arg(args, const char*);
    }
    
    va_end(args);
    return list;
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

Segment *segment_new_if_condition(const char *key, const char *condition, Vector *segments)
{
    Segment *segment = malloc(sizeof(Segment));

    segment->type = IF;
    segment->value.if_condition->key = key;
    segment->value.if_condition->condition = condition;
    segment->value.if_condition->segments = segments;

    return segment;
}

Segment *segment_new_else_condition(Vector *segments)
{
    Segment *segment = malloc(sizeof(Segment));

    segment->type = ELSE;
    segment->value.else_condition->segments = segments;

    return segment;
}

Segment *segment_new_var(const char *var)
{
    Segment *segment = malloc(sizeof(Segment));

    segment->type = STRING;
    segment->value.var->var = var;

    return segment;
}

Segment *segment_new_static(const char *string)
{
    Segment *segment = malloc(sizeof(Segment));

    segment->type = STATIC;
    segment->value.static_string->string = string;

    return segment;
}

void segment_free(Segment *segment) {
    switch (segment->type) {
        case BOOLEAN:
            break;
        case STRING:
            free(segment->value.string);
            break;
        case NUMBER:
            break;
        case LIST_STRING:
            for (size_t i = 0; i < segment->value.list_string->size; i++) {
                free(segment->value.list_string->strings[i]);
            }
            free(segment->value.list_string->strings);
            break;
        default:
            printf("Dynamic segment type:\n\t");
            switch (segment->type) {
                case RANGE:
                    printf("range\n");
                    // TODO free value.range->segments & value.range
                    break;
                default:
                    printf("unknown\n");
                    exit(1);
            }
            break;
    }
}

Segment *context_get(const Context *ctx, const char *key)
{
    _Context *priv = (_Context *)(ctx + 1);
    for (size_t i = 0; i < priv->size; i++) {
        Context_Entry *entry = &priv->entries[i];
        // TODO possible buffer overflow but who cares ;)
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
    }
    return NULL;
}

void context_insert(Context *self, const char *key, Segment *value) {
    printf("Inserting %s, ", key);
    switch (value->type) {
        case BOOLEAN:
            printf("boolean, value: %s\n", value->value.boolean ? "true" : "false");
            break;
        case STRING:
            printf("string, value: %s\n", value->value.string);
            break;
        case NUMBER:
            printf("number, value: %d\n", value->value.number);
            break;
        case LIST_STRING:
            printf("list_string, size: %zu\n", value->value.list_string->size);
            for (size_t i = 0; i < value->value.list_string->size; i++) {
                printf("  %s\n", value->value.list_string->strings[i]);
            }
            break;
    }
    _Context *priv = (_Context *)(self + 1);
    Segment *get = self->get(self, key);
    if (get != NULL) {
        segment_free(get);
        *get = *value;
        return;
    }
    if (priv->size == priv->capacity) {
        priv->capacity *= 2;
        priv->entries = realloc(priv->entries, sizeof(Context_Entry) * priv->capacity);
    }
    Context_Entry *entry = &priv->entries[priv->size++];
    entry->key = key;
    entry->value = value;
}

void context_insert_number(Context *self, const char *key, int value) {
    Segment segment = {
        .type = NUMBER,
        .value.number = value,
    };
    context_insert(self, key, &segment);
}

void context_insert_string(Context *self, const char *key, char *value) {
    Segment segment = {
        .type = STRING,
        .value.string = value,
    };
    context_insert(self, key, &segment);
}

void context_insert_boolean(Context *self, const char *key, Bool value) {
    Segment segment = {
        .type = BOOLEAN,
        .value.boolean = value.value,
    };
    context_insert(self, key, &segment);
}

void context_insert_list_string(Context *self, const char *key, ListString value) {
    Segment segment = {
        .type = LIST_STRING,
        .value.list_string = &value,
    };
    context_insert(self, key, &segment);
}

Context *context_new()
{
    Context *ctx = malloc(sizeof(Context) + sizeof(_Context));
    ctx->insert = NULL;
    ctx->get = context_get;

    _Context *priv = (_Context *)(ctx + 1);
    priv->entries = malloc(sizeof(Context_Entry) * DEFAULT_CONTEXT_SIZE);
    priv->size = 0;
    priv->capacity = DEFAULT_CONTEXT_SIZE;

    return ctx;
}

struct ListSegments {
    Segment *segments;
    size_t size;
    size_t capacity;
};

ListSegments *list_segments_default() {
    ListSegments *list = malloc(sizeof(ListSegments));
    list->segments = malloc(sizeof(Segment) * DEFAULT_SEGMENT_SIZE);
    list->size = 0;
    list->capacity = DEFAULT_SEGMENT_SIZE;
    return list;
}

void list_segments_push(ListSegments *list, Segment *segment) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->segments = realloc(list->segments, sizeof(Segment) * list->capacity);
    }
    list->segments[list->size++] = *segment;
}

Segment *list_segments_last(ListSegments *list) {
    return &list->segments[list->size - 1];
}

void list_segments_clear(ListSegments *list) {
    for (size_t i = 0; i < list->size; i++) {
        segment_free(&list->segments[i]);
    }
    list->size = 0;
}

struct _Engine {
    const char *template;
    size_t template_size;
    ListSegments *segments;
};

void engine_preprocess(Engine *engine)
{
    _Engine *priv = (_Engine *)(engine + 1);

    ListSegments *segments = priv->segments;
    list_segments_clear(segments);

    Vector *stack = vector_default();
    stack->push(stack, segments);

    size_t pos = 0;
    while (pos < priv->template_size) {
        if (strncmp(priv->template + pos, "{{", 2) == 0) {
            const char *find = strstr(priv->template + pos, "}}");
            if (find == NULL) exit(1); // TODO error handling
            size_t end_pos = find - priv->template + 2;
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
                    assert(ARRAY_SIZE(tokens) == 4); // for, $var, in, $data
                    const char *loop_var = tokens[1];
                    const char *loop_data = tokens[3];
                    Vector *last = stack->last(stack);
                    last->push(last, segment_new_loop(loop_var, loop_data, loop_segments));
                }
                stack->push(stack, loop_segments);
            } else if (strncmp(directive, "if ", 3) == 0) {
                const char* condition = directive + 3;
                Vector *condition_segments = vector_default();
                Vector *last = stack->last(stack);
                last->push(last, segment_new_if_condition("if", condition, condition_segments));
                stack->push(stack, condition_segments);
            } else if (strncmp(directive, "elif ", 5) == 0) {
                const char* condition = directive + 5;
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
                last->push(last, segment_new_var(directive));
            }

            free(directive);

            pos = end_pos;
        } else {
            const char *find = strstr(priv->template + pos, "{{");
            size_t next_pos = find == NULL ? priv->template_size : find - priv->template + pos;
            Vector *last = stack->last(stack);
            char* slice = str_slice(priv->template, pos, next_pos);
            last->push(last, segment_new_static(strdup(slice)));

            pos = next_pos;
        }
    }

    if (stack->len(stack) != 1) exit(1); // TODO error handling
}

const char *engine_render(const Engine *engine, Context *ctx)
{
    _Engine *priv = (_Engine *)(engine + 1);
    // TODO
    return NULL;
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
    priv->segments = list_segments_default();

    return engine;
}
