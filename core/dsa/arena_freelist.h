#ifndef ARENA_FREELIST_H
#define ARENA_FREELIST_H


//RED BLACK TREE OPTIMIZATION, if i feel up for it some day
// https://www.gingerbill.org/article/2021/11/30/memory-allocation-strategies-005/

// Unlike our trivial stack allocator, this header needs to store the
// block size along with the padding meaning the header is a bit
// larger than the trivial stack allocator
typedef struct Free_List_Allocation_Header Free_List_Allocation_Header;

struct Free_List_Allocation_Header
{
    size_t block_size;
    size_t padding;
};

// An intrusive linked list for the free memory blocks
typedef struct Free_List_Node Free_List_Node;

struct Free_List_Node
{
    Free_List_Node* next;
    size_t block_size;
};

;

typedef enum Placement_Policy
{
    Placement_Policy_Find_First,
    Placement_Policy_Find_Best
} Placement_Policy;

typedef struct Free_List Free_List;

struct Free_List
{
    void* data;
    size_t size;
    size_t used;

    Free_List_Node* head;
    Placement_Policy policy;
};


void free_list_node_insert(Free_List_Node** phead, Free_List_Node* prev_node, Free_List_Node* new_node);

void free_list_node_remove(Free_List_Node** phead, Free_List_Node* prev_node, Free_List_Node* del_node);

void free_list_free_all(Free_List* fl);

void free_list_init(Free_List* fl, void* data, size_t size);


// Defined Memory Allocation Strategies Part 3: /article/2019/02/15/memory-allocation-strategies-003/#alloc
size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size);

Free_List_Node* free_list_find_first(Free_List* fl, size_t size, size_t alignment, size_t* padding_,
                                     Free_List_Node** prev_node_);

Free_List_Node* free_list_find_best(Free_List* fl, size_t size, size_t alignment, size_t* padding_,
                                    Free_List_Node** prev_node_);

void* free_list_alloc(Free_List* fl, size_t size, size_t alignment);

void free_list_coalescence(Free_List* fl, Free_List_Node* prev_node, Free_List_Node* free_node);

void* free_list_free(Free_List* fl, void* ptr);

void free_list_coalescence(Free_List* fl, Free_List_Node* prev_node, Free_List_Node* free_node);

void free_list_test();



#endif


