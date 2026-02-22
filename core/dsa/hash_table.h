#ifndef HASH_TABLE_H
#define HASH_TABLE_H


#define HM_STRING_DEFAULT_CAPACITY 100
#define HM_STRING_TEST_CAPACITY 100


//NOTE: this is specifically meant for usage with hash maps where the key is a string,
// value pair can be anything except another string
typedef struct hash_table
{
    uint64_t value_data_size;
    uint64_t capacity;
    uint64_t num_entries;

    // NOTE: all strings will be null by default
    // TODO: use an arena with sized pools sizes, and allocate from that
    char** key_str_data;
    void** value_data;
} hash_table;


hash_table* _hash_table_string_create(u64 value_data_size, u64 capacity);
#define HASH_MAP_STRING_CREATE(type, capacity) _hash_table_string_create(sizeof(type), capacity)
#define HASH_MAP_STRING_CREATE_DEFAULT_SIZE(type) _hash_table_string_create(sizeof(type), HM_STRING_TEST_CAPACITY)


void hash_table_destroy(hash_table* h);

void hash_table_insert(hash_table* h, const char* key_str, void* value);

void hash_table_remove(hash_table* h, const char* key_str);

bool hash_table_contains(hash_table* h, const char* key_str);

void hash_table_set(hash_table* h, const char* key_str, void* value);

void* hash_table_get(hash_table* h, const char* key_str);

void hash_table_print(hash_table* h, void (*print_func_value)(void*));


void hash_table_test();


#endif //HASH_MAP_STRING_H
