#ifndef STRING_H
#define STRING_H

#include "../global/types.h"
#include <stddef.h>

typedef struct string string;
typedef string* string_t;
typedef struct slice slice;
typedef slice* slice_t;

string_t string_default(void);
string_t string_with_capacity(size_t capacity);
string_t string_from_cstr(const str_t cstr);
void string_free(string_t str);
size_t string_len(string_t str);
size_t string_cap(string_t str);
int string_cmp(string_t a, string_t b);
void string_append(string_t str, const str_t item);
/**
 * Returns the first index of the given substring in the given string
 * starting from the given index.
 *
 * @param  str    The string to search in.
 * @param  substr The substring to search for.
 * @param  start  The index to start searching from.
 *
 * @return The first index of the substring in the string, or -1 if the
 *        substring is not in the string.
 */
size_t string_find_from(string_t str, const str_t substr, size_t start);
/**
 * Returns the first index of the given substring in the given string
 *
 * @param  str    The string to search in.
 * @param  substr The substring to search for.
 *
 * @return The first index of the substring in the string, or -1 if the
 *        substring is not in the string.
 */
size_t string_find(string_t str, const str_t substr);
/**
 * Returns slice of the given string from start to end.
 *
 * @param  str   The string to slice.
 * @param  start The start index of the slice.
 * @param  end   The end index of the slice.
 *
 * @return The slice of the string.
 */
slice_t string_slice(string_t str, size_t start, size_t end);
string_t string_replace(string_t str, const str_t from, const str_t to);
void string_print(string_t str);
str_t string_as_cstr(string_t str);

#endif // !STRING_H
