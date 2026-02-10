#ifndef HASH_SET_H
#define HASH_SET_H

#define HASH_SET_DEFAULT_CAPACITY 100
#include <stdint.h>


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


hash_set* hash_set_init(uint64_t data_size, uint64_t capacity);


void hash_set_free(hash_set* h);


bool hash_set_insert(hash_set* h, void* key);


void hash_set_delete(hash_set* h, void* key);


bool hash_set_contains(const hash_set* h, void* key);


i32 hash_set_contains_index(const hash_set* h, void* key);



void hash_set_print(hash_set* h, void (*print_func)(void*));


uint64_t hash_set_get_size(hash_set* h);

//there's currently not a need for this since this currently isn't dynamic
void hash_set_rehash(hash_set* h);

void hash_set_copy(hash_set* hash_from, hash_set* hash_to);

hash_set* hash_set_merge(hash_set* h1, hash_set* h2);


void hash_set_test(void);



#endif //HASH_SET_H
