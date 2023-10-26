#ifndef REGEX_H
#define REGEX_H

#include <stddef.h>

char **match(const char *pattern, const char *text, size_t num_of_vars);

#endif // !REGEX_H
