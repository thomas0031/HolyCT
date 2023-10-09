#include <assert.h>
#include <stdio.h>
#include "../collections/String.h"

void test_string_default(void)
{
    String_t str = string_default();

    assert(string_len(str) == 0);

    string_free(str);

    printf("test_string_default passed\n");
}

void test_string_with_capacity(void)
{
    String_t str = string_with_capacity(10);

    assert(string_len(str) == 0);
    assert(string_cap(str) == 10);

    string_free(str);

    printf("test_string_with_capacity passed\n");
}

void test_string_from_cstr(void)
{
    String_t str = string_from_cstr("hello");

    assert(string_len(str) == 5);

    string_free(str);

    printf("test_string_from_cstr passed\n");
}

void test_string_cmp(void)
{
    String_t str1 = string_from_cstr("hello");
    String_t str2 = string_from_cstr("hello");

    assert(string_cmp(str1, str2) == 0);
    
    string_free(str1);
    
    printf("test_string_cmp passed\n");
}

void test_string_append(void)
{
    String_t str = string_default();

    string_append(str, "h");
    string_append(str, "e");
    string_append(str, "l");
    string_append(str, "l");
    string_append(str, "o");

    assert(string_len(str) == 5);
    assert(string_cmp(str, string_from_cstr("hello")) == 0);

    string_free(str);

    printf("test_string_push passed\n");
}

void test_string_find(void)
{
    String_t str = string_from_cstr("hello");

    assert(string_find(str, "h") == 0);
    assert(string_find(str, "e") == 1);
    assert(string_find(str, "l") == 2);
    assert(string_find(str, "o") == 4);

    string_free(str);

    printf("test_string_find passed\n");
}

void test_string_replace(void)
{
    String_t str = string_from_cstr("foo bar foo bar");
    String_t expected = string_from_cstr("foo 42 foo 42");

    String_t actual = string_replace(str, "bar", "42");
    
    assert(string_cmp(actual, expected) == 0);

    string_free(str);
    string_free(expected);
    string_free(actual);

    printf("test_string_replace passed\n");
}

void test_string_find_from(void)
{
    String_t str = string_from_cstr("hello");

    assert(string_find_from(str, "h", 0) == 0);
    assert(string_find_from(str, "e", 0) == 1);
    assert(string_find_from(str, "l", 0) == 2);
    assert(string_find_from(str, "o", 0) == 4);

    assert(string_find_from(str, "l", 3) == 3);

    assert(string_find_from(str, "h", 1) == -1);
    assert(string_find_from(str, "e", 2) == -1);
    assert(string_find_from(str, "l", 4) == -1);
    assert(string_find_from(str, "o", 5) == -1);

    string_free(str);

    printf("test_string_find_from passed\n");
}

void run_string_tests(void)
{
    test_string_default();
    test_string_with_capacity();
    test_string_from_cstr();
    test_string_append();
    test_string_cmp();
    test_string_find();
    test_string_find_from();
    test_string_replace();
}
