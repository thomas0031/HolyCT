#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "../core/Engine.h"

bool eval_condition(const char *expr, const Context *context);

char *str_slice(const char *str, size_t start, size_t end);
char *str_trim(const char *str);
char **str_split(const char *str, const char *delim);
char *str_join(const Vector *strings, const char *delim);
char *str_itoa(int num);
size_t str_arr_size(char **arr);

#endif // !UTILS_H
