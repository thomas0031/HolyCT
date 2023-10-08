#ifndef STRING_H
#define STRING_H

#include "../global/types.h"
#include <stddef.h>

typedef struct string string;
typedef string* string_t;
typedef struct slice slice;
typedef slice* slice_t;

/**
 * Creates a new string with the default capacity.
 *
 * @return The new string.
 */
string_t string_default(void);

/**
 * Creates a new string with the given capacity.
 *
 * @param  capacity The capacity of the string.
 *
 * @return The new string.
 */
string_t string_with_capacity(size_t capacity);

/**
 * Creates a new string from the given c string.
 *
 * @param  cstr The c string to create the string from.
 *
 * @return The new string.
 */
string_t string_from_cstr(const str_t cstr);

/**
 * Creates a new string from the given string.
 *
 * @param  str The string to create the string from.
 *
 * @return The new string.
 */
void string_free(string_t str);

/**
 * Returns the length of the string.
 *
 * @param  str The string.
 *
 * @return The length of the string.
 */
size_t string_len(string_t str);

/**
 * Returns the capacity of the string.
 *
 * @param  str The string.
 *
 * @return The capacity of the string.
 */
size_t string_cap(string_t str);

/**
 * Returns the item at the given index.
 *
 * @param  str   The string.
 * @param  index The index of the item.
 *
 * @return The item at the given index.
 */
int string_cmp(string_t a, string_t b);

/**
 * Returns the item at the given index.
 *
 * @param  str   The string.
 * @param  index The index of the item.
 *
 * @return The item at the given index.
 */
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

/**
 * Returns the string with the given substring replaced with the given
 * string.
 *
 * @param  str The string to replace in.
 * @param  from The substring to replace.
 * @param  to The string to replace with.
 *
 * @return The string with the substring replaced.
 */
string_t string_replace(string_t str, const str_t from, const str_t to);

/**
 * Returns the string with the given substring replaced with the given
 * string.
 *
 * @param  str The string to replace in.
 * @param  from The substring to replace.
 * @param  to The string to replace with.
 *
 * @return The string with the substring replaced.
 */
void string_print(string_t str);

/**
 * Returns the string as a c string.
 *
 * @param  str The string.
 *
 * @return The string as a c string.
 */
str_t string_as_cstr(string_t str);

#endif // !STRING_H
