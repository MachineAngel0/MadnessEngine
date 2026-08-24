#include "free_list.h"
#include "asserts.h"
#include "unit_test.h"

Free_List* free_list_init(Allocator* allocator, u64 memory_size, u32 free_list_node_count)
{
    Free_List* fl = allocator_alloc(allocator, sizeof(Free_List));
    fl->freelist_node_count = free_list_node_count;
    fl->nodes = allocator_alloc(allocator, free_list_node_count * sizeof(Free_List_Node));
    fl->head = &fl->nodes[0];

    fl->capacity = memory_size;
    fl->used = 0;

    freelist_free_all(fl);


    return fl;
}

bool free_list_alloc(Free_List* free_list, u64 request_size, u64* out_offset)
{
    return free_list_alloc_aligned(free_list, request_size, 1, out_offset);
}

bool free_list_alloc_aligned(Free_List* free_list, u64 request_size, u64 alignment, u64* out_offset)
{
    MASSERT(free_list);
    MASSERT(out_offset);

    if (request_size <= 0)
    {
        WARN("free_list_alloc: invalid request size")
        return false;
    }

    if (alignment == 0 || (alignment & (alignment - 1)) != 0)
    {
        WARN("free_list_alloc: alignment must be a non-zero power of two");
        return false;
    }

    Free_List_Node* node = free_list->head;
    Free_List_Node* previous = NULL;

    while (node)
    {
        // Align the beginning of this free range.
        // node->offset = 100
        // alignment    = 64
        // aligned_offset = 128
        u64 aligned_offset =
            free_list_align_up(node->offset, alignment);

        u64 padding = aligned_offset - node->offset;


        // Make sure the allocation actually fits inside this free range.
        if (padding <= node->size &&
            request_size <= node->size - padding)
        {
            u64 remaining =
                node->size - padding - request_size;

            /*
             * We potentially have:
             * [padding][allocation][remaining]
             * The padding is itself still free, so we need
             * another node if padding != 0.
             */

            if (padding == 0)
            {
                /*
                 * Allocation begins exactly at the start
                 * of the free range.
                 */

                if (remaining == 0)
                {
                    // Entire node consumed.
                    Free_List_Node* next = node->next;

                    if (previous)
                        previous->next = next;
                    else
                        free_list->head = next;

                    free_list_return_node(node);
                }
                else
                {
                    // Consume the beginning of this range.
                    node->offset += request_size;
                    node->size = remaining;
                }
            }
            else
            {
                /*
                 * We need to preserve the padding:
                 * [padding][allocation][remaining]
                 * The current node can represent the padding.
                 */

                Free_List_Node* remainder_node = NULL;

                if (remaining != 0)
                {
                    remainder_node = free_list_get_node(free_list);

                    if (!remainder_node)
                    {
                        WARN(
                            "free_list_alloc: no free-list nodes available "
                            "for aligned allocation");
                        return false;
                    }
                }

                /*
                 * Current node becomes:
                 * [padding]
                 */
                node->size = padding;

                if (remaining != 0)
                {
                    //New node becomes:
                    //[remaining]
                    remainder_node->offset =
                        aligned_offset + request_size;

                    remainder_node->size = remaining;
                    remainder_node->next = node->next;

                    node->next = remainder_node;
                }
            }

            *out_offset = aligned_offset;

            free_list->used += request_size;

            return true;
        }

        previous = node;
        node = node->next;
    }


    WARN("free_list_alloc: no block with enough aligned free space "
         "(requested: %lluB, alignment: %lluB, available: %lluB).",
         request_size,
         alignment,
         freelist_space_available(free_list));
    return false;
}


