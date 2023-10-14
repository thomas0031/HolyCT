#include <stdio.h>

#include "collections/HashMap.h"
#include "collections/String.h"
#include "collections/Vector_generic.h"
#include "tests/mod.c"

#include "core/Engine.h"


int main(int argc, char *argv[])
{
    printf("Creating engine...\n");
    Engine *engine = engine_new("templates/index.html");

    printf("Creating context...\n");
    Context *ctx = context_new();
    printf("Inserting values...\n");
    ctx->insert(ctx, string_new_from_cstr("title"), string_new_from_cstr("Title"));
    ctx->insert(ctx, string_new_from_cstr("heading"), string_new_from_cstr("List of items:"));
    Vector *inner_list = vector_default();
    inner_list->push(inner_list, string_new_from_cstr("1"));
    inner_list->push(inner_list, string_new_from_cstr("2"));
    inner_list->push(inner_list, string_new_from_cstr("3"));
    ctx->insert(ctx, string_new_from_cstr("inner_list"), inner_list);
    Vector *outer_list = vector_default();
    outer_list->push(outer_list, string_new_from_cstr("1"));
    outer_list->push(outer_list, string_new_from_cstr("2"));
    outer_list->push(outer_list, string_new_from_cstr("3"));
    ctx->insert(ctx, string_new_from_cstr("outer_list"), outer_list);

    printf("Preprocessing...\n");
    engine->preprocess(engine);
    printf("Optimized rendering...\n");
    String *rendered_page = engine->optimized_render(engine, ctx);
    printf("Rendered page: %s\n", rendered_page->as_cstr(rendered_page));

    return 0;
}
