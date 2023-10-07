#include <assert.h>
#include <stdio.h>
#include "../collections/String.h"

void test_string_default(void)
{
    string_t str = string_default();

    assert(string_len(str) == 0);

    string_free(str);

    printf("test_string_default passed\n");
}

void test_string_from_cstr(void)
{
    string_t str = string_from_cstr("hello");

    assert(string_len(str) == 5);

    string_free(str);

    printf("test_string_from_cstr passed\n");
}

void run_string_tests(void)
{
    test_string_default();
    test_string_from_cstr();
}
