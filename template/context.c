#include "context.h"

#include <stdlib.h>
#include "../collections/hashmap.h"

struct context_data {
    hashmap_t map;
};

void context_insert_impl(struct context *self, str_t key, str_t value) 
{
    struct context_data *data = (struct context_data *)(self + 1);

    data->map->put(data->map, key, value);
}

void context_print_impl(struct context *self)
{
    struct context_data *data = (struct context_data *)(self + 1);

    data->map->print(data->map);
}

context_t context_new(void) 
{
    context_t ctx = malloc(sizeof(context) + sizeof(struct context_data));
    if (!ctx) return NULL;

    ctx->insert = context_insert_impl;
    ctx->print = context_print_impl;

    struct context_data *data = (struct context_data *)(ctx + 1);
    data->map = hashmap_new(hash_str, cmp_str, print_str);

    return ctx;
}

// Any other functions as required...

