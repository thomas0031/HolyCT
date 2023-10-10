#include <stdio.h>

#include "collections/String.h"
#include "tests/mod.c"

#include "core/Engine.h"

#include <time.h>

int main(int argc, char *argv[])
{
    run_all_tests();

    Engine_t engine = engine_new("templates/index.html");

    Context_t ctx = context_new();
    ctx->insert(ctx, "who", "World");
    ctx->insert(ctx, "param", "lol");
    ctx->insert(ctx, "whoo", "das");


    // String_t rendered_page = engine->render(engine, ctx);
    // engine->preprocess(engine);
    // String_t optimized_rendered_page = engine->optimized_render(engine, ctx);
    clock_t start, end;

    start = clock();
    for (int i = 0; i < 100000; i++) {
        String_t rendered_page = engine->render(engine, ctx);
    }
    end = clock();
    double non_optimized_time_taken = ((double) end - start) / CLOCKS_PER_SEC;
    
    start = clock();
    engine->preprocess(engine);
    for (int i = 0; i < 100000; i++) {
        String_t optimized_rendered_page = engine->optimized_render(engine, ctx);
    }
    end = clock();
    double optimized_time_taken = ((double) end - start) / CLOCKS_PER_SEC;

    printf("Non optimized time: %f seconds\n", non_optimized_time_taken);
    printf("Optimized time: %f seconds\n", optimized_time_taken);

    return 0;
}
