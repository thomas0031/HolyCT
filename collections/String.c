#include "String.h"
#include "vector.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct string {
    vec_u8_t data;
};

struct slice {
    str_t ptr;
    size_t len;
};

string_t string_default(void)
{
    string_t str = malloc(sizeof(struct string));

    str->data = vec_u8_default();

    return str;
}

string_t string_with_capacity(size_t cap)
{
    string_t str = malloc(sizeof(struct string));

    str->data = vec_u8_with_capacity(cap);

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

size_t string_cap(string_t str)
{
    return vec_u8_cap(str->data);
} 

int string_cmp(string_t a, string_t b)
{
    if (string_len(a) != string_len(b)) {
        return string_len(a) - string_len(b);
    }

    size_t len = string_len(a);
    for (size_t i = 0; i < len; ++i) {
        if (vec_u8_get(a->data, i) != vec_u8_get(b->data, i)) {
            return vec_u8_get(a->data, i) - vec_u8_get(b->data, i);
        }
    }

    return 0;
}

void string_append(string_t str, const str_t cstr)
{
    for (size_t i = 0; cstr[i] != '\0'; ++i) {
        vec_u8_push(str->data, cstr[i]);
    }
}

// TODO: optimize this, currently O(n*m) where n is the length of the string
// and m is the length of the substring
size_t string_find_from(string_t str, const str_t substr, size_t start)
{
    size_t len = string_len(str);
    size_t sub_len = strlen(substr);

    if (start > len - sub_len + 1) return -1;

    for (size_t i = start; i < len; ++i) {
        if (vec_u8_get(str->data, i) == substr[0]) {
            size_t j = 0;
            for (; j < sub_len; ++j) {
                if (vec_u8_get(str->data, i + j) != substr[j]) {
                    break;
                }
            }
            if (j == sub_len) {
                return i;
            }
        }
    }

    return -1;
}

size_t string_find(string_t str, const str_t substr)
{
    return string_find_from(str, substr, 0);
}

slice_t string_slice(string_t str, size_t start, size_t end)
{
    if (start > end || end > string_len(str)) return NULL;

    slice_t slice = malloc(sizeof(struct slice));

    slice->ptr = vec_u8_get_ptr(str->data, start);
    slice->len = end - start;

    return slice;
}

string_t string_replace(string_t str, const str_t from, const str_t to)
{
    size_t len = string_len(str);
    size_t from_len = strlen(from);
    size_t to_len = strlen(to);

    // Count the occurrences of 'from' in 'str'
    int count = 0;
    size_t tmp_from_idx = 0;
    while ((tmp_from_idx = string_find_from(str, from, tmp_from_idx) + 1)) {
        count++;
    }

    // Calculate the new string length
    size_t new_len = len + count * (to_len - from_len);
    
    // Allocate memory for the new string data
    string_t new_str = string_with_capacity(new_len);

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

    string_append(new_str, initial_dst);

    return new_str;
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

str_t string_as_cstr(string_t str)
{
    return vec_u8_get_ptr_raw(str->data, 0);
}
