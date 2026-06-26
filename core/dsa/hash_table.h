#ifndef HASH_TABLE_H
#define HASH_TABLE_H


#define HASH_TABLE_DEFAULT_CAPACITY 100
#include "dsa_utility.h"
#include "str.h"


typedef struct hash_table
{
    uint64_t value_data_size;
    uint64_t capacity;
    uint64_t num_entries;

    //NOTE: this is specifically meant for usage with hash maps where the key is a string, value pair can be anything
    //NOTE: if the type is a pointer data is not stored, otherwise it is

    // TODO: use an arena with sized pools sizes, and allocate from that
    const char** key_str_data;
    void* value_data;
} hash_table;

#define HASH_TABLE_TYPE(type) hash_table

hash_table* _hash_table_create(u64 value_data_size, u64 capacity);
#define HASH_TABLE_CREATE(type, capacity) _hash_table_create(sizeof(type), capacity)
#define HASH_TABLE_CREATE_DEFAULT_SIZE(type) _hash_table_create(sizeof(type), HASH_TABLE_DEFAULT_CAPACITY)

void hash_table_destroy(hash_table* h);

bool hash_table_contains(hash_table* h, const char* key_str);
bool hash_table_contains_st(hash_table* h, String key_str);

void hash_table_insert(hash_table* h, const char* key_str, void* value);
bool hash_table_remove(hash_table* h, const char* key_str);

bool hash_table_get(hash_table* h, const char* key_str, void* out_data);
void hash_table_set(hash_table* h, const char* key_str, void* value);


void hash_table_print(hash_table* h, void (*print_func_value)(void*));
u64 hash_table_get_hash_id(hash_table* h, const char* key_str);


void hash_table_test();


typedef struct hash_table_string
{
    u64 capacity;
    u32 value_data_size;
    u32 num_entries;

    //NOTE: this is specifically meant for usage with hash maps where the key is a string, value pair can be anything
    //NOTE: if the type is a pointer data is not stored, otherwise it is
    String* key_data;
    void* value_data;

    Allocator* allocator;
    Heap_Allocator* free_list_allocator;
} hash_table_string;

#define HASH_TABLE_STR_TYPE(type) hash_table_string


hash_table_string* hash_table_string_create(u64 data_stride, u64 capacity, Allocator* allocator,
                                            Heap_Allocator* free_list_allocator)
{
    if (allocator && free_list_allocator)
    {
        MASSERT_MSG(false, "CANNOT HAVE BOTH TYPES IN USE");
    }

    MASSERT(allocator || free_list_allocator);


    hash_table_string* hts = {0};

    if (allocator)
    {
        hts = allocator_alloc(allocator, sizeof(hash_table_string));
        hts->key_data = allocator_alloc(allocator, sizeof(String) * capacity);
        hts->value_data = allocator_alloc(allocator, capacity * data_stride);

        hts->allocator = allocator;
    }

    if (free_list_allocator)
    {
        hts = allocator_heap_alloc(free_list_allocator, sizeof(hash_table_string));
        hts->key_data = allocator_heap_alloc(free_list_allocator, sizeof(String) * capacity);
        hts->value_data = allocator_heap_alloc(free_list_allocator, capacity * data_stride);

        hts->free_list_allocator = free_list_allocator;
    }


    hts->capacity = capacity;
    hts->value_data_size = data_stride;

    memset(hts->key_data, 0, sizeof(String) * data_stride);
    memset(hts->value_data, 0, data_stride * capacity);

    return hts;
}

void hash_table_string_destroy(hash_table_string* h)
{
    if (h->free_list_allocator)
    {
        allocator_heap_free(h->free_list_allocator, h->key_data);
        allocator_heap_free(h->free_list_allocator, h->value_data);
        allocator_heap_free(h->free_list_allocator, h);
    }
}


void hash_table_str_resize(hash_table_string* h, u64 new_capacity)
{
    if (!h->free_list_allocator) { return; }

    void* new_key_data = allocator_heap_alloc(h->free_list_allocator, sizeof(String*) * new_capacity);
    void* new_value_data = allocator_heap_alloc(h->free_list_allocator, sizeof(hash_table_string) * new_capacity);

    allocator_heap_free(h->free_list_allocator, h->key_data);
    allocator_heap_free(h->free_list_allocator, h->value_data);

    h->capacity = new_capacity;

    h->key_data = new_key_data;
    h->value_data = new_value_data;
}


