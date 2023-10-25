#include "Engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CONTEXT_SIZE 10

typedef struct _Context _Context;
typedef struct Context_Entry Context_Entry;

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

enum SegmentType {
    BOOLEAN,
    STRING,
    NUMBER,
    LIST_STRING,
};

struct Segment {
    enum SegmentType type;
    union {
        bool boolean;
        char *string;
        int number;
        ListString *list_string;
    } value;
};

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

typedef struct _Engine _Engine;

struct _Engine {
    const char *template;
};

void engine_preprocess(Engine *engine)
{
    _Engine *priv = (_Engine *)(engine + 1);
    // TODO
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
    printf("%s\n", priv->template);

    return engine;
}
