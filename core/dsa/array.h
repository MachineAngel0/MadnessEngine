#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

//fixed sized array, so no reallocating more space
typedef struct Array
{
    void* data; //array of void* data
    u64 stride; // size/stride of each void* data
    u64 capacity; // size of the array
    u64 num_items; // current/top index in our array

#ifndef NDEBUG
    //extra debug type info that gets removed in release builds
    const char* debug_type_name;
#endif
} Array;


Array* _array_create(const u64 data_stride, const u64 capacity, const char* type_name);

#define array_create(type, capacity)\
    _array_create(sizeof(type), capacity, #type)

bool _array_type_check(Array* array, const char* type_name);

#define array_type_check(arr, type)\
    _array_type_check(arr, #type)


void array_free(Array* array);
void array_clear(Array* array);

void array_print(Array* array, void (*print_func)(void*));

void array_print_range(Array* array, u64 start, u64 end, void (*print_func)(void*));

bool array_is_empty(const Array* array);
bool array_is_full(const Array* array);
bool array_valid_index(const Array* array, const u64 index);
void* array_get(Array* array, const u64 index);
void array_set(Array* array, const void* data, const u64 pos);


//TODO: TEST
// fills entire array
void array_fill(const Array* array, const void* data);

//TODO: TEST
// fills on a range array
void array_fill_range(Array* array, u64 start, u64 end, void* data);
void array_push(Array* array, void* new_data);

void array_pop(Array* array);


//shift the array, maintaining order
void array_remove(Array* array, const u64 index);

//replaces the index value with the last item in the array, then decrementing the array num count
void array_remove_swap(Array* array, u64 index);

//TODO: all the sorting functions
void array_counting_sort(Array* array, int (*cmp_func)(void*, void*));

void array_merge_sort(Array* array, int (*cmp_func)(void*, void*));

void array_radix_sort(Array* array, int (*cmp_func)(void*, void*));


void array_test();


// #define STBDS_ADDRESSOF(typevar, value)     ((__typeof__(typevar)[1]){value}) // literal array decays to pointer to value
// // #define array_new_contains_or_add_test(array, value)    \
// {                                                     \
//     temp_value = (void*) STBDS_ADDRESSOF((array, (value)) \
//     array_new_contains_or_add_other(array, temp_value); \
// }



#endif //ARRAY_H