bool free_list_free(Free_List* free_list, u64 offset, u64 size)
{
    MASSERT(free_list);

    if (size <= 0)
    {
        WARN("free_list_free: invalid size")
        return false;
    }


    Free_List_Node* node = free_list->head;
    Free_List_Node* previous = NULL;

    if (!node)
    {
        // Check for the case where the entire thing is allocated.
        // In this case a new node is needed at the head.
        Free_List_Node* new_node = free_list_get_node(free_list);
        if (!new_node)
        {
            return false;
        }

        new_node->offset = offset;
        new_node->size = size;
        new_node->next = 0;
        free_list->head = new_node;
        free_list->used -= size;
        return true;
    }


    while (node)
    {
        if (node->offset + node->size == offset)
        {
            // Can be appended to the right of this node.
            node->size += size;

            // Check if this then connects the range between this and the next
            // node, and if so, combine them and return the second node..
            if (node->next && node->next->offset == node->offset + node->size)
            {
                node->size += node->next->size;
                Free_List_Node* next = node->next;
                node->next = node->next->next;
                free_list_return_node(next);
            }
            free_list->used -= size;
            return true;
        }
        if (node->offset == offset)
        {
            // If there is an exact match, this means the exact block of memory
            // that is already free is being freed again.
            FATAL("FREE_LIST_FREE: Attempting to free already-freed block of memory at offset %llu", node->offset);
            return false;
        }
        if (node->offset > offset)
        {
            // Iterated beyond the space to be freed. Need a new node.
            Free_List_Node* new_node = free_list_get_node(free_list);
            if (!new_node)
            {
                return false;
            }
            new_node->offset = offset;
            new_node->size = size;

            // If there is a previous node, the new node should be inserted between this and it.
            if (previous)
            {
                previous->next = new_node;
                new_node->next = node;
            }
            else
            {
                // Otherwise, the new node becomes the head.
                new_node->next = node;
                free_list->head = new_node;
            }

            // Double-check next node to see if it can be joined.
            if (new_node->next && new_node->offset + new_node->size == new_node->next->offset)
            {
                new_node->size += new_node->next->size;
                Free_List_Node* rubbish = new_node->next;
                new_node->next = rubbish->next;
                free_list_return_node(rubbish);
            }

            // Double-check previous node to see if the new_node can be joined to it.
            if (previous && previous->offset + previous->size == new_node->offset)
            {
                previous->size += new_node->size;
                Free_List_Node* rubbish = new_node;
                previous->next = rubbish->next;
                free_list_return_node(rubbish);
            }
            free_list->used -= size;

            return true;
        }


        // If on the last node and the last node's offset + size < the free offset,
        // a new node is required.
        if (!node->next && node->offset + node->size < offset)
        {
            Free_List_Node* new_node = free_list_get_node(free_list);
            new_node->offset = offset;
            new_node->size = size;
            new_node->next = 0;
            node->next = new_node;
            free_list->used -= size;

            return true;
        }

        previous = node;
        node = node->next;
    }

    MASSERT_MSG(false, "FREE_LIST_FREE: UNABLE TO FIND BLOCK TO BE FREE. MISUSE OR MEMORY CORRUPTION");
    return false;
}

Free_List_Node* free_list_get_node(Free_List* free_list)
{
    for (u64 i = 1; i < free_list->freelist_node_count; ++i)
    {
        if (free_list->nodes[i].size == 0)
        {
            free_list->nodes[i].next = 0;
            free_list->nodes[i].offset = 0;
            return &free_list->nodes[i];
        }
    }

    // Return nothing if no nodes are available.
    INFO("NO FREE LIST NODE AVAILABLE");
    return 0;
}


void freelist_free_all(Free_List* free_list)
{
    MASSERT(free_list)

    // Invalidate the offset for all but the first node. The invalid
    // value will be checked for when seeking a new node from the list.
    memory_system_zero_memory(free_list->nodes, sizeof(Free_List_Node) * free_list->freelist_node_count);

    // Reset the head to occupy the entire thing.
    free_list->head->offset = 0;
    free_list->head->size = free_list->capacity;
    free_list->head->next = 0;

    free_list->used = 0;
}

void free_list_return_node(Free_List_Node* node)
{
    node->offset = 0;
    node->size = 0;
    node->next = 0;
}

u64 freelist_space_available(Free_List* free_list)
{
    // return free_list->capacity - free_list->used;
    MASSERT(free_list)

    u64 running_total = 0;
    Free_List_Node* node = free_list->head;
    while (node)
    {
        running_total += node->size;
        node = node->next;
    }

    return running_total;
}

u64 free_list_align_up(u64 value, u64 alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}


