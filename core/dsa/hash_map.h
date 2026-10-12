#ifndef HASH_MAP_H
#define HASH_MAP_H


#define HASH_MAP_DEFAULT_CAPACITY 100


//NOTE: rn im just using open addressing, its generally faster in most cases,
// my hash map isn't very fast so room for optimization



typedef enum Hash_Map_State
{
    HASH_MAP_EMPTY,
    HASH_MAP_DELETED,
    HASH_MAP_USING,
} Hash_Map_State;



#define HASH_MAP_TYPE(key, value) Hash_Map

typedef struct Hash_Map
{
    u64 key_data_size;
    u64 value_data_size;
    u64 capacity;
    u64 num_entries;

    //SOA
    Hash_Map_State* state;
    void** key_data;
    void** value_data;
} Hash_Map;


Hash_Map* hash_map_init(uint64_t key_data_size, uint64_t value_data_size, uint64_t capacity, Allocator* allocator);

#define HASH_MAP_CREATE(key_type, value_type, capacity, allocator)\
         hash_map_init(sizeof(key_type), sizeof(value_type), capacity, allocator)


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

void hash_map_free(Hash_Map* h);

void hash_map_insert(Hash_Map* h, void* key, void* value);

void hash_map_remove(Hash_Map* h, void* key);

bool hash_map_get(Hash_Map* h, void* key, void* out_data);

bool hash_map_contains(Hash_Map* h, void* key);

void hash_map_rehash(Hash_Map* h);

void hash_map_print(Hash_Map* h, void (*print_func_key)(void*), void (*print_func_value)(void*));
uint64_t hash_map_size(Hash_Map* h);

//TODO:
// void hash_table_copy(hash_map* h1, hash_map* h2);
// hash_map* hash_table_merge(hash_map* h1, hash_map* h2); // should pass back a new hash table

void hash_map_test(void);

#endif //HASH_MAP_H
