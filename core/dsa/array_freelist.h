#ifndef FREELIST_ARRAY_H
#define FREELIST_ARRAY_H


typedef struct Freelist_Array_Handle
{
    u32 handle;
    u32 gen;
} Freelist_Array_Handle;

typedef struct Freelist_Array_Node
{
    Freelist_Array_Handle* next;
} Freelist_Array_Node;

//fixed sized free list array, so no reallocating more space
typedef struct Freelist_Array
{
    Array* array;

    u32* free_list;
    u32 count;

    u32* handle;
    u32* gen;
} Freelist_Array;

#define Free_List_ARRAY_TYPE(type) Freelist_Array

//TODO: use the allocator interface
Freelist_Array* _freelist_array_create(u64 data_stride, u64 capacity, Allocator* allocator);


void fl_array_destroy(Freelist_Array* array);


void* _fl_array_internal_get(Freelist_Array* array, u32 index);


Freelist_Array_Handle fl_array_new(Freelist_Array* array);

void* _fl_array_query(Freelist_Array* array, Freelist_Array_Handle handle);


void fl_array_release(Freelist_Array* array, const Freelist_Array_Handle handle);


/*
bool fl_array_serialize(Freelist_Array* array, FILE* fptr);
bool fl_array_deserialize(Freelist_Array* array, FILE* fptr);
*/

#define freelist_array_create(type, capacity, allocator)\
    _freelist_array_create(sizeof(type), capacity, allocator)

#define fl_array_query(array, type, free_list_handle)\
    *(type*) _fl_array_query(array, free_list_handle)





void free_list_array_test();


#endif //FREELIST_ARRAY_H
