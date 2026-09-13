#ifndef SORT_KEY_H
#define SORT_KEY_H
#include <stdint.h>
#include <string.h>


#define PIPELINE_BITS     16
#define PERMUTATION_BITS  32
#define DEPTH_BITS        16
/* 16 + 32 + 16 = 64 */

#define DEPTH_SHIFT        0
#define PERMUTATION_SHIFT  (DEPTH_SHIFT + DEPTH_BITS)         /* 16 */
#define PIPELINE_SHIFT     (PERMUTATION_SHIFT + PERMUTATION_BITS) /* 48 */

#define PIPELINE_MASK     ((1ull << PIPELINE_BITS) - 1)
#define PERMUTATION_MASK  ((1ull << PERMUTATION_BITS) - 1)
#define DEPTH_MASK         ((1ull << DEPTH_BITS) - 1)

STATIC_ASSERT(PIPELINE_BITS + PERMUTATION_BITS + DEPTH_BITS <= sizeof(uint64_t) * 8);


typedef uint64_t Sort_Key;

typedef struct
{
    uint16_t pipelineId;
    uint32_t permutationId;
    uint16_t depthBits;
} UnpackedKey;


static uint32_t FloatToSortableUint(float depth)
{
    uint32_t bits;
    memcpy(&bits, &depth, sizeof(bits));
    if (bits & 0x80000000u)
        return ~bits; /* negative float */
    else
        return bits | 0x80000000u; /* positive float */
}

/* Opaque list: pipeline dominates, then permutation, then depth (front-to-back) */
Sort_Key sort_key_make_opaque(uint16_t pipeline_id, uint32_t permutation_id, float depth)
{
    uint64_t key = 0;
    uint32_t sortableDepth = FloatToSortableUint(depth);
    uint16_t depthBits = (uint16_t)(sortableDepth >> 16); /* top 16 bits of sortable depth */

    key |= ((uint64_t)pipeline_id & PIPELINE_MASK) << PIPELINE_SHIFT;
    key |= ((uint64_t)permutation_id & PERMUTATION_MASK) << PERMUTATION_SHIFT;
    key |= ((uint64_t)depthBits & DEPTH_MASK) << DEPTH_SHIFT;
    return key;
}

/* Transparent list: depth dominates (back-to-front), pipeline/permutation as tie-breakers */
Sort_Key MakeTransparsort_key_make_transparent(uint16_t pipeline_id, uint32_t permutation_id, float depth)
{
    uint64_t key = 0;
    uint32_t sortable_depth = ~FloatToSortableUint(depth);
    uint16_t depth_bits = (uint16_t)(sortable_depth >> 16);

    key |= ((uint64_t)pipeline_id & PIPELINE_MASK) << PIPELINE_SHIFT;
    key |= ((uint64_t)permutation_id & PERMUTATION_MASK) << PERMUTATION_SHIFT;
    key |= ((uint64_t)depth_bits & DEPTH_MASK) << DEPTH_SHIFT;
    return key;
}

UnpackedKey UnpackSortKey(uint64_t key)
{
    UnpackedKey out;
    out.pipelineId = (uint16_t)((key >> PIPELINE_SHIFT) & PIPELINE_MASK);
    out.permutationId = (uint32_t)((key >> PERMUTATION_SHIFT) & PERMUTATION_MASK);
    out.depthBits = (uint16_t)((key >> DEPTH_SHIFT) & DEPTH_MASK);
    return out;
}


#endif
