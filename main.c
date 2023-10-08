#include <stdio.h>
#include "collections/String.h"
#include "global/types.h"
#include "tests/mod.c"

#include "template/engine.h"

int main(int argc, char *argv[])
{
    run_all_tests();

    engine_t engine = engine_new("templates/index.html");

    context_t ctx = context_new();
    ctx->insert(ctx, "who", "World");
    ctx->insert(ctx, "param", "lol");
    ctx->insert(ctx, "whoo", "das");
    ctx->print(ctx);

    string_t rendered_page = engine->render(engine, ctx);
    string_print(rendered_page);


    return 0;
}
