#include <stdio.h>

#include "collections/HashMap.h"
#include "collections/String.h"
#include "collections/Vector_generic.h"
#include "common/utils.h"
#include "tests/mod.c"

#include "core/Engine.h"

int main(int argc, char *argv[])
{
    //str_t int_literal_cmp_eq = "5 == 5";
    //printf("%s: %d\n", int_literal_cmp_eq, eval_condition(int_literal_cmp_eq, NULL));
    //str_t int_literal_cmp_neq = "5 != 5";
    //printf("%s: %d\n", int_literal_cmp_neq, eval_condition(int_literal_cmp_neq, NULL));
    //str_t int_literal_cmp_gt = "5 > 5";
    //printf("%s: %d\n", int_literal_cmp_gt, eval_condition(int_literal_cmp_gt, NULL));
    //str_t int_literal_cmp_lt = "5 < 5";
    //printf("%s: %d\n", int_literal_cmp_lt, eval_condition(int_literal_cmp_lt, NULL));
    //str_t int_literal_cmp_gte = "5 >= 5";
    //printf("%s: %d\n", int_literal_cmp_gte, eval_condition(int_literal_cmp_gte, NULL));
    //str_t int_literal_cmp_lte = "5 <= 5";
    //printf("%s: %d\n", int_literal_cmp_lte, eval_condition(int_literal_cmp_lte, NULL));
    //str_t int_literal_cmp_and = "5 && 5";
    //printf("%s: %d\n", int_literal_cmp_and, eval_condition(int_literal_cmp_and, NULL));
    //str_t int_literal_cmp_or = "5 || 5";
    //printf("%s: %d\n", int_literal_cmp_or, eval_condition(int_literal_cmp_or, NULL));
    //str_t int_literal_cmp_paren = "(5 == 5)";
    //printf("%s: %d\n", int_literal_cmp_paren, eval_condition(int_literal_cmp_paren, NULL));
    //str_t int_literal_cmp_paren2 = "((5 == 5) && (5 == 5))";
    //printf("%s: %d\n", int_literal_cmp_paren2, eval_condition(int_literal_cmp_paren2, NULL));
    //str_t int_literal_cmp_paren3 = "(5 == 5) && ((5 == 5) || (5 == 5))";
    //printf("%s: %d\n", int_literal_cmp_paren3, eval_condition(int_literal_cmp_paren3, NULL));
    //str_t int_sum_cmp = "5 + 5 == 10";
    //printf("%s: %d\n", int_sum_cmp, eval_condition(int_sum_cmp, NULL));
    //str_t int_literal_ident = "a == 5";
    //Context *ctx = context_new();
    //ctx->insert(ctx, string_new_from_cstr("a"), string_new_from_cstr("5"));
    //printf("%s: %d\n", int_literal_ident, eval_condition(int_literal_ident, ctx));
    
    printf("Creating engine...\n");
    Engine *engine = engine_new("templates/all.ct");

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
    ctx->insert(ctx, string_new_from_cstr("show_footer"), string_new_from_cstr("false"));
    ctx->insert(ctx, string_new_from_cstr("custom_footer"), string_new_from_cstr("This is a custom footer"));
    ctx->insert(ctx, string_new_from_cstr("count"), string_new_from_cstr("5"));

    printf("Preprocessing...\n");
    engine->preprocess(engine);
    printf("Optimized rendering...\n");
    String *rendered_page = engine->optimized_render(engine, ctx);
    printf("Rendered page: %s\n", rendered_page->as_cstr(rendered_page));
    // save to file 
    FILE *fp = fopen("output.html", "w");
    fprintf(fp, "%s", rendered_page->as_cstr(rendered_page));

    return 0;
}
