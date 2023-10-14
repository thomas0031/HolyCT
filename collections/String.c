#include "String.h"
#include "Vector_generic.h"
#include "Vector_typed.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_STRING_CAPACITY 16

typedef struct String_private String_private;

struct String_private {
    Vector_u8_t data;
};

struct Slice {
    str_t ptr;
    size_t len;
};

size_t hash_string(const void *key)
{
    const String *str = key;
    String_private *priv = (String_private *)(str + 1);

    size_t hash = 5381;
    for (size_t i = 0; i < priv->data->len(priv->data); ++i) {
        hash = ((hash << 5) + hash) + priv->data->get(priv->data, i);
    }

    return hash;
}

void string_push_cstr(String *self, const char* str)
{
    String_private *priv = (String_private *)(self + 1);

    for (size_t i = 0; str[i] != '\0'; ++i) {
        priv->data->push(priv->data, str[i]);
    }
}

void string_push_string(String *self, const String *str)
{
    String_private *priv = (String_private *)(self + 1);
    String_private *str_priv = (String_private *)(str + 1);

    for (size_t i = 0; i < str_priv->data->len(str_priv->data); ++i) {
        priv->data->push(priv->data, str_priv->data->get(str_priv->data, i));
    }
}

void string_free(String *str)
{
    String_private *priv = (String_private *)(str + 1);

    vector_u8_free(priv->data);
    free(str);

    str = NULL;
}

size_t string_len(const String *str)
{
    String_private *priv = (String_private *)(str + 1);

    return priv->data->len(priv->data);
}

size_t string_capacity(const String *str)
{
    String_private *priv = (String_private *)(str + 1);

    return priv->data->cap(priv->data);
} 

void string_push(String *str, char c)
{
    String_private *priv = (String_private *)(str + 1);

    priv->data->push(priv->data, c);
}

int string_compare(const String *self, const String *other)
{
    size_t self_len = string_len(self);
    size_t other_len = string_len(other);

    if (self_len != other_len) return self_len - other_len;

    String_private *self_priv = (String_private *)(self + 1);
    String_private *other_priv = (String_private *)(other + 1);

    for (size_t i = 0; i < self_len; ++i) {
        if (self_priv->data->get(self_priv->data, i) != other_priv->data->get(other_priv->data, i)) {
            return self_priv->data->get(self_priv->data, i) - other_priv->data->get(other_priv->data, i);
        }
    }

    return 0;
}

int string_compare_cstr(const String *self, const str_t other)
{
    size_t self_len = string_len(self);
    size_t other_len = strlen(other);

    if (self_len != other_len) return self_len - other_len;

    String_private *self_priv = (String_private *)(self + 1);

    for (size_t i = 0; i < self_len; ++i) {
        if (self_priv->data->get(self_priv->data, i) != other[i]) {
            return self_priv->data->get(self_priv->data, i) - other[i];
        }
    }

    return 0;
}

str_t string_as_cstr(const String *str)
{
    String_private *priv = (String_private *)(str + 1);

    return (str_t)priv->data->get_ptr(priv->data, 0);
}

Slice *string_get_slice(const String *str, size_t start, size_t end)
{
    if (start > end || end > string_len(str)) return NULL;

    Slice *slice = malloc(sizeof(Slice));
    if (!slice) return NULL;

    String_private *priv = (String_private *)(str + 1);

    slice->len = end - start;
    slice->ptr = (str_t)priv->data->get_ptr(priv->data, start);

    return slice;
}

