#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "../core/Engine.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

bool eval_condition(const char *expr, const Context *context);

char *str_slice(const char *str, size_t start, size_t end);
char *str_trim(const char *str);
char **str_split(const char *str, const char *delim);

#endif // !UTILS_H
