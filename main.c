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
    ctx->insert(ctx, string_new_from_cstr("who"), string_new_from_cstr("World"));
    ctx->insert(ctx, string_new_from_cstr("param"), string_new_from_cstr("lol"));
    ctx->insert(ctx, string_new_from_cstr("cringe"), string_new_from_cstr("das"));

    engine->preprocess(engine);
    String *rendered_page = engine->optimized_render(engine, ctx);
    printf("Rendered page: %s\n", rendered_page->as_cstr(rendered_page));
    //clock_t start, end;

    //start = clock();
    //printf("Preprocessing...\n");
    //engine->preprocess(engine);
    //printf("Rendering...\n");
    //for (int i = 0; i < 100000; i++) {
    //    String *optimized_rendered_page = engine->optimized_render(engine, ctx);
    //}
    //end = clock();
    //double optimized_time_taken = ((double) end - start) / CLOCKS_PER_SEC;

    //printf("Optimized time: %f seconds\n", optimized_time_taken);

    return 0;
}
