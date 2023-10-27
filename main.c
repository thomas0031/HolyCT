#include <stdio.h>

#include "collections/VectorGeneric.h"
#include "core/Engine.h"

int main(int argc, char *argv[])
{
    printf("Creating engine...\n");
    Engine *engine = engine_new("templates/all.ct");

    printf("Creating context...\n");
    Context *ctx = context_new();

    printf("Inserting values...\n");
    ctx->insert(ctx, "title", "Title");
    ctx->insert(ctx, "show_heading", &True);
    ctx->insert(ctx, "heading", "List of items:");
    
    Vector *items = vector_default();
    items->push(items, "Item 1");
    items->push(items, "Item 2");
    items->push(items, "Item 3");
    ctx->insert(ctx, "items", items);
    
    Vector *outer_loop = vector_default();
    outer_loop->push(outer_loop, "Outer 1");
    outer_loop->push(outer_loop, "Outer 2");
    outer_loop->push(outer_loop, "Outer 3");
    ctx->insert(ctx, "outer_loop", outer_loop);
    
    Vector *inner_loop = vector_default();
    inner_loop->push(inner_loop, "Inner 1");
    inner_loop->push(inner_loop, "Inner 2");
    inner_loop->push(inner_loop, "Inner 3");
    ctx->insert(ctx, "inner_loop", inner_loop);
    
    ctx->insert(ctx, "show_footer", &False);
    ctx->insert(ctx, "custom_footer", "This is a custom footer");
    ctx->insert(ctx, "count", "5");

    printf("Preprocessing...\n");
    engine->preprocess(engine);

    printf("Optimized rendering...\n");
    const char *rendered_page = engine->render(engine, ctx);
    printf("Rendered page: %s\n", rendered_page);

    // save to file 
    FILE *fp = fopen("output.html", "w");
    fprintf(fp, "%s", rendered_page);

    return 0;
}
