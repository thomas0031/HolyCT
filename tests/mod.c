#include "test_vector.c"
#include "test_string.c"
#include "test_hashmap.c"

void run_all_tests(void)
{
    run_vector_tests();
    run_string_tests();
    run_hashmap_tests();
}
