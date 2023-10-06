#include "string.h"
#include "vector.h"
#include <stdlib.h>
#include <stdio.h>

struct string {
    vec_u8_t data;
};

string_t string_default(void)
{
    string_t str = malloc(sizeof(struct string));

    str->data = vec_u8_default();

    return str;
}

string_t string_from_cstr(const str_t cstr)
{
    string_t str = string_default();

    for (size_t i = 0; cstr[i] != '\0'; ++i) {
        vec_u8_push(str->data, cstr[i]);
    }

    return str;
}

void string_free(string_t str)
{
    vec_u8_free(str->data);
    free(str);

    str = NULL;
}

size_t string_len(string_t str)
{
    return vec_u8_len(str->data);
}

void string_push(string_t str, char item)
{
    vec_u8_push(str->data, item);
}

void string_print(string_t str)
{
    putchar('"');
    for (size_t i = 0; i < string_len(str); ++i) {
        putchar(vec_u8_get(str->data, i));
    }
    putchar('"');
    fflush(stdout);
}
