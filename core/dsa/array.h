#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))


//VOID* VERSION


//fixed sized array, so no reallocating more space
typedef struct Array
{
    u64 capacity; // size of the array
    u32 stride; // size/stride of each void* data
    u32 num_items; // current/top index in our array
    void* data; //array of void* data

    // Free_List_Allocator* allocator_fl;

#ifndef NDEBUG
    char* type_name;
#endif
} Array;

#define ARRAY_TYPE(type) Array

//TODO: use the allocator interface
Array* _array_create(u64 data_stride, u64 capacity, Allocator* allocator);

#define array_create(type, capacity, allocator)\
    _array_create(sizeof(type), capacity, allocator)

void array_free(Array* array);
void array_clear(Array* array);
void array_zero(Array* array);

void array_push(Array* array, const void* new_data);
void array_pop(Array* array);

//Ex: array_push_c_array(array, (type* test [] ={ data, data,} ), ARRAY_SIZE(test))
//TODO: write a test for this
void _array_push_c_array(Array* array, const void* new_data, u32 count);

#define array_push_c_array(array, c_array)\
    _array_push_c_array(array, c_array, ARRAY_SIZE(c_array));


bool array_serialize(Array* array, FILE* fptr);
bool array_deserialize(Array* array, FILE* file);


void array_print(Array* array, void (*print_func)(void*));
void array_print_range(Array* array, u64 start, u64 end, void (*print_func)(void*));

bool array_is_empty(const Array* array);
bool array_is_full(const Array* array);
bool array_valid_index(const Array* array, const u64 index);
u64 array_get_bytes_used(const Array* array);
void* _array_get(Array* array, const u64 index);
void* _array_top(Array* array);

#define array_top(arr, type)\
    (*(type*)_array_get(arr, arr->num_items-1))

#define array_get(arr, type, index)\
    (*(type*)_array_get(arr, index))

#define array_top_free(arr, type)\
    (*(type*)_array_get(arr, arr->num_items))

void array_set(Array* array, const void* data, const u64 pos);


//TODO: TEST
// fills entire array
void array_fill(const Array* array, const void* data);

//TODO: TEST
// fills on a range array
void array_fill_range(Array* array, u64 start, u64 end, void* data);


//shift the array, maintaining order
void array_remove(Array* array, const u64 index);

//replaces the index value with the last item in the array, then decrementing the array num count
void array_remove_swap(Array* array, u64 index);

//TODO: all the sorting functions
void array_counting_sort(Array* array, int (*cmp_func)(void*, void*));

void array_merge_sort(Array* array, int (*cmp_func)(void*, void*));

void array_radix_sort(Array* array, int (*cmp_func)(void*, void*));


void array_test();


#endif //ARRAY_H
