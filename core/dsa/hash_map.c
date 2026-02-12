#include "hash_map.h"

#include <time.h>


hash_map* hash_map_init(uint64_t key_data_size, uint64_t value_data_size, uint64_t capacity)
{
    hash_map* h = malloc(sizeof(hash_map));

    h->capacity = capacity;
    h->key_data_size = key_data_size;
    h->value_data_size = value_data_size;


    h->key_data = malloc(capacity * sizeof(void*));
    memset(h->key_data, 0, capacity * sizeof(void*));
    h->value_data = malloc(capacity * sizeof(void*));
    memset(h->value_data, 0, capacity * sizeof(void*));

    h->state = malloc(capacity * sizeof(hash_map_state));
    memset(h->state, 0, capacity * sizeof(hash_map_state));

    for (int i = 0; i < h->capacity; i++)
    {
        h->state[i] = HASH_MAP_EMPTY;
        h->key_data[i] = malloc(h->key_data_size);
        h->value_data[i] = malloc(h->value_data_size);
        memset(h->key_data[i], 0, key_data_size);
        memset(h->value_data[i], 0, value_data_size);
    }

    return h;
}

#define HASH_MAP_CREATE(key_type, value_type, capacity)\
         hash_map_init(sizeof(key_type), sizeof(value_type), capacity)


//this version will have capacity will be increased until it is a power of two
// then we can do this with our generated hash index
// hash_key = hash_index & (h->capacity-1); // if our size was a power of two
// printf("%u\n", hash_index); // debug info
// hash_map* hash_map_init_power_of_two(uint64_t data_size, uint64_t capacity)
// {
//     hash_map* h = malloc(sizeof(hash_map));
//
//     //change it into a power of two
//     uint64_t capacity_power_two = next_power_of_two_u32(capacity);
//
//     h->data = malloc(capacity_power_two * sizeof(void *));
//     memset(h->data, 0, capacity_power_two * sizeof(void *));
//
//     h->capacity = capacity_power_two;
//     h->data_size = data_size;
//     return h;
// }


void hash_map_free(hash_map* h)
{
    if (!h)
    {
        WARN("FREEING INVALID HASH MAP")
        return;
    }

    for (uint64_t i = 0; i < h->capacity; i++)
    {
        free(h->key_data[i]);
        free(h->value_data[i]);
    }

    free(h->state);
    free(h->key_data);
    free(h->value_data);
    free(h);
}


void hash_map_insert(hash_map* h, void* key, void* value)
{
    uint64_t hash_index = generate_hash_key_64bit(key, h->key_data_size);
    uint64_t hash_key = hash_index % h->capacity;

    //find the first free spot
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        if (h->state[cur_index] == HASH_MAP_DELETED || h->state[cur_index] == HASH_MAP_EMPTY)
        {
            //add key, value in
            memcpy(h->key_data[cur_index], key, h->key_data_size);
            memcpy(h->value_data[cur_index], value, h->value_data_size);
            h->state[cur_index] = HASH_MAP_USING;
            h->num_entries += 1;
            return;
        }

        //check for duplicates
        if (h->state[cur_index] == HASH_MAP_USING && memcmp(h->key_data[cur_index], key, h->key_data_size) == 0)
        {
            return;
        }
    }
}

void hash_map_remove(hash_map* h, void* key)
{
    //get an index
    uint64_t hash_index = generate_hash_key_64bit(key, h->key_data_size);
    uint64_t hash_key = hash_index % h->capacity;

    //find our value or the first empty
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        //the first empty we hit means the value was never in the hash set
        if (h->state[cur_index] == HASH_MAP_EMPTY)
        {
            return;
        }
        //find our key and just mark as deleted
        if (h->state[cur_index] == HASH_MAP_USING && memcmp(h->key_data[cur_index], key, h->key_data_size) == 0)
        {
            h->state[cur_index] = HASH_MAP_DELETED;
            h->num_entries -= 1;
            return;
        }
    }
}

void* hash_map_get(hash_map* h, void* key)
{
    uint64_t hash_index = generate_hash_key_64bit(key, h->key_data_size);
    uint64_t hash_key = hash_index % h->capacity;

    //find our value or the first empty
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        //the first empty we hit means the value was never in the hash set
        if (h->state[cur_index] == HASH_MAP_EMPTY)
        {
            return NULL;
        }
        //find our key and just mark as deleted
        if (h->state[cur_index] == HASH_MAP_USING && memcmp(h->key_data[cur_index], key, h->key_data_size) == 0)
        {
            return h->value_data[cur_index];
        }
    }
    return NULL;
}

