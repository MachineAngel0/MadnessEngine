#ifndef FREE_LIST_H
#define FREE_LIST_H


#include <stdbool.h>

#include "allocator.h"
#include "defines.h"


//NOTE: does not itself allocate memory, simply informs the user what memory is available for any memory request
//based on the kohi engine freelist

typedef struct Free_List_Node
{
    u64 offset;
    u64 size;
    struct Free_List_Node* next;
} Free_List_Node;

typedef struct Free_List
{
    u64 capacity;
    u64 used;

    u32 freelist_node_count;
    Free_List_Node* nodes;
    Free_List_Node* head;
} Free_List;



Free_List* free_list_init(Allocator* allocator, u64 memory_size, u32 free_list_node_count);

bool free_list_alloc(Free_List* free_list, u64 request_size, u64* out_offset);
bool free_list_alloc_aligned(Free_List* free_list, u64 request_size, u64 alignment, u64* out_offset);

bool free_list_free(Free_List* free_list, u64 offset, u64 size);

Free_List_Node* free_list_get_node(Free_List* free_list);

void freelist_free_all(Free_List* free_list);

void free_list_return_node(Free_List_Node* node);

u64 freelist_space_available(Free_List* free_list);

u64 free_list_align_up(u64 value, u64 alignment);

u64 freelist_test();

#endif //FREE_LIST_H
