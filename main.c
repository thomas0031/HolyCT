#include <stdio.h>
#include "tests/test_vector.c"
#include "tests/test_string.c"

int main(int argc, char *argv[])
{
    test_vector_default();
    test_vector_push();

    test_string_default();
    test_string_from_cstr();

    return 0;
}
