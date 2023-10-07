#include <stdio.h>
#include "tests/test_vector.c"
#include "tests/test_string.c"
#include "tests/test_hashmap.c"

#include "collections/hashmap.h"

int main(int argc, char *argv[])
{
    test_vector_default();
    test_vector_push();

    test_hashmap_default();
    test_string_from_cstr();

    test_hashmap_default();
    test_hashmap_put();
    test_hashmap_with_dummy_hash_cmp();
    test_hashmap_get();

    return 0;
}
