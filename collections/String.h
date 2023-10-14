#ifndef STRING_H
#define STRING_H

#include "../global/types.h"
#include "Vector_generic.h"
#include <stddef.h>

typedef struct String String;
typedef struct Slice Slice;

struct String {
    size_t (*len)(const String *self);
    size_t (*capacity)(const String *self);
    str_t (*as_cstr)(const String *self);
    Slice *(*get_slice)(const String *self, size_t from, size_t to);
    void (*push)(String *self, char c);
    void (*push_str)(String *self, void *str);
    int (*compare)(const String *self, const String *other);
    int (*compare_cstr)(const String *self, const str_t other);
    String *(*replace)(String *self, const str_t from, const str_t to);
    size_t (*find)(const String *self, const str_t substr);
    bool (*starts_with)(const String *self, const str_t other);
    Vector *(*split)(const String *self, const str_t delim);
};

/**
 * The default string.
 */
String *string_default();

/**
 * Create a new string with a capacity.
 *
 * @param capacity The capacity of the string.
 *
 * @return The new string.
 */
String *string_with_capacity(size_t capacity);

/**
 * Create a new string from a C string.
 *
 * @param from The C string to create the string from.
 *
 * @return The new string.
 */
String *string_new_from_cstr(const str_t from);

/**
 * Create a new string from a string.
 *
 * @param from The string to create the string from.
 *
 * @return The new string.
 */
String *string_new_from_string(const String *from);

/**
 * Pushes c string to the end of the string.
 *
 * @param self The string to push to.
 * @param str The c string to push.
 */
void string_push_cstr(String *self, const char* str);

/**
 * Pushes string to the end of the string.
 *
 * @param self The string to push to.
 * @param str The string to push.
 */
void string_push_string(String *self, const String *str);

int slice_find(const Slice *self, const char *from);

String *slice_to_string(const Slice *self);

void slice_free(Slice *self);

/**
 * A default hash function for strings.
 *
 * @param  key The string to hash.
 *
 * @return The hash of the string.
 */
size_t hash_string(const void *key);

/**
 * A default compare function for strings.
 *
 * @param  a The first string.
 * @param  b The second string.
 *
 * @return < 0 if a < b, 0 if a == b, > 0 if a > b.
 */
static inline int cmp_string(const void *a, const void *b)
{
    return ((const String *)a)->compare((const String *)a, (const String *)b);
}


#define string_new(from) \
    _Generic((from), \
        char*: string_new_from_cstr, \
        const char*: string_new_from_cstr, \
        char[]: string_new_from_cstr, \
        const char[]: string_new_from_cstr, \
        String_t: string_new_from_string, \
        const String_t: string_new_from_string, \
    )(from)

#define push_str(self, str) push_str, \
    _Generic((str), \
        char*:          string_push_cstr,   \
        const char*:    string_push_cstr,   \
        String*:       string_push_string,  \
        const String*: string_push_string  \
    )(self, str)

// TODO move to String struct
void print_string(String *s);

void print_slice(Slice *s);

Slice *slice_trim(Slice *self);

int slice_compare_cstr(const Slice *self, const str_t other);

String *string_join(const Vector *strings, const str_t other);

#endif // !STRING_H
