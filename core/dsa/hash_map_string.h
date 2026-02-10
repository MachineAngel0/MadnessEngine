#ifndef HASH_MAP_STRING_H
#define HASH_MAP_STRING_H


#define HM_STRING_DEFAULT_CAPACITY 100
#define HM_STRING_TEST_CAPACITY 100


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


hash_map_string* _hash_map_string_create(u64 value_data_size, u64 capacity);
#define HASH_MAP_STRING_CREATE(type, capacity) _hash_map_string_create(sizeof(type), capacity)
#define HASH_MAP_STRING_CREATE_DEFAULT_SIZE(type) _hash_map_string_create(sizeof(type), HM_STRING_TEST_CAPACITY)


void hash_map_string_destroy(hash_map_string* h);

void hash_map_string_insert(hash_map_string* h, char* key_str, void* value);

void hash_map_string_remove(hash_map_string* h, char* key_str);

bool hash_map_string_contains(hash_map_string* h, char* key_str);

void hash_map_string_set(hash_map_string* h, char* key_str, void* value);

void* hash_map_string_get(hash_map_string* h, char* key_str);

void hash_map_string_print(hash_map_string* h, void (*print_func_value)(void*));


void hash_map_string_test();


#endif //HASH_MAP_STRING_H
