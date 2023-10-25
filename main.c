#include <stdio.h>

#include "core/Engine.h"

int main(int argc, char *argv[])
{
    printf("Creating engine...\n");
    Engine *engine = engine_new("templates/all.ct");

    printf("Creating context...\n");
    Context *ctx = context_new();
    printf("Inserting values...\n");
    ctx->insert(ctx, "title", "Title");
    ctx->insert(ctx, "show_heading", True);
    ctx->insert(ctx, "heading", "List of items:");
    
    ListString items = create_list_string("Item 1", "Item 2", "Item 3", NULL);
    ctx->insert(ctx, "items", items);
    
    ListString outer_loop = create_list_string("Outer 1", "Outer 2", "Outer 3", NULL);
    ctx->insert(ctx, "outer_loop", outer_loop);
    
    ListString inner_loop = create_list_string("Inner 1", "Inner 2", "Inner 3", NULL);
    ctx->insert(ctx, "inner_loop", inner_loop);
    
    ctx->insert(ctx, "show_footer", False);
    ctx->insert(ctx, "custom_footer", "This is a custom footer");
    ctx->insert(ctx, "count", 5);

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
