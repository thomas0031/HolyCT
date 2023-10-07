#include <assert.h>
#include <stdio.h>
#include "../collections/vector.h"

void test_vector_default(void)
{
    vec_u8_t vec = vec_u8_default();

    assert(vec_u8_len(vec) == 0);
    assert(vec_u8_cap(vec) == 8);

    vec_u8_free(vec);

    printf("test_vector_default passed\n");
}

void test_vector_push(void)
{
    vec_u8_t vec = vec_u8_default();

    for (size_t i = 0; i < 9; ++i) {
        vec_u8_push(vec, i);
    }

    assert(vec_u8_len(vec) == 9);
    assert(vec_u8_cap(vec) == 16);

    vec_u8_free(vec);

    printf("test_vector_push passed\n");
}

void run_vector_tests(void)
{
    test_vector_default();
    test_vector_push();
}

