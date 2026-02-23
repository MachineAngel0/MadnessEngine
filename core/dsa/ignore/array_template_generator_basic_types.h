#ifndef GENERATEDs_ARRAY_HEADER_H
#define GENERATEDs_ARRAY_HEADER_H

#include <assert.h>
#include <stdio.h>
#include "defines.h"

#define DARRAY_T int
#define DARRAY_PREFIX i
#define DARRAY_NAME IntArray
// Must be manually instantiated by #including the file
#include "array_template_generator.h"

#define DARRAY_T uint16_t
#define DARRAY_PREFIX uint16_t
#define DARRAY_NAME u16Array
// Must be manually instantiated by #including the file
#include "array_template_generator.h"




int basic_testing_generator_nonsense(void){
    IntArray ints = {0};
    ipush(&ints, 1);
    ipush(&ints, 2);
    ipush(&ints, 3);
    ipush(&ints, 4);
    ipush(&ints, 5);
    ipush(&ints, 6);
    assert(ints.count == 6);
    assert(ints.items[0] == 1);
    assert(ints.items[1] == 2);
    assert(ints.items[2] == 3);
    assert(ints.items[3] == 4);
    assert(ints.items[4] == 5);
    assert(ints.items[5] == 6);
    assert(ints.capacity == 8);
    for(size_t i = 0; i < ints.count; i++){
        printf("[%zu] = %d\n", i, ints.items[i]);
    }
    free(ints.items);


    u16 ints = {0};
    ipush(&ints, 1);
    ipush(&ints, 2);
    ipush(&ints, 3);
    ipush(&ints, 4);
    ipush(&ints, 5);
    ipush(&ints, 6);
    assert(ints.count == 6);
    assert(ints.items[0] == 1);
    assert(ints.items[1] == 2);
    assert(ints.items[2] == 3);
    assert(ints.items[3] == 4);
    assert(ints.items[4] == 5);
    assert(ints.items[5] == 6);
    assert(ints.capacity == 8);
    for(size_t i = 0; i < ints.count; i++){
        printf("[%zu] = %d\n", i, ints.items[i]);
    }
    free(ints.items);



    return 0;
}


#endif
