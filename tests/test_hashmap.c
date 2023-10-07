#include <assert.h>
#include <stdio.h>
#include "../collections/hashmap.h"
#include <string.h>

void test_hashmap_default(void)
{
    hashmap_t map = hashmap_default();

    assert(hashmap_size(map) == 0);

    hashmap_free(map);

    printf("test_hashmap_default passed\n");
}

void test_hashmap_put(void)
{
    hashmap_t map = hashmap_default();

    const char *key1 = "hello";
    bool put1 = hashmap_put(map, key1, "world");
    assert(put1 == true);
    assert(hashmap_size(map) == 1);
    bool put2 = hashmap_put(map, key1, "world2");
    assert(put2 == true);
    assert(hashmap_size(map) == 1);
    const char *key2 = "hello2";
    bool put3 = hashmap_put(map, key2, "world3");
    assert(put3 == true);
    assert(hashmap_size(map) == 2);

    hashmap_free(map);

    printf("test_hashmap_put passed\n");
}

void test_hashmap_with_dummy_hash_cmp(void)
{
    long unsigned int dummy_hash(const void *key)
    {
        return 0;
    }

    int dummy_cmp(const void *a, const void *b)
    {
        return 0;
    }

    hashmap_t map = hashmap_new(dummy_hash, dummy_cmp);

    const char *key1 = "hello";
    bool put1 = hashmap_put(map, key1, "world");
    assert(put1 == true);
    assert(hashmap_size(map) == 1);
    const char *key2 = "hello2";
    bool put2 = hashmap_put(map, key2, "world2");
    assert(put2 == true);
    assert(hashmap_size(map) == 1);
    const char *key3 = "hello3";
    bool put3 = hashmap_put(map, key3, "world3");
    assert(put3 == true);
    assert(hashmap_size(map) == 1);

    hashmap_free(map);

    printf("test_hashmap_with_dummy_hash_cmp passed\n");
}

void test_hashmap_get(void)
{
    hashmap_t map = hashmap_default();

    const char *key = "hello";
    const char *value = "world";

    bool put1 = hashmap_put(map, key, (void *)value);
    assert(put1 == true);
    assert(hashmap_size(map) == 1);

    const char *get = hashmap_get(map, key);
    assert(strncmp(get, value, 5) == 0);

    hashmap_free(map);

    printf("test_hashmap_get passed\n");
}


void run_hashmap_tests(void)
{
    test_hashmap_default();
    test_hashmap_put();
    test_hashmap_with_dummy_hash_cmp();
    test_hashmap_get();
}