bool free_list_validate(Free_List* free_list)
{
    MASSERT(free_list);

    u64 free_size = 0;

    Free_List_Node* node = free_list->head;

    while (node)
    {
        MASSERT(node->size > 0);

        MASSERT(node->offset < free_list->capacity);

        MASSERT(
            node->size <=
            free_list->capacity - node->offset);

        if (node->next)
        {
            // Sorted by offset.
            MASSERT(node->offset < node->next->offset);

            // No overlap.
            MASSERT(
                node->offset + node->size <=
                node->next->offset);
        }

        free_size += node->size;

        node = node->next;
    }

    MASSERT(
        free_size + free_list->used ==
        free_list->capacity);

    return true;
}

u64 freelist_test()
{
    TEST_START("FREE LIST");

    const u64 memory_size = 1024;
    const u32 node_count = 32;

    /*
     * The Allocator is only backing the Free_List's metadata.
     *
     * In a real Vulkan allocator this would be some CPU-side allocator
     * used to allocate the Free_List and its nodes. The memory being
     * managed by the Free_List is represented by memory_size.
     */
    const u64 allocator_memory_size =
        sizeof(Free_List) +
        node_count * sizeof(Free_List_Node) +
        64;

    void* backing_memory = malloc(allocator_memory_size);
    TEST_DEBUG(backing_memory);

    Allocator allocator = {0};

    allocator_init(
        &allocator,
        backing_memory,
        allocator_memory_size);

    Free_List* free_list = free_list_init(
        &allocator,
        memory_size,
        node_count);

    TEST_DEBUG(free_list);

    /*
     * ------------------------------------------------------------
     * Initial state
     * ------------------------------------------------------------
     */

    TEST_DEBUG(free_list->capacity == memory_size);
    TEST_DEBUG(free_list->used == 0);

    TEST_DEBUG(free_list->head != NULL);
    TEST_DEBUG(free_list->head->offset == 0);
    TEST_DEBUG(free_list->head->size == memory_size);
    TEST_DEBUG(free_list->head->next == NULL);

    TEST_DEBUG(freelist_space_available(free_list) == memory_size);
    TEST_DEBUG(free_list_validate(free_list));


    /*
     * ------------------------------------------------------------
     * Basic allocation
     * ------------------------------------------------------------
     */

    u64 offset_a = 0;

    TEST_DEBUG(
        free_list_alloc(
            free_list,
            100,
            &offset_a));

    TEST_DEBUG(offset_a == 0);
    TEST_DEBUG(free_list->used == 100);
    TEST_DEBUG(freelist_space_available(free_list) == 924);
    TEST_DEBUG(free_list_validate(free_list));


    /*
     * Allocate another block.
     */

    u64 offset_b = 0;

    TEST_DEBUG(
        free_list_alloc(
            free_list,
            200,
            &offset_b));

    TEST_DEBUG(offset_b == 100);
    TEST_DEBUG(free_list->used == 300);
    TEST_DEBUG(freelist_space_available(free_list) == 724);
    TEST_DEBUG(free_list_validate(free_list));

    /*
     * ------------------------------------------------------------
     * Alignment
     * ------------------------------------------------------------
     *
     * Current state:
     *
     * [0..100)       A
     * [100..300)     B
     * [300..1024)    free
     *
     * Allocate 64 bytes aligned to 256.
     *
     * 300 -> 512 requires 212 bytes of padding.
     *
     * Result:
     *
     * [0..100)       A
     * [100..300)     B
     * [300..512)     free/padding
     * [512..576)     C
     * [576..1024)    free
     */

    u64 offset_c = 0;

    TEST_DEBUG(
        free_list_alloc_aligned(
            free_list,
            64,
            256,
            &offset_c));

    TEST_DEBUG(offset_c == 512);
    TEST_DEBUG(offset_c % 256 == 0);

    TEST_DEBUG(free_list->used == 364);
    TEST_DEBUG(freelist_space_available(free_list) == 660);

    TEST_DEBUG(free_list_validate(free_list));

    /*
     * Explicitly verify the two free ranges produced by
     * the aligned allocation.
     */

    Free_List_Node* node = free_list->head;

   TEST_DEBUG(node != NULL);
   TEST_DEBUG(node->offset == 300);
   TEST_DEBUG(node->size == 212);

   TEST_DEBUG(node->next != NULL);
   TEST_DEBUG(node->next->offset == 576);
   TEST_DEBUG(node->next->size == 448);

   TEST_DEBUG(node->next->next == NULL);


    /*
     * ------------------------------------------------------------
     * Free middle allocation
     * ------------------------------------------------------------
     *
     * Free B:
     *
     * [0..100)       A
     * [100..512)     free
     * [512..576)     C
     * [576..1024)    free
     */

    TEST_DEBUG(
        free_list_free(
            free_list,
            offset_b,
            200));

    TEST_DEBUG(free_list->used == 164);
    TEST_DEBUG(freelist_space_available(free_list) == 860);
    TEST_DEBUG(free_list_validate(free_list));

    /*
     * The first free range should now be:
     *
     * [100..512)
     */

    node = free_list->head;

   TEST_DEBUG(node != NULL);
   TEST_DEBUG(node->offset == 100);
   TEST_DEBUG(node->size == 412);

   TEST_DEBUG(node->next != NULL);
   TEST_DEBUG(node->next->offset == 576);
   TEST_DEBUG(node->next->size == 448);

   TEST_DEBUG(node->next->next == NULL);


    /*
     * ------------------------------------------------------------
     * Free C
     * ------------------------------------------------------------
     *
     * C connects the two free ranges:
     *
     * [100..512) + [512..576) + [576..1024)
     *
     * resulting in:
     *
     * [0..100)       A
     * [100..1024)    free
     */

    TEST_DEBUG(
        free_list_free(
            free_list,
            offset_c,
            64));

   TEST_DEBUG(free_list->used == 100);
   TEST_DEBUG(freelist_space_available(free_list) == 924);
   TEST_DEBUG(free_list_validate(free_list));

    node = free_list->head;

    TEST_DEBUG(node != NULL);
    TEST_DEBUG(node->offset == 100);
    TEST_DEBUG(node->size == 924);
    TEST_DEBUG(node->next == NULL);


    /*
     * ------------------------------------------------------------
     * Free A
     * ------------------------------------------------------------
     */

    TEST_DEBUG(
        free_list_free(
            free_list,
            offset_a,
            100));

    TEST_DEBUG(free_list->used == 0);
    TEST_DEBUG(freelist_space_available(free_list) == memory_size);
    TEST_DEBUG(free_list_validate(free_list));

    TEST_DEBUG(free_list->head != NULL);
    TEST_DEBUG(free_list->head->offset == 0);
    TEST_DEBUG(free_list->head->size == memory_size);
    TEST_DEBUG(free_list->head->next == NULL);
    /*
     * ------------------------------------------------------------
     * Test several alignments
     * ------------------------------------------------------------
     *
     * Since the free list starts at zero, these mostly test that
     * the returned address satisfies the alignment requirement.
     */

    const u64 alignments[] =
    {
        1,
        2,
        4,
        8,
        16,
        32,
        64,
        128,
        256
    };

    for (u32 i = 0;
         i < sizeof(alignments) / sizeof(alignments[0]);
         ++i)
    {
        u64 offset = 0;

        TEST_DEBUG(
            free_list_alloc_aligned(
                free_list,
                32,
                alignments[i],
                &offset));

        TEST_DEBUG(offset % alignments[i] == 0);
        TEST_DEBUG(free_list->used == 32);
        TEST_DEBUG(free_list_validate(free_list));

        TEST_DEBUG(
            free_list_free(
                free_list,
                offset,
                32));

       TEST_DEBUG(free_list->used == 0);
       TEST_DEBUG(freelist_space_available(free_list) == memory_size);
       TEST_DEBUG(free_list_validate(free_list));
    }


    /*
     * ------------------------------------------------------------
     * Alignment requiring padding
     * ------------------------------------------------------------
     *
     * Force the free range to begin at an unaligned offset.
     *
     * Allocate 13 bytes first:
     *
     * [0..13)       used
     * [13..1024)    free
     *
     * Then request 32 bytes aligned to 64:
     *
     * aligned offset = 64
     * padding = 51
     *
     * Result:
     *
     * [13..64)      free
     * [64..96)      used
     * [96..1024)    free
     */

    u64 offset_d = 0;

    TEST_DEBUG(
        free_list_alloc(
            free_list,
            13,
            &offset_d));

    TEST_DEBUG(offset_d == 0);
    TEST_DEBUG(free_list_validate(free_list));

    u64 offset_e = 0;

    TEST_DEBUG(
        free_list_alloc_aligned(
            free_list,
            32,
            64,
            &offset_e));

    TEST_DEBUG(offset_e == 64);
    TEST_DEBUG(offset_e % 64 == 0);

    TEST_DEBUG(free_list->used == 45);
    TEST_DEBUG(freelist_space_available(free_list) == 979);
    TEST_DEBUG(free_list_validate(free_list));

    /*
     * Verify padding and remainder nodes.
     */

    node = free_list->head;

    TEST_DEBUG(node != NULL);
    TEST_DEBUG(node->offset == 13);
    TEST_DEBUG(node->size == 51);

    TEST_DEBUG(node->next != NULL);
    TEST_DEBUG(node->next->offset == 96);
    TEST_DEBUG(node->next->size == 928);

    TEST_DEBUG(node->next->next == NULL);


    /*
     * Free E. This should merge the padding and remainder.
     */

    TEST_DEBUG(
        free_list_free(
            free_list,
            offset_e,
            32));

    TEST_DEBUG(free_list->used == 13);
    TEST_DEBUG(freelist_space_available(free_list) == 1011);
    TEST_DEBUG(free_list_validate(free_list));

    node = free_list->head;

   TEST_DEBUG(node != NULL);
   TEST_DEBUG(node->offset == 13);
   TEST_DEBUG(node->size == 1011);
   TEST_DEBUG(node->next == NULL);


    /*
     * Free D and restore the entire allocator.
     */

    TEST_DEBUG(
        free_list_free(
            free_list,
            offset_d,
            13));

   TEST_DEBUG(free_list->used == 0);
   TEST_DEBUG(freelist_space_available(free_list) == memory_size);
   TEST_DEBUG(free_list_validate(free_list));

   TEST_DEBUG(free_list->head->offset == 0);
   TEST_DEBUG(free_list->head->size == memory_size);
   TEST_DEBUG(free_list->head->next == NULL);


    /*
     * ------------------------------------------------------------
     * Invalid allocation requests
     * ------------------------------------------------------------
     */

    u64 invalid_offset = 0;

    TEST_DEBUG(
        !free_list_alloc(
            free_list,
            0,
            &invalid_offset));

    TEST_DEBUG(
        !free_list_alloc_aligned(
            free_list,
            32,
            0,
            &invalid_offset));

    TEST_DEBUG(
        !free_list_alloc_aligned(
            free_list,
            32,
            3,
            &invalid_offset));

    TEST_DEBUG(free_list->used == 0);
    TEST_DEBUG(free_list_validate(free_list));


    /*
     * ------------------------------------------------------------
     * Allocation too large
     * ------------------------------------------------------------
     */

    TEST_DEBUG(
        !free_list_alloc(
            free_list,
            memory_size + 1,
            &invalid_offset));

    TEST_DEBUG(free_list->used == 0);
    TEST_DEBUG(free_list_validate(free_list));


    /*
     * ------------------------------------------------------------
     * Test free_all
     * ------------------------------------------------------------
     */

    TEST_DEBUG(
        free_list_alloc(
            free_list,
            512,
            &offset_a));

    TEST_DEBUG(free_list->used == 512);
    TEST_DEBUG(free_list_validate(free_list));

    freelist_free_all(free_list);

    TEST_DEBUG(free_list->used == 0);
    TEST_DEBUG(freelist_space_available(free_list) == memory_size);
    TEST_DEBUG(free_list_validate(free_list));

    TEST_DEBUG(free_list->head != NULL);
    TEST_DEBUG(free_list->head->offset == 0);
    TEST_DEBUG(free_list->head->size == memory_size);
    TEST_DEBUG(free_list->head->next == NULL);


    /*
     * ------------------------------------------------------------
     * Final state
     * ------------------------------------------------------------
     */

    TEST_DEBUG(free_list->used == 0);
    TEST_DEBUG(freelist_space_available(free_list) == memory_size);
    TEST_DEBUG(free_list_validate(free_list));


    TEST_END("FREE LIST");

    free(backing_memory);

    return 0;
}