bool hash_map_contains(hash_map* h, void* key)
{
    uint64_t hash_index = generate_hash_key_64bit(key, h->key_data_size);
    uint64_t hash_key = hash_index % h->capacity;

    //find our value or the first empty
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        //the first empty we hit means the value was never in the hash set
        if (h->state[cur_index] == HASH_MAP_EMPTY)
        {
            return false;
        }
        //find our key and just mark as deleted
        if (h->state[cur_index] == HASH_MAP_USING && memcmp(h->key_data[cur_index], key, h->key_data_size) == 0)
        {
            return true;
        }
    }
    //false if we ever reach this point for some reason
    return false;
}


void hash_map_rehash(hash_map* h);

void hash_map_print(hash_map* h, void (*print_func_key)(void*), void (*print_func_value)(void*))
{
    for (uint64_t i = 0; i < h->capacity; i++)
    {
        //only print the valid items
        if (h->state[i] == HASH_MAP_USING)
        {
            printf("KEY:");
            print_func_key(h->key_data[i]);
            printf("VALUE:");
            print_func_value(h->value_data[i]);
            printf("\n");
        }
    }
    printf("\n");
}

uint64_t hash_map_size(hash_map* h)
{
    return h->capacity;
}


void hash_map_test(void)
{
    TEST_START(HASH_MAP);
    hash_map* h = hash_map_init(sizeof(int), sizeof(int), HASH_MAP_DEFAULT_CAPACITY);
    // hash_map* h = hash_map_init(sizeof(int), HASH_MAP_DEFAULT_CAPACITY);

    srand((u64)time(NULL));

    for (u64 i = 0; i < HASH_MAP_DEFAULT_CAPACITY; i++)
    {
        int key = (int)(rand() % HASH_MAP_DEFAULT_CAPACITY);
        int val = (int)(rand() % HASH_MAP_DEFAULT_CAPACITY);

        hash_map_insert(h, &key, &val);
    }

    for (uint64_t i = 0; i < 10; i++)
    {
        if (hash_map_contains(h, &i))
        {
            printf("contains %llu\n", i);
        }
    }


    hash_map_print(h, print_int, print_int);


    hash_map_free(h);

    struct hash_test_key
    {
        int a;
        bool c;
    };
    struct hash_test_value
    {
        u16 a;
        u64 c;
    };

    hash_map* hash_2 = hash_map_init(sizeof(struct hash_test_key), sizeof(struct hash_test_value),
                                     HASH_MAP_DEFAULT_CAPACITY);

    struct hash_test_key hi = {10, true};
    struct hash_test_key hi1 = {12, false};
    struct hash_test_key hi2 = {9, false};
    struct hash_test_key hi3 = {654654, true};

    struct hash_test_value bye = {13, 123};
    struct hash_test_value bye1 = {64, 54};
    struct hash_test_value bye2 = {1981, 777};


    hash_map_insert(h, &hi, &bye);
    hash_map_insert(h, &hi1, &bye1);
    hash_map_insert(h, &hi2, &bye2);


    if (hash_map_contains(h, &hi))
    {
        printf("contains hi 0\n");
    };
    if (hash_map_contains(h, &hi1))
    {
        printf("contains hi 1\n");
    };
    if (hash_map_contains(h, &hi2))
    {
        printf("contains hi 2\n");
    };
    if (hash_map_contains(h, &hi3))
    {
        //should not contain
        printf("contains hi 3\n");
    }
    else
    {
        printf("test passed does not contain hi 3\n");
    }

    free(hash_2);


    hash_map* hash_char = hash_map_init(sizeof(char), sizeof(int),
                                        HASH_MAP_DEFAULT_CAPACITY);
    const char* char1 = "iahsdoi";
    const char* char2 = "whats up";
    const char* char3 = "down and out";
    const char* char10 = "otherwise";
    int val1 = 10;
    int val2 = 184;
    int val3 = 97;

    hash_map_insert(hash_char, &char1, &val1);
    hash_map_insert(hash_char, &char2, &val2);
    hash_map_insert(hash_char, &char3, &val3);

    if (hash_map_contains(hash_char, &char1))
    {
        printf("contains char 1\n");
    }
    if (hash_map_contains(hash_char, &char2))
    {
        printf("contains char 2\n");
    }
    if (hash_map_contains(hash_char, &char3))
    {
        printf("contains char 3\n");
    }

    if (hash_map_contains(hash_char, &char10))
    {
        printf("TEST FAILED contains char 10\n");
    }
    else
    {
        printf("TEST PASSED does not contain char 10\n");
    }


    TEST_REPORT(HASH_MAP);
}
