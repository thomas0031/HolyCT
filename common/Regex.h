#ifndef REGEX_H
#define REGEX_H

#include "../collections/String.h"

#include <stddef.h>

str_t *match(const str_t pattern, const str_t text, size_t num_of_vars);

#endif // !REGEX_H
