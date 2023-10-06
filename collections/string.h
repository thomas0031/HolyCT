#ifndef STRING_H
#define STRING_H

#include "../global/types.h"
#include <stddef.h>

typedef struct string string;
typedef string* string_t;

string_t string_default(void);
string_t string_from_cstr(const str_t cstr);
void string_free(string_t str);
size_t string_len(string_t str);
void string_push(string_t str, char item);
void string_print(string_t str);

#endif // !STRING_H
