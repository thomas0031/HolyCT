#include "engine.h"
#include "context.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct engine_private {
    string_t template;
} engine_private;

static string_t engine_render_impl(engine *eng, context_t ctx)
{
    engine_private *private = (engine_private*)(eng + 1);

    string_t rendered = string_default();
    //rendered = self.template
    //for key, value in context.items():
    //    rendered = rendered.replace("{{ " + key + " }}", str(value))

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
