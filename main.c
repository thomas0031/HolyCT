#include <stdio.h>

#include "tests/mod.c"

#include "core/Engine.h"

int main(int argc, char *argv[])
{
    run_all_tests();

    Engine_t engine = engine_new("templates/index.html");

    Context_t ctx = context_new();
    ctx->insert(ctx, "who", "World");
    ctx->insert(ctx, "param", "lol");
    ctx->insert(ctx, "whoo", "das");

    String_t rendered_page = engine->render(engine, ctx);
    string_print(rendered_page);

    return 0;
}
