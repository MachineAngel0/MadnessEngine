

#ifndef CONTAINER_UTILITY_H
#define CONTAINER_UTILITY_H


bool is_power_of_two(uintptr_t x)
{
    // bits that are a power of two have only one bit set to 1, 1,2,4,8,16,32,64
    //ex: 8  8 = 1000, 7 = 0111  1000 & 0111 = 0000, therefore power of two
    return (x & (x - 1)) == 0;
}
//To align a memory address to the specified alignment is simple modulo arithmetic.
//find how many bytes forward you need to go in order for the memory address is a multiple of the specified alignment
uintptr_t align_forward(uintptr_t ptr, size_t align)
{
    uintptr_t p, a, modulo;

    MASSERT(is_power_of_two(align));

    p = ptr;
    a = (uintptr_t) align;
    // Same as (p % a) but faster as 'a' is a power of two
    modulo = p & (a - 1);

    if (modulo != 0)
    {
        // If 'p' address is not aligned, push the address to the
        // next value which is aligned
        p += a - modulo;

        //ex: p = 7, a = 8, modulo p%a = 7
        // 8-7 = 1 , p+=1 = 8
        //ex2: p = 14, a = 8, modulo p%a (14%8) = 6
        // 8-6 = 2 , p+=2 = 16
    }
    return p;
}


uint32_t next_power_of_two_u32(uint32_t x)
{
    if (x == 0) return 1;

    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}

uint64_t next_power_of_two_u64(uint64_t x)
{
    if (x == 0) return 1;

    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    x++;
    return x;
}


#define FNV_PRIME_32 0x01000193
#define FNV_PRIME_64 0x00000100000001b3
#define FNV_OFFSET_BASIS_32 0x811c9dc5
#define FNV_OFFSET_BASIS_64 0xcbf29ce484222325


uint32_t generate_hash_key_32bit(const void* key, size_t data_size)
{
    // // https://en.wikipedia.org/wiki/Jenkins_hash_function#lookup3
    //
    // size_t i = 0;
    // uint32_t hash = 0;
    // while (i != length)
    // {
    //     hash += key[i++];
    //     hash += hash << 10;
    //     hash ^= hash >> 6;
    // }
    // hash += hash << 3;
    // hash ^= hash >> 11;
    // hash += hash << 15;
    // return hash;


    // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    const uint8_t* bytes = (const uint8_t*)key;
    uint32_t hash = FNV_OFFSET_BASIS_32;

    for(size_t i = 0; i < data_size; i++)
    {
        hash ^= bytes[i];
        hash *= FNV_PRIME_32;
    }

    return hash;

}

uint64_t generate_hash_key_64bit(uint8_t* key, size_t data_size)
{
    // // https://en.wikipedia.org/wiki/Jenkins_hash_function#lookup3
    //
    // size_t i = 0;
    // uint32_t hash = 0;
    // while (i != length)
    // {
    //     hash += key[i++];
    //     hash += hash << 10;
    //     hash ^= hash >> 6;
    // }
    // hash += hash << 3;
    // hash ^= hash >> 11;
    // hash += hash << 15;
    // return hash;


    // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    const uint8_t* bytes = (const uint8_t*)key;
    uint32_t hash = FNV_OFFSET_BASIS_64;

    for(size_t i = 0; i < data_size; i++)
    {
        hash ^= bytes[i];
        hash *= FNV_PRIME_32;
    }

    return hash;
}


#endif //CONTAINER_UTILITY_H
