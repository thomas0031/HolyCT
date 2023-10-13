#include "Engine.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdlib.h>
#include "../collections/HashMap.h"

typedef struct Context_private Context_private;
typedef struct Segment Segment;

struct Context_private {
    HashMap *map;
};


typedef enum {
    STATIC,
    DYNAMIC,
} SegmentType;

struct Segment {
    SegmentType type;
    String *value;
};

void context_insert(Context *self, String *key, String *value) 
{
    Context_private *data = (Context_private *)(self + 1);

    data->map->put(data->map, key, value);
}

String *context_get(const Context *self, String *key)
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

#define loop while (1)

void engine_preprocess(Engine *self)
{
    engine_private *private = (engine_private*)(self + 1);

    Vector *segments = private->segments;
    segments->clear(segments);

    size_t last_pos = 0;
    loop {
        Slice *slice_from_last_pos = private->template->get_slice(private->template, last_pos, private->template->len(private->template));
        size_t start = slice_find(slice_from_last_pos, "{{");
        slice_free(slice_from_last_pos);
        if (start == -1) break;
        start += last_pos;

        Slice *slice_from_start = private->template->get_slice(private->template, start, private->template->len(private->template));
        size_t end = slice_find(slice_from_start, "}}");
        slice_free(slice_from_start);
        if (end == -1) break;   // TODO return error
        end += 2 + start;

        Slice *key_slice = private->template->get_slice(private->template, start + 2, end - 2);
        String *key = slice_to_string(slice_trim(key_slice));

        Segment *static_segment = malloc(sizeof(Segment));
        static_segment->type = STATIC;

        static_segment->value = slice_to_string(private->template->get_slice(private->template, last_pos, start));
        segments->push(segments, static_segment);
        
        Segment *dynamic_segment = malloc(sizeof(Segment));
        dynamic_segment->type = DYNAMIC;
        dynamic_segment->value = key;
        segments->push(segments, dynamic_segment);

        last_pos = end;
    }
    Segment *static_segment = malloc(sizeof(Segment));
    static_segment->type = STATIC;
    static_segment->value = slice_to_string(private->template->get_slice(private->template, last_pos, private->template->len(private->template)));
    segments->push(segments, static_segment);
}

String *engine_optimized_render(const Engine *self, Context *ctx)
{
    engine_private *private = (engine_private*)(self + 1);

    Vector *segments = private->segments;

    String *rendered = string_default();
    for (size_t i = 0; i < segments->len(segments); ++i) {
        const Segment *segment = segments->get(segments, i);

        switch (segment->type) {
            case DYNAMIC:
                rendered->push_str(rendered, ctx->get(ctx, segment->value));
                break;
            case STATIC:
                rendered->push_str(rendered, segment->value);
                break;
            default:
                // error;
                return NULL;
        }
    }

    return rendered;
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
