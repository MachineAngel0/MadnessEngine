#ifndef HASH_MAP_STRING_H
#define HASH_MAP_STRING_H


#define HM_STRING_DEFAULT_CAPACITY 100
#define HM_STRING_TEST_CAPACITY 100

#include "dsa_utility.h"


//NOTE: this is specifically meant for usage with hash maps where the key is a string,
// value pair can be anything except another string
typedef struct hash_map_string
{
    uint64_t value_data_size;
    uint64_t capacity;
    uint64_t num_entries;

    // NOTE: all strings will be null be default
    // TODO: use an arena with sized pools sizes, and allocate from that, or use a string builder
    char** key_str_data;
    void** value_data;
} hash_map_string;


hash_map_string* _hash_map_string_create(u64 value_data_size, u64 capacity)
{
    hash_map_string* h = malloc(sizeof(hash_map_string));

    h->capacity = capacity;
    h->value_data_size = value_data_size;

    h->key_str_data = calloc(capacity, sizeof(char*));
    h->value_data = calloc(capacity, sizeof(void*));


    for (int i = 0; i < h->capacity; i++)
    {
        // h->key_str_data[i] = malloc(HM_STRING_DEFAULT_CAPACITY);
        h->key_str_data[i] = NULL;
        h->value_data[i] = malloc(h->value_data_size);
        memset(h->value_data[i], 0, value_data_size);
    }

    return h;
}

#define HASH_MAP_STRING_CREATE(type, capacity) _hash_map_string_create(sizeof(type), capacity)
#define HASH_MAP_STRING_CREATE_DEFAULT_SIZE(type) _hash_map_string_create(sizeof(type), HM_STRING_TEST_CAPACITY)


void hash_map_string_destroy(hash_map_string* h)
{
    for (int i = 0; i < h->capacity; i++)
    {
        free(h->key_str_data[i]);
        free(h->value_data[i]);
    }

    free(h->key_str_data);
    free(h->value_data);

    free(h);
}

void hash_map_string_insert(hash_map_string* h, char* key_str, void* value)
{
    MASSERT_MSG(h, "HASH MAP STRING INSERT: INVALID PARAMS")
    MASSERT_MSG(key_str, "HASH MAP STRING INSERT: INVALID PARAMS");
    MASSERT_MSG(value, "HASH MAP STRING INSERT: INVALID PARAMS");

    u64 string_size = strlen(key_str);

    uint64_t hash_index = generate_hash_key_64bit((u8*)key_str, string_size);
    uint64_t hash_key = hash_index % h->capacity;

    //find the first free spot
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        if (h->key_str_data[cur_index] == NULL)
        {
            // memcpy(h->key_str_data[cur_index], key_str, string_size);
            h->key_str_data[cur_index] = malloc(HM_STRING_DEFAULT_CAPACITY);
            memcpy(h->key_str_data[cur_index], key_str, string_size);
            h->key_str_data[cur_index][string_size] = '\0';
            memcpy(h->value_data[cur_index], value, h->value_data_size);
            h->num_entries += 1;
            return;
        }
    }

    WARN("HASH MAP STRING INSERT: NO INSERT SPOT");
}

void hash_map_string_remove(hash_map_string* h, char* key_str)
{
    MASSERT_MSG(h && key_str && key_str, "HASH MAP STRING DELETE: INVALID PARAMS");

    u64 string_size = strlen(key_str);

    uint64_t hash_index = generate_hash_key_64bit((u8*)key_str, string_size);
    uint64_t hash_key = hash_index % h->capacity;

    //find the string or an empty
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        if (h->key_str_data[cur_index] == NULL)
        {
            WARN("HASH MAP STRING REMOVE: COULD NOT FIND STRING");
            return;
        }
        if (strcmp(h->key_str_data[cur_index], key_str) == 0)
        {
            free(h->key_str_data[cur_index]);
            h->key_str_data[cur_index] = NULL;
            h->num_entries -= 1;
            return;
        }
    }

    WARN("HASH MAP STRING REMOVE: COULD NOT FIND STRING");
}

