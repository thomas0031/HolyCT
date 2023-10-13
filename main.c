#include <stdio.h>

#include "collections/HashMap.h"
#include "collections/String.h"
#include "tests/mod.c"

#include "core/Engine.h"

#include <time.h>

int main(int argc, char *argv[])
{
    printf("Running tests...\n");
    run_all_tests();

    printf("Creating engine...\n");
    Engine *engine = engine_new("templates/index.html");

    printf("Creating context...\n");
    Context *ctx = context_new();
    printf("Inserting values...\n");
    ctx->insert(ctx, string_new_from_cstr("title"), context_value_new_string("Title"));
    ctx->insert(ctx, string_new_from_cstr("heading"), context_value_new_string("List of items:"));
    ctx->insert(ctx, string_new_from_cstr("items"), context_value_new_vector("Item 1", "Item 2", "Item 3", NULL));

    engine->preprocess(engine);
    String *rendered_page = engine->optimized_render(engine, ctx);
    printf("Rendered page: %s\n", rendered_page->as_cstr(rendered_page));

    return 0;
}
