#include "hash_table.h"

hash_table* _hash_table_create(u64 value_data_size, u64 capacity)
{
    hash_table* h = malloc(sizeof(hash_table));

    h->capacity = capacity;
    h->value_data_size = value_data_size;

    h->key_str_data = calloc(capacity, sizeof(char*));

    h->value_data = malloc(value_data_size * capacity);
    memset(h->value_data, 0, value_data_size * capacity);


    return h;
}

void hash_table_destroy(hash_table* h)
{
    free(h->key_str_data);
    free(h->value_data);
    free(h);
}

void hash_table_insert(hash_table* h, const char* key_str, void* value)
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

            h->key_str_data[cur_index] = key_str;
            memcpy((u8*)h->value_data + (cur_index * h->value_data_size), value, h->value_data_size);
            h->num_entries += 1;
            return;
        }
    }

    WARN("HASH TABLE STRING INSERT: NO INSERT SPOT, INCREASE SIZE OF HASH TABLE");
}

bool hash_table_remove(hash_table* h, const char* key_str)
{
    MASSERT_MSG(h && key_str, "HASH MAP STRING DELETE: INVALID PARAMS");

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
            h->key_str_data[cur_index] = NULL;
            h->num_entries -= 1;
            return true;
        }
    }

    WARN("HASH MAP STRING REMOVE: COULD NOT FIND STRING");
    return false;
}

bool hash_table_contains(hash_table* h, const char* key_str)
{
    MASSERT_MSG(h, "HASH MAP STRING DELETE: INVALID PARAMS");

    if (!key_str)
    {
        MASSERT_MSG(h, "HASH TABLE CONTAINS: INVALID KEY");
        return false;
    }

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


bool hash_table_get(hash_table* h, const char* key_str, void* out_data)
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
            // FATAL("HASH MAP STRING GET: KEY NOT FOUND");
            return false;
        }
        if (strcmp(h->key_str_data[cur_index], key_str) == 0)
        {
            memcpy(out_data, (u8*)h->value_data + (h->value_data_size * cur_index), h->value_data_size);
            return true;
        }
    }

    // FATAL("HASH MAP STRING GET: KEY NOT FOUND");
    return false;
}


void hash_table_set(hash_table* h, const char* key_str, void* value)
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
            memcpy((u8*)h->value_data + (cur_index * h->value_data_size), value, h->value_data_size);
            return;
        }
    }
    return;
}


void hash_table_print(hash_table* h, void (*print_func_value)(void*))
{
    //NOTE: will not work if we are using pointer types
    for (u64 i = 0; i < h->capacity; i++)
    {
        if (h->key_str_data[i] == NULL)
        {
            continue;
        }

        printf("KEY: %s VALUE: ", h->key_str_data[i]);
        print_func_value((u8*)h->value_data + (i * h->value_data_size));
        printf("\n");
    }
}

u64 hash_table_get_hash_id(hash_table* h, const char* key_str)
{
    u64 string_size = strlen(key_str);
    return generate_hash_key_64bit((u8*)key_str, string_size) % h->capacity;
}


void hash_table_test()
{
    TEST_START("HASH TABLE");
    hash_table* hm_test = HASH_TABLE_CREATE(i32, HT_STRING_TEST_CAPACITY);

    i32 val = 100;
    hash_table_insert(hm_test, "key", &val);
    hash_table_print(hm_test, print_int);

    hash_table_remove(hm_test, "key");
    hash_table_print(hm_test, print_int);


    i32 val2 = 101;
    hash_table_insert(hm_test, "key2", &val2);

    i32 check_value;
    hash_table_get(hm_test, "key2", &check_value);
    TEST_DEBUG(check_value == val2);

    hash_table_set(hm_test, "key2", &val);
    hash_table_get(hm_test, "key2", &check_value);
    TEST_DEBUG(check_value == val);

    hash_table_set(hm_test, "key2", &val2);
    hash_table_get(hm_test, "key2", &check_value);
    TEST_DEBUG(check_value == val2);

    hash_table_print(hm_test, print_int);

    hash_table_destroy(hm_test);

    // PTR TEST//

    hash_table* hm_test_ptr = HASH_TABLE_CREATE(void*, HT_STRING_TEST_CAPACITY);
    i32* val_ptr = malloc(sizeof(int));
    *val_ptr = 100;

    hash_table_insert(hm_test_ptr, "key", &val_ptr);
    hash_table_print(hm_test_ptr, print_int);

    hash_table_remove(hm_test_ptr, "key");
    hash_table_print(hm_test_ptr, print_int);

    i32* val2_ptr = malloc(sizeof(int));
    *val2_ptr = 101;
    hash_table_insert(hm_test_ptr, "key2", &val2_ptr);


    i32* check_value_ptr;
    hash_table_get(hm_test_ptr, "key2", &check_value_ptr);
    TEST_DEBUG(check_value_ptr == val2_ptr);
    TEST_DEBUG(*check_value_ptr == *val2_ptr);

    hash_table_set(hm_test_ptr, "key2", &val_ptr);
    hash_table_get(hm_test_ptr, "key2", &check_value_ptr);
    TEST_DEBUG(check_value_ptr == val_ptr);
    TEST_DEBUG(*check_value_ptr == *val_ptr);

    hash_table_set(hm_test_ptr, "key2", &val2_ptr);
    hash_table_get(hm_test_ptr, "key2", &check_value_ptr);
    TEST_DEBUG(check_value_ptr == val2_ptr);
    TEST_DEBUG(*check_value_ptr == *val2_ptr);

    hash_table_print(hm_test_ptr, print_int);

    hash_table_destroy(hm_test_ptr);


    TEST_END("HASH TABLE");
}
