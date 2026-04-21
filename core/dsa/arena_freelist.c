#include "arena_freelist.h"

void arena_free_list_init(Arena_Free_List* fl, void* backing_memory, size_t memory_size)
{
    fl->data = backing_memory;
    fl->capacity = memory_size;
    fl->used = 0;
    arena_free_list_free_all(fl);
}

void* arena_free_list_alloc_aligned(Arena_Free_List* fl, size_t size, size_t alignment)
{
    size_t padding = 0;
    Free_List_Node* prev_node = NULL;
    Free_List_Node* node = NULL;
    size_t alignment_padding, required_space, remaining;
    Free_List_Allocation_Header* header_ptr;

    if (size < sizeof(Free_List_Node))
    {
        size = sizeof(Free_List_Node);
    }
    if (alignment < 8)
    {
        alignment = 8;
    }

    /*
    if (fl->policy == Placement_Policy_Find_Best) {
        node = free_list_find_best(fl, size, alignment, &padding, &prev_node);
    } else {
        node = free_list_find_first(fl, size, alignment, &padding, &prev_node);
    }*/

    node = arena_free_list_find_first(fl, size, alignment, &padding, &prev_node);

    if (node == NULL)
    {
        MASSERT_MSG(0, "Free list has no free memory");
        return NULL;
    }

    alignment_padding = padding - sizeof(Free_List_Allocation_Header);
    required_space = size + padding;
    remaining = node->block_size - required_space;

    if (remaining > 0)
    {
        Free_List_Node* new_node = (Free_List_Node*)((char*)node + required_space);
        new_node->block_size = remaining; //NOTE* this might be a source of bugs
        arena_free_list_node_insert(&fl->head, node, new_node);
    }

    arena_free_list_node_remove(&fl->head, prev_node, node);

    header_ptr = (Free_List_Allocation_Header*)((char*)node + alignment_padding);
    header_ptr->block_size = required_space;
    header_ptr->padding = alignment_padding;

    fl->used += required_space;

    void* out = (void*)((char*)header_ptr + sizeof(Free_List_Allocation_Header));
    memset(out, 0, size);
    return out;
    // return (void*)((char*)header_ptr + sizeof(Free_List_Allocation_Header));
}

void* arena_free_list_alloc(Arena_Free_List* fl, size_t size)
{
    return arena_free_list_alloc_aligned(fl, size, DEFAULT_ALIGNMENT);
}


void arena_free_list_free(Arena_Free_List* fl, void* ptr)
{
    Free_List_Allocation_Header *header;
    Free_List_Node *free_node;
    Free_List_Node *node;
    Free_List_Node *prev_node = NULL;

    if (ptr == NULL) {
        return;
    }

    header = (Free_List_Allocation_Header *)((char *)ptr - sizeof(Free_List_Allocation_Header));
    free_node = (Free_List_Node *)header;
    free_node->block_size = header->block_size + header->padding;
    free_node->next = NULL;

    node = fl->head;
    while (node != NULL) {
        if (ptr < node) {
            arena_free_list_node_insert(&fl->head, prev_node, free_node);
            break;
        }
        prev_node = node;
        node = node->next;
    }

    fl->used -= free_node->block_size;

    arena_free_list_coalescence(fl, prev_node, free_node);
}

void arena_free_list_free_all(Arena_Free_List* fl)
{
    //reset the memory used
    fl->used = 0;
    //set the start of the memory to be the head node
    Free_List_Node* first_node = (Free_List_Node*)fl->data;
    first_node->block_size = fl->capacity;
    first_node->next = NULL;
    fl->head = first_node;
}

void arena_free_list_coalescence(Arena_Free_List* fl, Free_List_Node* prev_node, Free_List_Node* free_node)
{
    if (free_node->next != NULL && (void *)((char *)free_node + free_node->block_size) == free_node->next) {
        free_node->block_size += free_node->next->block_size;
        arena_free_list_node_remove(&fl->head, free_node, free_node->next);
    }

    if (prev_node->next != NULL && (void *)((char *)prev_node + prev_node->block_size) == free_node) {
        prev_node->block_size += free_node->next->block_size;
        arena_free_list_node_remove(&fl->head, prev_node, free_node);
    }
}

