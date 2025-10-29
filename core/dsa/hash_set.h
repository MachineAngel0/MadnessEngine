#ifndef HASH_SET_H
#define HASH_SET_H

#define HASH_SET_DEFAULT_CAPACITY 100
#include <stdint.h>
#include <time.h>

#include "../misc_util.h"
#include "dsa_utility.h"


//TODO: might be good to create two set of functions one for if the hash is going to own its own memory
//TODO: if performance is an issue look into robin hood addressing


typedef enum hash_set_state
{
    HASH_SET_EMPTY,
    HASH_SET_DELETED,
    HASH_SET_USING,
} hash_set_state;


typedef struct hash_set
{
    uint64_t data_size;
    uint64_t capacity;
    uint64_t num_entries;

    //SOA data
    hash_set_state* state;
    void** data_key;
} hash_set;


hash_set* hash_set_init(uint64_t data_size, uint64_t capacity)
{
    hash_set* h = malloc(sizeof(hash_set));

    h->capacity = capacity;
    h->data_size = data_size;
    h->num_entries = 0;


    h->data_key = malloc(capacity * sizeof(void *));
    memset(h->data_key, 0, capacity * sizeof(void *));

    h->state = malloc(capacity * sizeof(hash_set_state));
    // memset(h->state, HASH_SET_EMPTY, capacity * sizeof(hash_set_state));

    for (uint64_t i = 0; i < capacity; i++)
    {
        h->state[i] = HASH_SET_EMPTY;

        // allocate and zero out per-key memory
        h->data_key[i] = malloc(data_size);
        memset(h->data_key[i], 0, data_size);
    }

    return h;
}


void hash_set_free(hash_set* h)
{
    if (!h)
    {
        WARN("FREEING INVALID HASH SET")
        return;
    }

    for (uint64_t i = 0; i < h->capacity; i++)
    {
        free(h->data_key[i]);
    }

    free(h->state);
    free(h->data_key);
    free(h);
}


void hash_set_insert(hash_set* h, void* key)
{
    //get an index
    uint64_t hash_index = generate_hash_key_64bit(key, h->data_size);
    uint64_t hash_key = hash_index % h->capacity;


    //find the first free spot, we check state, then we modify the data
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        if (h->state[cur_index] == HASH_SET_EMPTY || h->state[cur_index] == HASH_SET_DELETED)
        {
            memcpy(h->data_key[cur_index], key, h->data_size);
            h->state[cur_index] = HASH_SET_USING;
            h->num_entries += 1;
            return;
        }
        //comparing the values, memcmp returns 0 on success
        if (h->state[cur_index] == HASH_SET_USING && memcmp(h->data_key[cur_index], key, h->data_size) == 0)
        {
            return;
        }
    }
}

void hash_set_delete(hash_set* h, void* key)
{
    //get an index
    uint64_t hash_index = generate_hash_key_64bit(key, h->data_size);
    uint64_t hash_key = hash_index % h->capacity;

    //find our value or the first empty
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        //the first empty we hit means the value was never in the hash set
        if (h->state[cur_index] == HASH_SET_EMPTY)
        {
            return;
        }
        //find our key and just mark as deleted
        if (h->data_key[cur_index] == key)
        {
            h->state[cur_index] = HASH_SET_DELETED;
            h->num_entries -= 1;
            return;
        }
    }
}

bool hash_set_contains(const hash_set* h, void* key)
{
    //get an index
    uint64_t hash_index = generate_hash_key_64bit(key, h->data_size);
    uint64_t hash_key = hash_index % h->capacity;

    //if the spot is empty then we can just return false
    if (h->state[hash_key] == HASH_SET_EMPTY)
    {
        return false;
    }

    //find our value or the first empty
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        //0 is false, 1 is true
        if (h->state[cur_index] == HASH_SET_USING && memcmp(h->data_key[cur_index], key, h->data_size) == 0)
        {
            return true;
        }
        if (h->state[cur_index] == HASH_SET_EMPTY)
        {
            return false;
        }
    }

    return false;
}


void hash_set_print(hash_set* h, void (*print_func)(void*))
{
    for (uint64_t i = 0; i < h->capacity; i++)
    {
        //only print the valid items
        if (h->state[i] == HASH_SET_USING)
        {
            print_func(h->data_key[i]);
            printf("\n");
        }
    }
    printf("\n");
}

uint64_t hash_set_get_size(hash_set* h)
{
    return h->num_entries;
}

//there's currently not a need for this since this currently isn't dynamic
void hash_set_rehash(hash_set* h);

void hash_set_copy(hash_set* hash_from, hash_set* hash_to);

hash_set* hash_set_merge(hash_set* h1, hash_set* h2);


void hash_set_test()
{
    hash_set* h = hash_set_init(sizeof(int), HASH_SET_DEFAULT_CAPACITY);
    // hash_set* h = hash_set_init(sizeof(int), HASH_SET_DEFAULT_CAPACITY);

    srand((uint64_t) time(NULL));

    for (uint64_t i = 0; i < HASH_SET_DEFAULT_CAPACITY; i++)
    {
        int key = (rand() % HASH_SET_DEFAULT_CAPACITY);
        // printf("KEY: %d\n", key);

        hash_set_insert(h, &key);
    }

    int x = -1;
    hash_set_insert(h, &x);


    for (uint64_t i = 0; i < 10; i++)
    {
        if (hash_set_contains(h, &i))
        {
            printf("contains %llu\n", i);
        }
    }
    printf("hash set contains # of items: %llu\n", h->num_entries);


    hash_set_print(h, print_int);


    hash_set_free(h);
}


#endif //HASH_SET_H
