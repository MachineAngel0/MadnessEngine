#ifndef LINKED_LIST_GENERIC_H
#define LINKED_LIST_GENERIC_H

#include <stdbool.h>
#include <stdlib.h>

//Linked List LL_Node
typedef struct LL_Node
{
    void* data;
    struct LL_Node* next;
} LL_Node;

typedef struct Linked_List
{
    LL_Node* head;

    //NOTE: using the values below does mean that the data will no longer be homogonized
    // but it will be simpler to use, as we only need to pass in size at creation
    // this is a trade off im willing to make, as hetero data types are generally slower

    size_t data_size;
    size_t count;
} Linked_List;

Linked_List* ll_init(size_t data_size);

LL_Node* ll_create_node(void* data, size_t data_size);

//frees entire linked list and all assoicated nodes
void ll_free(Linked_List* linked_list);

//frees just the nodes, not the ll itself, also retains data_size
void ll_clear(Linked_List* linked_list);

size_t ll_get_size(Linked_List* Linked_List);

void ll_print(Linked_List* linked_list, void (*print_func)(void*));

bool ll_search(Linked_List* linked_list, void* data_to_find, int (*cmp_func)(void*, void*));

//NOTE: YOU CAN PASS IN A STACK ALLOCATED VALUE, but not directly into the params
// if it is stack allocated, pass it by ref, otherwise if pointer/heap pass directly
void ll_prepend(Linked_List* linked_list, void* data);

void ll_append(Linked_List* linked_list, void* data);

void ll_insert_before_pos(Linked_List* linked_list, void* data, size_t pos);

void ll_insert_after_pos(Linked_List* linked_list, void* data, size_t pos);
;
void ll_remove_first(Linked_List* linked_list);

void ll_remove_last(Linked_List* linked_list);

// void ll_remove_at(Linked_List* linked_list, int pos){};


void ll_bubble_sort(Linked_List* linked_list, int (*cmp_func)(void*, void*));

//TODO: point new node to the node after cur, then point cur to the new node
// void ll_insert_sort(Linked_List* linked_list, int pos);
// void ll_merge_sort(Linked_List* linked_list, int pos);

void ll_reverse(Linked_List* linked_list);

void linked_list_test();


#endif //LINKED_LIST_GENERIC_H
