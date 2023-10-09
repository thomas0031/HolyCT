#include "String.h"
#include "Vector_typed.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct String {
    Vector_u8_t data;
};

struct Slice {
    str_t ptr;
    size_t len;
};

String_t string_default(void)
{
    String_t str = malloc(sizeof(String));

    str->data = vector_u8_default();

    return str;
}

String_t string_with_capacity(size_t cap)
{
    String_t str = malloc(sizeof(String));

    str->data = vector_u8_with_capacity(cap);

    return str;
}

String_t string_from_cstr(const str_t cstr)
{
    String_t str = string_default();

    for (size_t i = 0; cstr[i] != '\0'; ++i) {
        str->data->push(str->data, cstr[i]);
    }

    return str;
}

void string_free(String_t str)
{
    vector_u8_free(str->data);
    free(str);

    str = NULL;
}

size_t string_len(String_t str)
{
    return str->data->len(str->data);
}

size_t string_cap(String_t str)
{
    return str->data->cap(str->data);
} 

int string_cmp(String_t a, String_t b)
{
    if (string_len(a) != string_len(b)) {
        return string_len(a) - string_len(b);
    }

    size_t len = string_len(a);
    for (size_t i = 0; i < len; ++i) {
        u8 a_byte = a->data->get(a->data, i);
        u8 b_byte = b->data->get(b->data, i);
        if (a_byte != b_byte) {
            return a_byte - b_byte;
        }
    }

    return 0;
}

void string_append(String_t str, const str_t cstr)
{
    for (size_t i = 0; cstr[i] != '\0'; ++i) {
        str->data->push(str->data, cstr[i]);
    }
}

// TODO: optimize this, currently O(n*m) where n is the length of the string
// and m is the length of the substring
size_t string_find_from(String_t str, const str_t substr, size_t start)
{
    size_t len = string_len(str);
    size_t sub_len = strlen(substr);

    if (start > len - sub_len + 1) return -1;

    for (size_t i = start; i < len; ++i) {
        if (str->data->get(str->data, i) == substr[0]) {
            size_t j = 0;
            for (; j < sub_len; ++j) {
                if (str->data->get(str->data, i + j) != substr[j]) break;
            }
            if (j == sub_len) return i;
        }
    }

    return -1;
}

size_t string_find(String_t str, const str_t substr)
{
    return string_find_from(str, substr, 0);
}

Slice_t string_slice(String_t str, size_t start, size_t end)
{
    if (start > end || end > string_len(str)) return NULL;

    Slice_t slice = malloc(sizeof(Slice));

    slice->ptr = str->data->get_ptr(str->data, start);  // TODO check
    slice->len = end - start;

    return slice;
}

String_t string_replace(String_t str, const str_t from, const str_t to)
{
    size_t len = string_len(str);
    size_t from_len = strlen(from);
    size_t to_len = strlen(to);

    // Count the occurrences of 'from' in 'str'
    int count = 0;
    size_t tmp_from_idx = 0;
    while ((tmp_from_idx = string_find_from(str, from, tmp_from_idx) + 1)) count++;

    // Calculate the new string length
    size_t new_len = len + count * (to_len - from_len);
    
    // Allocate memory for the new string data
    String_t new_str = string_with_capacity(new_len);

    tmp_from_idx = 0;
    str_t src = string_as_cstr(str);
    str_t dst = string_as_cstr(new_str);
    str_t initial_dst = dst;

    // Replace 'from' with 'to' in the new string
    while (src) {
        str_t match = strstr(src, from);
        if (match) {
            size_t len_before_match = match - src;
            strncpy(dst, src, len_before_match);  // Copy characters before the match
            dst += len_before_match;
            strcpy(dst, to);  // Copy 'to' string
            dst += strlen(to);
            src += len_before_match + strlen(from);
        } else {
            strcpy(dst, src);
            break;
        }
    }

    string_append(new_str, initial_dst);    // FAIL TOOT

    return new_str;
}

void string_print(String_t str)
{
    putchar('"');
    for (size_t i = 0; i < string_len(str); ++i) {
        putchar(str->data->get(str->data, i));
    }
    putchar('"');
}

str_t string_as_cstr(String_t str)
{
    return str->data->get_ptr_raw(str->data, 0);
}
