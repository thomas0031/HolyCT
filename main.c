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
    ctx->insert(ctx, string_new_from_cstr("show_heading"), True);
    ctx->insert(ctx, string_new_from_cstr("heading"), string_new_from_cstr("List of items:"));
    Vector *items = vector_default();
    items->push(items, string_new_from_cstr("Item 1"));
    items->push(items, string_new_from_cstr("Item 2"));
    items->push(items, string_new_from_cstr("Item 3"));
    Vector *outer_loop = vector_default();
    outer_loop->push(outer_loop, string_new_from_cstr("Outer 1"));
    outer_loop->push(outer_loop, string_new_from_cstr("Outer 2"));
    outer_loop->push(outer_loop, string_new_from_cstr("Outer 3"));
    ctx->insert(ctx, string_new_from_cstr("outer_loop"), outer_loop);
    Vector *inner_loop = vector_default();
    inner_loop->push(inner_loop, string_new_from_cstr("Inner 1"));
    inner_loop->push(inner_loop, string_new_from_cstr("Inner 2"));
    inner_loop->push(inner_loop, string_new_from_cstr("Inner 3"));
    ctx->insert(ctx, string_new_from_cstr("inner_loop"), inner_loop);
    ctx->insert(ctx, string_new_from_cstr("items"), items);
    ctx->insert(ctx, string_new_from_cstr("show_footer"), False);
    ctx->insert(ctx, string_new_from_cstr("custom_footer"), string_new_from_cstr("This is a custom footer"));

    printf("Preprocessing...\n");
    engine->preprocess(engine);
    printf("Optimized rendering...\n");
    String *rendered_page = engine->optimized_render(engine, ctx);
    printf("Rendered page: %s\n", rendered_page->as_cstr(rendered_page));

    return 0;
}
