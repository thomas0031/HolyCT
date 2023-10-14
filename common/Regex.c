#include "Regex.h"

#include <regex.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

str_t *match(const str_t pattern, const str_t text, size_t num_of_vars) {
    regex_t regex;
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        fprintf(stderr, "Failed to compile regex\n");
        return NULL;
    }

    str_t *vars = malloc(num_of_vars * sizeof(str_t));
    regmatch_t *match = malloc(sizeof(regmatch_t) * (num_of_vars + 1)); // +1 for the entire matched string
    if (regexec(&regex, text, num_of_vars + 1, match, 0) == 0) {
        for (size_t i = 0; i < num_of_vars; i++) {
            size_t var_len = match[i + 1].rm_eo - match[i + 1].rm_so + 1;
            vars[i] = malloc(var_len * sizeof(char));
            strncpy(vars[i], text + match[i + 1].rm_so, var_len);
            vars[i][var_len - 1] = '\0';
        }
    } else {
        free(vars);
        vars = NULL;
    }

    regfree(&regex);
    free(match);

    return vars;
}
