#include "Engine.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdlib.h>
#include "../collections/HashMap.h"

typedef struct context_private context_private;

struct context_private {
    hashmap_t map;
};

void context_insert_impl(Context_t self, str_t key, str_t value) 
{
    context_private *data = (context_private *)(self + 1);

    data->map->put(data->map, key, value);
}

void context_print_impl(Context_t self)
{
    context_private *data = (context_private *)(self + 1);

    data->map->print(data->map);
}

str_t context_get_impl(Context_t self, str_t key)
{
    context_private *data = (context_private *)(self + 1);

    return data->map->get(data->map, key);
}

Context_t context_new(void) 
{
    Context_t ctx = malloc(sizeof(Context) + sizeof(context_private));
    if (!ctx) return NULL;

    ctx->insert = context_insert_impl;
    ctx->print = context_print_impl;
    ctx->get = context_get_impl;

    context_private *data = (context_private *)(ctx + 1);
    data->map = hashmap_new(hash_str, cmp_str, print_str);

    return ctx;
}

typedef struct engine_private engine_private;

struct engine_private {
    String_t template;
    Vector_t segments;
};

static String_t engine_render_impl(Engine_t eng, Context_t ctx)
{
    engine_private *private = (engine_private*)(eng + 1);

    // rendered = self.template
    String_t rendered = private->template;

    context_private *data = (context_private *)(ctx + 1);

    hashmap_iterator_t iter = data->map->iter(data->map);
    // for key, value in context.items():
    while (iter->has_next(iter)) {
        String_t prev_rendered = rendered;
        // rendered = rendered.replace("{{" + key + "}}", str(value))
        hashmap_entry_t entry = iter->next(iter);
        String_t string_from = string_from_cstr("{{");
        string_append(string_from, (str_t)entry->key);
        string_append(string_from, "}}");
        const str_t from = string_as_cstr(string_from);
        const str_t to = (str_t)entry->value;
        rendered = string_replace(prev_rendered, from, to);

        string_free(string_from);
        string_free(prev_rendered);
    }

    return rendered;
}

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

void engine_preprocess_impl(Engine_t self)
{
    engine_private *private = (engine_private*)(self + 1);

    Vector_t segments = private->segments;
    segments->clear(segments);

    size_t last_pos = 0;
    while (1) {
        const size_t start = string_find_from(private->template, "{{", last_pos);
        if (start == -1) break;
        const size_t end = string_find_from(private->template, "}}", start) + 2;
        if (end == -1) break;   // TODO return error
        const Slice_t key = string_slice(private->template, start + 2, end - 2);    // TODO may add slice

        Segment_t static_segment = malloc(sizeof(Segment));
        static_segment->type = STATIC;
        static_segment->value = string_slice(private->template, last_pos, start);
        segments->push(segments, static_segment);
        
        Segment_t dynamic_segment = malloc(sizeof(Segment));
        dynamic_segment->type = DYNAMIC;
        dynamic_segment->value = key;
        segments->push(segments, dynamic_segment);

        last_pos = end;
    }
    Segment_t static_segment = malloc(sizeof(Segment));
    static_segment->type = STATIC;
    static_segment->value = string_slice(private->template, last_pos, string_len(private->template));
    segments->push(segments, static_segment);
}

String_t engine_optimized_render_impl(Engine_t self, Context_t ctx)
{
    engine_private *private = (engine_private*)(self + 1);

    Vector_t segments = private->segments;

    String_t rendered = string_default();
    for (size_t i = 0; i < segments->len(segments); ++i) {
        const Segment_t segment = segments->get(segments, i);

        str_t val = string_slice_to_cstr(segment->value);   // TODO fixme non optimized
        switch (segment->type) {
            case DYNAMIC:
                string_append(rendered, ctx->get(ctx, val));
                break;
            case STATIC:
                string_append(rendered, val);
                break;
            default:
                // error;
                return NULL;
        }
    }

    return rendered;
}

Engine_t engine_new(str_t path)
{
    Engine_t eng = malloc(sizeof(Engine) + sizeof(engine_private));
    if (!eng) return NULL;

    eng->render = engine_render_impl;
    eng->preprocess = engine_preprocess_impl;
    eng->optimized_render = engine_optimized_render_impl;

    engine_private *private = (engine_private*)(eng + 1);
    str_t template = read_from_file(path);
    private->template = string_from_cstr(template);
    private->segments = vector_default(NULL);

    return eng;
}
