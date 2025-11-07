#ifndef HASH_MAP_H
#define HASH_MAP_H


#define HASH_MAP_DEFAULT_CAPACITY 100
#include <time.h>

#include "dsa_utility.h"

//NOTE: rn im just using open addressing,
// until i have the need for larger data sets, then I will implement a closed addressing hash

typedef enum hash_map_state
{
    HASH_MAP_EMPTY,
    HASH_MAP_DELETED,
    HASH_MAP_USING,
} hash_map_state;


typedef struct hash_map
{
    uint64_t key_data_size;
    uint64_t value_data_size;
    uint64_t capacity;
    uint64_t num_entries;

    //SOA
    hash_map_state* state;
    void** key_data;
    void** value_data;
} hash_map;


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
        h->value_data[i] = malloc( h->value_data_size);
        memset(h->key_data[i], 0, key_data_size);
        memset(h->value_data[i], 0, value_data_size);
    }

    return h;
}

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
        if (h->state[cur_index] == HASH_MAP_USING && memcmp(h->key_data[cur_index],key, h->key_data_size) == 0)
        {
            return;
        }
    }
}

void hash_map_delete(hash_map* h, void* key)
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


void hash_table_copy(hash_map* h1, hash_map* h2);
hash_map* hash_table_merge(hash_map* h1, hash_map* h2); // should pass back a new hash table


void hash_map_test()
{
    hash_map* h = hash_map_init(sizeof(int), sizeof(int), HASH_MAP_DEFAULT_CAPACITY);
    // hash_map* h = hash_map_init(sizeof(int), HASH_MAP_DEFAULT_CAPACITY);

    srand((uint64_t)time(NULL));

    for (uint64_t i = 0; i < HASH_MAP_DEFAULT_CAPACITY; i++)
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
}

#endif //HASH_MAP_H