// TODO: optimize this, currently O(n*m) where n is the length of the string
// and m is the length of the substring
size_t string_find(const String *self, const str_t substr)
{
    String_private *str = (String_private *)(self + 1);

    size_t len = str->data->len(str->data);
    size_t sub_len = strlen(substr);

    for (size_t i = 0; i < len; ++i) {
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

bool string_starts_with(const String *self, const str_t substr)
{
    String_private *str = (String_private *)(self + 1);

    size_t len = str->data->len(str->data);
    size_t sub_len = strlen(substr);

    if (sub_len > len) return false;

    for (size_t i = 0; i < sub_len; ++i) {
        if (str->data->get(str->data, i) != substr[i]) return false;
    }

    return true;
}

Vector *string_split(const String *self, const str_t delim)
{
    String_private *private = (String_private *)(self + 1);

    Vector *vec = vector_default();

    size_t len = private->data->len(private->data);
    size_t delim_len = strlen(delim);

    size_t start = 0;
    for (size_t i = 0; i < len; ++i) {
        if (private->data->get(private->data, i) == delim[0]) {
            size_t j = 0;
            for (; j < delim_len; ++j) {
                if (private->data->get(private->data, i + j) != delim[j]) break;
            }
            if (j == delim_len) {
                Slice *slice = string_get_slice(self, start, i);
                vec->push(vec, slice_to_string(slice));
                slice_free(slice);
                start = i + delim_len;
            }
        }
    }

    Slice *slice = string_get_slice(self, start, len);

    vec->push(vec, slice_to_string(slice));
    slice_free(slice);

    return vec;
}

int slice_find(const Slice *self, const char *substr)
{
    size_t len = self->len;
    size_t sub_len = strlen(substr);

    for (size_t i = 0; i < len; ++i) {
        if (self->ptr[i] == substr[0]) {
            size_t j = 0;
            for (; j < sub_len; ++j) {
                if (self->ptr[i + j] != substr[j]) break;
            }
            if (j == sub_len) return i;
        }
    }

    return -1;
}

String *slice_to_string(const Slice *self)
{
    String *str = string_default();
    if (!str) return NULL;

    String_private *priv = (String_private *)(str + 1);

    for (size_t i = 0; i < self->len; ++i) {
        priv->data->push(priv->data, self->ptr[i]);
    }

    return str;
}

void slice_free(Slice *self)
{
    free(self);
    self = NULL;
}

String *string_replace(String *self, const str_t from, const str_t to)
{
    size_t len = string_len(self);
    size_t from_len = strlen(from);
    size_t to_len = strlen(to);

    // Count the occurrences of 'from' in 'str'
    int count = 0;
    size_t tmp_from_idx = 0;
    while (1) {
        Slice *slice = string_get_slice(self, tmp_from_idx, len - tmp_from_idx);
        int slice_from_idx = slice_find(slice, from);
        if (slice_from_idx == -1) break;
        tmp_from_idx += slice_from_idx;
        slice_free(slice);
    }

    // Calculate the new string length
    size_t new_len = len + count * (to_len - from_len);
    
    // Allocate memory for the new string data
    String *new_str = string_with_capacity(new_len);

    tmp_from_idx = 0;
    str_t src = string_as_cstr(self);
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

    new_str->push_str(new_str, initial_dst);

    return new_str;
}

String *string_default(void)
{
    return string_with_capacity(DEFAULT_STRING_CAPACITY);
}

String *string_with_capacity(size_t capacity)
{
    String *str = malloc(sizeof(String) + sizeof(String_private));
    if (!str) return NULL;
    str->len = string_len;
    str->capacity = string_capacity;
    str->push = string_push;
    str->compare = string_compare;
    str->compare_cstr = string_compare_cstr;
    str->as_cstr = string_as_cstr;
    str->get_slice = string_get_slice;
    str->replace = string_replace;
    str->find = string_find;
    str->starts_with = string_starts_with;
    str->split = string_split;

    String_private *priv = (String_private *)(str + 1);
    priv->data = vector_u8_with_capacity(capacity);

    return str;
}

String *string_new_from_cstr(const str_t from)
{
    String *str = string_default();
    if (!str) return NULL;

    String_private *priv = (String_private *)(str + 1);

    for (size_t i = 0; from[i] != '\0'; ++i) {
        priv->data->push(priv->data, from[i]);
    }

    return str;
}

String *string_new_from_string(const String *from)
{
    String *str = string_default();
    if (!str) return NULL;

    String_private *priv = (String_private *)(str + 1);
    String_private *from_priv = (String_private *)(from + 1);

    for (size_t i = 0; i < from_priv->data->len(from_priv->data); ++i) {
        priv->data->push(priv->data, from_priv->data->get(from_priv->data, i));
    }

    return str;
}

void print_string(String *s)
{
    String_private *private = (String_private *)(s + 1);

    for (size_t i = 0; i < private->data->len(private->data); ++i) {
        printf("%c", private->data->get(private->data, i));
    }

    printf("\n");
}

Slice *slice_trim(Slice *self)
{
    while (self->len > 0 && (self->ptr[0] == ' ' || self->ptr[0] == '\n')) {
        self->ptr++;
        self->len--;
    }

    while (self->len > 0 && (self->ptr[self->len - 1] == ' ' || self->ptr[self->len - 1] == '\n')) {
        self->len--;
    }

    return self;
}

int slice_compare_cstr(const Slice *self, const str_t other)
{
    size_t len = strlen(other);

    if (self->len != len) return false;

    for (size_t i = 0; i < len; ++i) {
        if (self->ptr[i] != other[i]) return self->ptr[i] - other[i];
    }

    return 0;
}

String *string_join(const Vector *strings, const str_t delim)
{
    String *str = string_default();
    if (!str) return NULL;

    String_private *priv = (String_private *)(str + 1);

    for (size_t i = 0; i < strings->len(strings); ++i) {
        String *s = strings->get(strings, i);
        String_private *s_priv = (String_private *)(s + 1);
        for (size_t j = 0; j < s_priv->data->len(s_priv->data); ++j) {
            priv->data->push(priv->data, s_priv->data->get(s_priv->data, j));
        }
        if (i != strings->len(strings) - 1) {
            for (size_t j = 0; delim[j] != '\0'; ++j) {
                priv->data->push(priv->data, delim[j]);
            }
        }
    }

    return str;
}

void print_slice(Slice *s)
{
    for (size_t i = 0; i < s->len; ++i) {
        printf("%c", s->ptr[i]);
    }

    printf("\n");
}
