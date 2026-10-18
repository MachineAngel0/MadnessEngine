#ifndef SORT_KEY_H
#define SORT_KEY_H


#include <stdint.h>

#include "resource_types.h"


// LAYOUT : pipeline_id[16] | permutation_id[32] | mesh_id[16]


#define PIPELINE_BITS     16
#define PERMUTATION_BITS  32
#define MESH_BITS         16

#define MESH_SHIFT        0
#define PERMUTATION_SHIFT (MESH_SHIFT + MESH_BITS)                 /* 16 */
#define PIPELINE_SHIFT    (PERMUTATION_SHIFT + PERMUTATION_BITS)   /* 48 */

#define PIPELINE_MASK     ((1ull << PIPELINE_BITS) - 1)
#define PERMUTATION_MASK  ((1ull << PERMUTATION_BITS) - 1)
#define MESH_MASK         ((1ull << MESH_BITS) - 1)

STATIC_ASSERT(PIPELINE_BITS + PERMUTATION_BITS + MESH_BITS == sizeof(uint64_t) * 8);


typedef struct Shader_Sort_Key_Unpacked
{
    uint16_t pipeline_id;
    uint32_t permutation_id;
    uint16_t mesh_id;
} Shader_Sort_Key_Unpacked;

MINLINE Shader_Sort_Key sort_key_create_opaque(uint16_t pipeline_id,
                                               uint32_t permutation_id,
                                               uint16_t mesh_id)
{
    return (((uint64_t)pipeline_id     & PIPELINE_MASK)    << PIPELINE_SHIFT) |
           (((uint64_t)permutation_id  & PERMUTATION_MASK) << PERMUTATION_SHIFT) |
           (((uint64_t)mesh_id         & MESH_MASK)        << MESH_SHIFT);
}

MINLINE Shader_Sort_Key_Unpacked sort_key_unpack_opaque(Shader_Sort_Key key)
{
    Shader_Sort_Key_Unpacked out;
    out.pipeline_id    = (uint16_t)((key >> PIPELINE_SHIFT)    & PIPELINE_MASK);
    out.permutation_id = (uint32_t)((key >> PERMUTATION_SHIFT) & PERMUTATION_MASK);
    out.mesh_id        = (uint16_t)((key >> MESH_SHIFT)        & MESH_MASK);
    return out;
}

MINLINE int sort_key_compare(Shader_Sort_Key key1, Shader_Sort_Key key2)
{
    return cmp_u64(&key1, &key2);
}



#endif
