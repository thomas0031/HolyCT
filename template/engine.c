#include "engine.h"

#include <stdio.h>
#include <stdlib.h>

#include <stdlib.h>
#include "../collections/hashmap.h"

typedef struct context_private {
    hashmap_t map;
} context_private;

void context_insert_impl(context_t self, str_t key, str_t value) 
{
    context_private *data = (context_private *)(self + 1);

    data->map->put(data->map, key, value);
}

void context_print_impl(context_t self)
{
    context_private *data = (context_private *)(self + 1);

    data->map->print(data->map);
}

context_t context_new(void) 
{
    context_t ctx = malloc(sizeof(context) + sizeof(context_private));
    if (!ctx) return NULL;

    ctx->insert = context_insert_impl;
    ctx->print = context_print_impl;

    context_private *data = (context_private *)(ctx + 1);
    data->map = hashmap_new(hash_str, cmp_str, print_str);

    return ctx;
}

typedef struct engine_private {
    string_t template;
} engine_private;

static string_t engine_render_impl(engine *eng, context_t ctx)
{
    engine_private *private = (engine_private*)(eng + 1);

    // rendered = self.template
    string_t rendered = private->template;

    context_private *data = (context_private *)(ctx + 1);

    hashmap_iterator_t iter = data->map->iter(data->map);
    // for key, value in context.items():
    while (iter->has_next(iter)) {
        string_t prev_rendered = rendered;
        // rendered = rendered.replace("{{" + key + "}}", str(value))
        hashmap_entry_t entry = iter->next(iter);
        string_t string_from = string_from_cstr("{{");
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

engine_t engine_new(str_t path)
{
    engine_t eng = malloc(sizeof(engine) + sizeof(engine_private));
    if (!eng) return NULL;

    eng->render = engine_render_impl;

    engine_private *private = (engine_private*)(eng + 1);
    str_t template = read_from_file(path);
    private->template = string_from_cstr(template);

    return eng;
}
