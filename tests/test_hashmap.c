#include <assert.h>
#include <stdio.h>
#include "../collections/HashMap.h"
#include <string.h>

void test_hashmap_default(void)
{
    hashmap_t map = hashmap_default();

    assert(map->size(map) == 0);

    hashmap_free(map);

    printf("test_hashmap_default passed\n");
}

void test_hashmap_put(void)
{
    hashmap_t map = hashmap_default();

    const char *key1 = "hello";
    bool put1 = map->put(map, key1, "world");
    assert(put1 == true);
    assert(map->size(map) == 1);
    bool put2 = map->put(map, key1, "world2");
    assert(put2 == true);
    assert(map->size(map) == 1);
    const char *key2 = "hello2";
    bool put3 = map->put(map, key2, "world3");
    assert(put3 == true);
    assert(map->size(map) == 2);

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

    hashmap_t map = hashmap_new(dummy_hash, dummy_cmp, NULL);

    const char *key1 = "hello";
    bool put1 = map->put(map, key1, "world");
    assert(put1 == true);
    assert(map->size(map) == 1);
    const char *key2 = "hello2";
    bool put2 = map->put(map, key2, "world2");
    assert(put2 == true);
    assert(map->size(map) == 1);
    const char *key3 = "hello3";
    bool put3 = map->put(map, key3, "world3");
    assert(put3 == true);
    assert(map->size(map) == 1);

    hashmap_free(map);

    printf("test_hashmap_with_dummy_hash_cmp passed\n");
}

void test_hashmap_get(void)
{
    hashmap_t map = hashmap_default();

    const char *key = "hello";
    char *value = "world";

    bool put1 = map->put(map, key, value);
    assert(put1 == true);
    assert(map->size(map) == 1);

    const char *get = map->get(map, key);
    assert(strncmp(get, value, 5) == 0);

    hashmap_free(map);

    printf("test_hashmap_get passed\n");
}

void test_hashmap_iterator(void)
{
    hashmap_t map = hashmap_default();

    map->put(map, "hello", "world");
    map->put(map, "hello2", "world2");
    map->put(map, "hello3", "world3");
    map->put(map, "hello4", "world4");
    map->put(map, "hello5", "world5");
    map->put(map, "hello6", "world6");

    hashmap_iterator_t iter = map->iter(map);
    const size_t expected = 6;
    size_t actual = 0;

    while (iter->has_next(iter)) {
        iter->next(iter);
        actual++;
    }

    assert(expected == actual);

    hashmap_free(map);
}


void run_hashmap_tests(void)
{
    test_hashmap_default();
    test_hashmap_put();
    test_hashmap_with_dummy_hash_cmp();
    test_hashmap_get();
    test_hashmap_iterator();
}