bool hash_table_string_contains(hash_table_string* h, String key_str)
{
    MASSERT_MSG(h, "HASH TABLE CONTAINS: INVALID PARAMS");

    if (!key_str.chars)
    {
        MASSERT_MSG(h, "HASH TABLE CONTAINS: INVALID KEY");
        return false;
    }

    uint64_t hash_index = generate_hash_key_64bit((u8*)key_str.chars, key_str.length);
    uint64_t hash_key = hash_index % h->capacity;

    //find the string or an empty
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        if (h->key_data[cur_index].length == 0)
        {
            WARN("HASH MAP STRING CONTAINS: COULD NOT FIND STRING");
            return false;
        }
        if (string_compare(&h->key_data[cur_index], &key_str))
        {
            return true;
        }
    }
    return false;
}

void hash_table_string_insert(hash_table_string* h, String key_str, void* value)
{
    MASSERT_MSG(h, "HASH MAP STRING INSERT: INVALID PARAMS")
    MASSERT_MSG(value, "HASH MAP STRING INSERT: INVALID PARAMS");


    uint64_t hash_index = generate_hash_key_64bit((u8*)key_str.chars, key_str.length);
    uint64_t hash_key = hash_index % h->capacity;

    //find the first free spot
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        if (h->key_data[cur_index].length == 0)
        {
            // memcpy(h->key_str_data[cur_index], key_str, string_size);

            h->key_data[cur_index] = key_str;
            memcpy((u8*)h->value_data + (cur_index * h->value_data_size), value, h->value_data_size);
            h->num_entries += 1;
            return;
        }
    }

    WARN("HASH TABLE STRING INSERT: NO INSERT SPOT, INCREASE SIZE OF HASH TABLE");
}

bool hash_table_string_remove(hash_table_string* h, String key_str)
{
    MASSERT_MSG(h && key_str.chars, "HASH MAP STRING DELETE: INVALID PARAMS");

    uint64_t hash_index = generate_hash_key_64bit((u8*)key_str.chars, key_str.length);
    uint64_t hash_key = hash_index % h->capacity;

    //find the string or an empty
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        if (h->key_data + cur_index == NULL)
        {
            WARN("HASH TABLE STRING REMOVE: COULD NOT FIND STRING");
            return false;
        }
        if (string_compare(&h->key_data[cur_index], &key_str))
        {
            h->key_data[cur_index].length = 0;
            h->num_entries -= 1;
            return true;
        }
    }

    WARN("HASH TABLE REMOVE: COULD NOT FIND STRING");
    return false;
}

bool hash_table_string_get(hash_table_string* h, String key_str, void* out_data)
{
    MASSERT_MSG(h && key_str.chars, "HASH TABLE STRING SET: INVALID PARAMS");


    uint64_t hash_index = generate_hash_key_64bit((u8*)key_str.chars, key_str.length);
    uint64_t hash_key = hash_index % h->capacity;

    //find the string or if it's a NULL then report
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        if (h->key_data[cur_index].length == 0)
        {
            // FATAL("HASH MAP STRING GET: KEY NOT FOUND");
            return false;
        }
        if (string_compare(&h->key_data[cur_index], &key_str))
        {
            memcpy(out_data, (u8*)h->value_data + (h->value_data_size * cur_index), h->value_data_size);
            return true;
        }
    }

    // FATAL("HASH MAP STRING GET: KEY NOT FOUND");
    return false;
}

void hash_table_string_set(hash_table_string* h, String key_str, void* value)
{
    MASSERT_MSG(h && key_str.chars && value, "HASH TABLE STRING SET: INVALID PARAMS");


    uint64_t hash_index = generate_hash_key_64bit((u8*)key_str.chars, key_str.length);
    uint64_t hash_key = hash_index % h->capacity;

    //find the string or if it's a NULL then report
    uint64_t cur_index = hash_key;
    for (; cur_index < h->capacity; cur_index++)
    {
        if (h->key_data[cur_index].length == 0)
        {
            WARN("HASH TABLE STRING SET: COULD NOT FIND KEY: %s", key_str);
            return;
        }
        if (string_compare(&h->key_data[cur_index], &key_str))
        {
            memcpy((u8*)h->value_data + (cur_index * h->value_data_size), value, h->value_data_size);
            return;
        }
    }
    return;
}


void hash_table_string_print(hash_table_string* h, void (*print_func_value)(void*))
{
    //NOTE: will not work if we are using pointer types
    for (u64 i = 0; i < h->capacity; i++)
    {
        if (h->key_data[i].length == 0)
        {
            continue;
        }

        string_print(&h->key_data[i]);
        print_func_value((u8*)h->value_data + (i * h->value_data_size));
        printf("\n");
    }
}


#endif //HASH_TABLE_H
