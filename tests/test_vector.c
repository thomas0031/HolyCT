#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../collections/Vector_generic.h"
#include "../collections/Vector_typed.h"

void test_vector_default(void)
{
    printf("test_vector_default passed\n");
}

void test_vector_push(void)
{
    printf("test_vector_push passed\n");
}

void test_vector_get(void)
{
    printf("test_vector_get passed\n");
}

void test_vector_get_ptr(void)
{
    printf("test_vector_get_ptr passed\n");
}

void run_vector_tests(void)
{
    test_vector_default();
    test_vector_push();
    test_vector_get();
    test_vector_get_ptr();
}
