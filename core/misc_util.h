#ifndef MISC_UTIL_H
#define MISC_UTIL_H
#include <stdio.h>
#include <string.h>

/*PRINT*/

void print_int(void* data_int)
{
    //cast to int* and then dereferenced, to get the value of the pointer of size int*
    printf("%d ", *(int *) data_int);
}
void println_int(void* data_int)
{
    //cast to int* and then dereferenced, to get the value of the pointer of size int*
    printf("%d\n", *(int *) data_int);
}

void print_float(void* data_int)
{
    printf("%f ", *(float *) data_int);
}

void print_double(void* data_int)
{
    printf("%f ", *(float *) data_int);
}

void print_char(void* data_int)
{
    printf("%c ", *(char *) data_int);
}


/*COMPARE*/

//compare function return -1,0,1
//-1 if a less than b, 0 equal, 1 if a greater than b

int cmp_int(void* a, void* b)
{
    int A = *(int*) a;
    int B = *(int*) b;
    return (A > B) - (A < B);
}


int cmp_uint(void* a, void* b)
{
    u32 A = *(u32*) a;
    u32 B = *(u32*) b;
    return (A > B) - (A < B);
}

int cmp_char(void* a, void* b)
{
    char A = *(char*) a;
    char B = *(char*) b;
    return (A > B) - (A < B);
}

int cmp_float(void* a, void* b)
{
    float A = *(float*) a;
    float B = *(float*) b;
    return (A > B) - (A < B);
}

//NOTE: just use memcmp(), returns 0 if both values are matching
// uint8_t cmp_equals_int(void* a, void* b)
// {
//     int A = *(int*) a;
//     int B = *(int*) b;
//     return A == B;
// }
//
// uint8_t cmp_equals_char(void* a, void* b)
// {
//     char A = *(char*) a;
//     char B = *(char*) b;
//     return A == B;
// }
//
// uint8_t cmp_equals_float(void* a, void* b)
// {
//     float A = *(float*) a;
//     float B = *(float*) b;
//     return A == B;
// }
void swap(void* left, void* right, size_t size)
{
    unsigned char* buffer = (unsigned char*)malloc(size);

    //copy left into buffer
    memcpy(buffer, left, size);
    //copy right into left
    memcpy(left, right, size);
    //copy buf(left) into right
    memcpy(right, buffer, size);

    free(buffer);
}


// Macro to print passed in string before and after a function call
#define PRINT_START_END_FUNCTION(string, func, ...) \
do { \
printf("%s %s\n", string, #func); \
func(__VA_ARGS__); \
printf("%s %s\n", string, #func); \
} while (0)

/* example usage
    PRINT_START_END_FUNCTION("hi", print_hello_world);
    PRINT_START_END_FUNCTION("another wow", print_dummy, "wow");
*/
void print_hello_world(void)
{
    printf("Hello world!\n");
}

void print_dummy(char* dummy_string)
{
    printf("%s\n", dummy_string);
}


#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif //MISC_UTIL_H