Free_List_Node* arena_free_list_find_first(Arena_Free_List* fl, size_t size, size_t alignment, size_t* out_padding,
                                     Free_List_Node** out_prev_node)
{
    // Iterates the list and finds the first free block with enough space
    Free_List_Node* node = fl->head;
    Free_List_Node* prev_node = NULL;

    size_t padding = 0;

    while (node != NULL)
    {
        //find alignment size
        padding = calc_padding_with_header_fl((uintptr_t)node, (uintptr_t)alignment,
                                              sizeof(Free_List_Allocation_Header));
        size_t required_space = size + padding;

        if (node->block_size >= required_space)
        {
            //found a block with sufficient memory
            break;
        }
        //otherwise keep traversing the free list
        prev_node = node;
        node = node->next;
    }
    //set out data
    if (out_padding) { *out_padding = padding; }
    if (out_prev_node) { *out_prev_node = prev_node; }
    return node;
}

Free_List_Node* arena_free_list_find_best(Arena_Free_List* fl, size_t size, size_t alignment, size_t* out_padding,
                                    Free_List_Node** out_prev_node)
{
    // This iterates the entire list to find the best fit
    // O(n)
    size_t smallest_diff = ~(size_t)0;

    Free_List_Node* node = fl->head;
    Free_List_Node* prev_node = NULL;
    Free_List_Node* best_node = NULL;

    size_t padding = 0;

    while (node != NULL)
    {
        //get sizing
        padding = calc_padding_with_header((uintptr_t)node, (uintptr_t)alignment, sizeof(Free_List_Allocation_Header));
        size_t required_space = size + padding;
        //check if the current node if better than the best node
        if (node->block_size >= required_space && (best_node->block_size - required_space < smallest_diff))
        {
            best_node = node;
        }
        prev_node = node;
        node = node->next;
    }
    if (out_padding) *out_padding = padding;
    if (out_prev_node) *out_prev_node = prev_node;
    return best_node;
}


size_t calc_padding_with_header_fl(uintptr_t ptr, uintptr_t alignment, size_t header_size)
{
    uintptr_t p, a, modulo, padding, needed_space;

    MASSERT(is_power_of_two(alignment));

    p = ptr;
    a = alignment;
    modulo = p & (a - 1); // (p % a) as it assumes alignment is a power of two

    padding = 0;
    needed_space = 0;

    if (modulo != 0)
    {
        // Same logic as 'align_forward'
        padding = a - modulo;
    }

    needed_space = (uintptr_t)header_size;

    if (padding < needed_space)
    {
        needed_space -= padding;

        if ((needed_space & (a - 1)) != 0)
        {
            padding += a * (1 + (needed_space / a));
        }
        else
        {
            padding += a * (needed_space / a);
        }
    }

    return (size_t)padding;
}

void arena_free_list_node_insert(Free_List_Node** phead, Free_List_Node* prev_node, Free_List_Node* new_node)
{
    if (prev_node == NULL)
    {
        if (*phead != NULL)
        {
            new_node->next = *phead;
        }
        else
        {
            *phead = new_node;
        }
    }
    else
    {
        if (prev_node->next == NULL)
        {
            prev_node->next = new_node;
            new_node->next = NULL;
        }
        else
        {
            new_node->next = prev_node->next;
            prev_node->next = new_node;
        }
    }
}

void arena_free_list_node_remove(Free_List_Node** phead, Free_List_Node* prev_node, Free_List_Node* del_node)
{
    if (prev_node == NULL)
    {
        *phead = del_node->next;
    }
    else
    {
        prev_node->next = del_node->next;
    }
}

void free_list_test(void)
{
    TEST_START("FREE LIST");

    u64 memory_alloc_size = 1000;
    void* mem_block = malloc(memory_alloc_size);
    Arena_Free_List fl;
    arena_free_list_init(&fl, mem_block, memory_alloc_size);

    TEST_DEBUG(fl.capacity == memory_alloc_size);
    TEST_DEBUG(fl.used == 0);

    //TODO: flesh this out
    int* i = arena_free_list_alloc(&fl, sizeof(int));
    float* f = arena_free_list_alloc(&fl, sizeof(float));
    double* d = arena_free_list_alloc(&fl, sizeof(double));
    u64* uint64 = arena_free_list_alloc(&fl, sizeof(u64));



    TEST_END("FREE LIST");


}
