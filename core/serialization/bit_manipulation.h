#ifndef BIT_MANIPULATION_H
#define BIT_MANIPULATION_H

#include "defines.h"
#include "asserts.h"

// serialization directory probably isn't the best place to put this

u64 bools_to_bitflag( bool* values, u32 count)
{
    MASSERT(count <= 64);
    u64 flags = 0;
    for (u32 i = 0; i < count; i++)
    {
        if (values[i])
        {
            flags |= (1ull << i);
        }
    }
    return flags;
}

void bitflag_to_bools(u64 flags, bool* out_values, u32 count)
{
    MASSERT(count <= 64);
    for (u32 i = 0; i < count; i++)
    {
        out_values[i] = (flags & (1ull << i)) != 0;
    }
}

#endif //BIT_MANIPULATION_H