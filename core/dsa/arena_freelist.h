#ifndef ARENA_FREELIST_H
#define ARENA_FREELIST_H
#include "memory/memory_system.h"

typedef struct Free_List_Allocation_Header
{
    size_t block_size;
    size_t padding;
} Free_List_Allocation_Header;


typedef struct Free_List_Node
{
    struct Free_List_Node* next;
    size_t block_size;
} Free_List_Node;

typedef struct Arena_Free_List
{
    void* data;
    size_t size;
    size_t used;

    Free_List_Node* head;
} Arena_Free_List;


void arena_free_list_create(Arena_Free_List* fl, void* backing_memory, size_t memory_size);
Arena_Free_List* arena_free_list_create_memory_system(Memory_System* memory_system, size_t memory_size);

void* arena_free_list_alloc(Arena_Free_List* fl, size_t size, size_t alignment);

void arena_free_list_free(Arena_Free_List* fl, void* ptr);
void arena_free_list_free_all(Arena_Free_List* fl);

void arena_free_list_coalescence(Arena_Free_List* fl, Free_List_Node* prev_node, Free_List_Node* free_node);

Free_List_Node* arena_free_list_find_first(Arena_Free_List* fl, size_t size, size_t alignment, size_t* out_padding,
                                           Free_List_Node** out_prev_node);

Free_List_Node* arena_free_list_find_best(Arena_Free_List* fl, size_t size, size_t alignment, size_t* out_padding,
                                          Free_List_Node** out_prev_node); //not using this, not really needed

size_t calc_padding_with_header_fl(uintptr_t ptr, uintptr_t alignment, size_t header_size);

void arena_free_list_node_insert(Free_List_Node** phead, Free_List_Node* prev_node, Free_List_Node* new_node);

void arena_free_list_node_remove(Free_List_Node** phead, Free_List_Node* prev_node, Free_List_Node* del_node);

void free_list_test(void);


#endif //ARENA_FREELIST_H