bool hash_map_string_contains(hash_map_string* h, char* key_str)
{
    MASSERT_MSG(h, "HASH MAP STRING DELETE: INVALID PARAMS");

    u64 string_size = strlen(key_str);

    uint64_t hash_index = generate_hash_key_64bit((u8*)key_str, string_size);
    uint64_t hash_key = hash_index % h->capacity;

    //find the string or an empty
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        if (h->key_str_data[cur_index] == NULL)
        {
            WARN("HASH MAP STRING REMOVE: COULD NOT FIND STRING");
            return false;
        }
        if (strcmp(h->key_str_data[cur_index], key_str) == 0)
        {
            return true;
        }
    }
    return false;
}

void hash_map_string_set(hash_map_string* h, char* key_str, void* value)
{
    MASSERT_MSG(h && key_str && value, "HASH MAP STRING SET: INVALID PARAMS");

    u64 string_size = strlen(key_str);

    uint64_t hash_index = generate_hash_key_64bit((u8*)key_str, string_size);
    uint64_t hash_key = hash_index % h->capacity;

    //find the string or if it's a NULL then report
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        if (h->key_str_data[cur_index] == NULL)
        {
            WARN("HASH MAP STRING SET: COULD NOT FIND KEY: %s", key_str);
            return;
        }
        if (strcmp(h->key_str_data[cur_index], key_str) == 0)
        {
            memcpy(h->value_data[cur_index], value, h->value_data_size);
            return;
        }
    }
    return;
}

void* hash_map_string_get(hash_map_string* h, char* key_str)
{
    MASSERT_MSG(h && key_str, "HASH MAP STRING SET: INVALID PARAMS");

    u64 string_size = strlen(key_str);

    uint64_t hash_index = generate_hash_key_64bit((u8*)key_str, string_size);
    uint64_t hash_key = hash_index % h->capacity;

    //find the string or if it's a NULL then report
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        if (h->key_str_data[cur_index] == NULL)
        {
            FATAL("HASH MAP STRING GET: KEY NOT FOUND");
            return NULL;
        }
        if (strcmp(h->key_str_data[cur_index], key_str) == 0)
        {
            return h->value_data[cur_index];
        }
    }

    FATAL("HASH MAP STRING GET: KEY NOT FOUND");
    return NULL;
}

void hash_map_string_print(hash_map_string* h, void (*print_func_value)(void*))
{
    DEBUG("HASH MAP STRING DEBUG PRINT");
    for (u64 i = 0; i < h->capacity; i++)
    {
        if (h->key_str_data[i] == NULL)
        {
            continue;
        }

        printf("KEY: %s VALUE: ", h->key_str_data[i]);
        print_func_value(h->value_data[i]);
        printf("\n");
    }
}


void hash_map_string_test()
{
    TEST_START("HASH MAP STRING");
    hash_map_string* hm_test = HASH_MAP_STRING_CREATE(i32, HM_STRING_TEST_CAPACITY);

    i32 val = 100;
    hash_map_string_insert(hm_test, "key", &val);

    hash_map_string_remove(hm_test, "key");


    i32 val2 = 101;
    hash_map_string_insert(hm_test, "key2", &val2);

    i32* check_value;
    check_value = (i32*)hash_map_string_get(hm_test, "key2");
    TEST_INFORM(*check_value == val2);

    hash_map_string_set(hm_test, "key2", &val);
    check_value = (i32*)hash_map_string_get(hm_test, "key2");
    TEST_INFORM(*check_value == val);

    hash_map_string_set(hm_test, "key2", &val2);
    check_value = (i32*)hash_map_string_get(hm_test, "key2");
    TEST_INFORM(*check_value == val2);

    hash_map_string_print(hm_test, print_int);

    hash_map_string_destroy(hm_test);
    TEST_END("HASH MAP STRING");
}


#endif //HASH_MAP_STRING_H
