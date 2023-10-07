#include <stdio.h>
#include "collections/String.h"
#include "global/types.h"
#include "template/context.h"
#include "tests/mod.c"

#include "template/engine.h"

int main(int argc, char *argv[])
{
    engine_t engine = engine_new("templates/index.html");

    context_t ctx = context_new();
    ctx->insert(ctx, "name", "world");
    ctx->print(ctx);

    string_t rendered_page = engine->render(engine, ctx);
    string_print(rendered_page);

    run_all_tests();

    return 0;
}
