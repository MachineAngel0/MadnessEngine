
#ifndef BITS_H 
#define BITS_H
#include "defines.h"

//TODO: not included in the core_unity file
u64 hamming_weight(u64 val)
{

    //counts all the 1 bits in the value
    // also called the hamming weight
    u64 sum = 0;

    while (val > 0)
    {
        sum += 1;
        val = val & (val - 1);
    }

    return sum;

}


#endif //BITS_H
