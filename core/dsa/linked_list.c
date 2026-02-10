#include "linked_list.h"

Linked_List* ll_init(size_t data_size)
{
    Linked_List* new_linked_list = (Linked_List *) malloc(sizeof(Linked_List));
    new_linked_list->head = NULL;
    new_linked_list->data_size = data_size;
    new_linked_list->count = 0;

    return new_linked_list;
}


LL_Node* ll_create_node(void* data, size_t data_size)
{
    LL_Node* new_node = (LL_Node *) malloc(sizeof(LL_Node));
    new_node->data = malloc(data_size);
    new_node->next = NULL;

    //copy data over so that the linked list owns the data
    memcpy(new_node->data, data, data_size);

    return new_node;
}

//frees entire linked list and all assoicated nodes
void ll_free(Linked_List* linked_list)
{
    //nothing to free
    if (linked_list == NULL) return;

    LL_Node* current = linked_list->head;
    LL_Node* next = NULL;

    while (current != NULL)
    {
        next = current->next;
        //free data, then the struct
        free(current->data);
        free(current);
        //move onto the next value
        current = next;
    }

    free(linked_list);
}


//frees just the nodes, not the ll itself, also retains data_size
void ll_clear(Linked_List* linked_list)
{
    //nothing to free
    if (linked_list == NULL) return;

    LL_Node* current = linked_list->head;
    LL_Node* next = NULL;

    while (current != NULL)
    {
        next = current->next;
        //free data, then the struct
        free(current->data);
        free(current);
        //move onto the next value
        current = next;
    }

    linked_list->count = 0;
}


size_t ll_get_size(Linked_List* Linked_List)
{
    return Linked_List->count;
}


void ll_print(Linked_List* linked_list, void (*print_func)(void*))
{
    LL_Node* current = linked_list->head;

    while (current != NULL)
    {
        print_func(current->data);
        current = current->next;
    }
    printf("\n");
}

bool ll_search(Linked_List* linked_list, void* data_to_find, int (*cmp_func)(void*, void*))
{
    LL_Node* current = linked_list->head;

    while (current != NULL)
    {
        // checking if the values are 0,
        // -1 a is less than b, 1 a is greater than b
        if (cmp_func(current->data, data_to_find) == 0) return true;;
    }

    return false;
}


//NOTE: YOU CAN PASS IN A STACK ALLOCATED VALUE, but not directly into the params
// if it is stack allocated, pass it by ref, otherwise if pointer/heap pass directly
void ll_prepend(Linked_List* linked_list, void* data)
{
    LL_Node* new_node = ll_create_node(data, linked_list->data_size);
    if (linked_list->head == NULL)
    {
        linked_list->head = new_node;
        return;
    };

    new_node->next = linked_list->head;
    linked_list->head = new_node;

    linked_list->count++;
}


void ll_append(Linked_List* linked_list, void* data)
{
    LL_Node* new_node = ll_create_node(data, linked_list->data_size);
    if (linked_list->head == NULL)
    {
        linked_list->head = new_node;
        return;
    };

    LL_Node* current = linked_list->head;

    while (current->next != NULL)
    {
        current = current->next;
    }

    current->next = new_node;

    linked_list->count++;
}


void ll_insert_before_pos(Linked_List* linked_list, void* data, size_t pos)
{
    if (pos > linked_list->count)
    {
        //TODO: WARN
        printf("INVALID POSITION: INSERT BEFORE POS");
        return;
    }

    if (linked_list->head == NULL)
    {
        //TODO: WARN
        printf("NULL LINKED LIST: INSERT BEFORE POS");
        return;
    };


    //we want the node before pos - 1
    if (pos == 0)
    {
        ll_prepend(linked_list, data);
        return;
    }

    //our pos is at or greater than 1
    LL_Node* current = linked_list->head;
    while (pos != 1)
    {
        current = current->next;
        pos--;
    }


    //point new node to the node after cur, then point cur to the new node
    LL_Node* next = current->next;
    LL_Node* new_node = ll_create_node(data, linked_list->data_size);
    new_node->next = next;
    current->next = new_node;
    linked_list->count++;
}


void ll_insert_after_pos(Linked_List* linked_list, void* data, size_t pos)
{
    if (pos > linked_list->count)
    {
        //TODO: WARN
        printf("INVALID POSITION: INSERT AFTER POS");
        return;
    }

    if (linked_list->head == NULL)
    {
        //TODO: WARN
        printf("NULL LINKED LIST: INSERT AFTER POS");
        return;
    };


    //we want the node at pos
    if (pos == 0)
    {
        //set a node to the node after head
        //have a new node point to that the next node
        //have the head point to the new node
        LL_Node* next = linked_list->head->next;
        LL_Node* new_node = ll_create_node(data, linked_list->data_size);
        new_node->next = next;
        linked_list->head->next = new_node;
        linked_list->count++;
        return;
    }

    LL_Node* current = linked_list->head;
    while (pos != 0)
    {
        current = current->next;
        pos--;
    }

    //point new node to the node after cur, then point cur to the new node
    LL_Node* next = current->next;
    LL_Node* new_node = ll_create_node(data, linked_list->data_size);
    new_node->next = next;
    current->next = new_node;
    linked_list->count++;
}

void ll_remove_first(Linked_List* linked_list)
{
    if (linked_list->head == NULL)
    {
        //TODO: warn
        return;
    }
    LL_Node* new_head = linked_list->head->next;

    linked_list->head->next = NULL;
    free(linked_list->head->data);
    linked_list->head->data = NULL;

    linked_list->head = new_head;
    linked_list->count--;
}

void ll_remove_last(Linked_List* linked_list)
{
    if (linked_list->head == NULL)
    {
        //TODO: warn
        return;
    }


    LL_Node* current = linked_list->head;
    LL_Node* prev = current;

    while (current->next != NULL)
    {
        prev = current;
        current = current->next;
    }

    //prev is pointing to the last node in the list

    current->next = NULL;
    free(current->data);
    free(current);

    prev->next = NULL;

    linked_list->count--;
}

// void ll_remove_at(Linked_List* linked_list, int pos){};


void ll_bubble_sort(Linked_List* linked_list, int (*cmp_func)(void*, void*) )
{
    for (int i = 0; i < linked_list->count; i++)
    {
        LL_Node* current = linked_list->head;
        //we take the current node and check it with the next node
        //then compare and swap
        while (current->next != NULL)
        {
            //the cmp function return -1,0,1
            if (cmp_func(current->data, current->next->data) > 0)
            {
                swap(current, current->next, linked_list->data_size);
            }
            current = current->next;
        }
    }
}


//TODO: point new node to the node after cur, then point cur to the new node
// void ll_insert_sort(Linked_List* linked_list, int pos);
// void ll_merge_sort(Linked_List* linked_list, int pos);

void ll_reverse(Linked_List* linked_list)
{
    LL_Node* prev = NULL;
    LL_Node* next = NULL;

    while (linked_list->head != NULL)
    {
        next = linked_list->head->next;
        linked_list->head->next = prev;
        prev = linked_list->head;
        linked_list->head = next;
    }
    linked_list->head = prev;
}


void linked_list_test()
{
    //TODO: add in asserts after i make my logger library

    /* so this does work
    int anum = 150;
    Linked_List* test = ll_init_test(sizeof(anum), print_int);
    ll_append(test, &anum);
    ll_print_test(test);
    printf("LINKED LIST START\n\n");
    */

    printf("LINKED LIST CREATE START\n");

    int num = 1;
    int num2 = 3;
    int num3 = 2;
    Linked_List* ll_node_int = ll_init(sizeof(num));
    printf("LINKED LIST CREATE END\n\n");

    //linked_list_print(ll_node_test);
    printf("LINKED LIST prepend START\n");
    ll_prepend(ll_node_int, &num);
    ll_prepend(ll_node_int, &num2);
    ll_prepend(ll_node_int, &num3);
    ll_print(ll_node_int, print_int);
    printf("LINKED LIST prepend END\n\n");

    printf("LINKED LIST APPEND START\n");
    ll_append(ll_node_int, &num3);
    ll_append(ll_node_int, &num2);
    ll_append(ll_node_int, &num);
    ll_print(ll_node_int, print_int);
    printf("LINKED LIST APPEND END\n\n");

    printf("LINKED LIST INSERT BEFORE POS START\n");
    int num4 = 4;
    ll_insert_before_pos(ll_node_int, &num4, 1);
    ll_print(ll_node_int, print_int);
    ll_print(ll_node_int, print_int);
    printf("LINKED LIST INSERT BEFORE POS END\n\n");

    printf("LINKED LIST INSERT AFTER POS START\n");
    int num5 = 5;
    ll_insert_before_pos(ll_node_int, &num5, 0);
    ll_insert_before_pos(ll_node_int, &num5, 2);
    ll_print(ll_node_int, print_int);
    printf("LINKED LIST INSERT AFTER POS END\n\n");

    printf("LINKED LIST REMOVE FIRST START\n");
    //ll_bubble_sort(ll_node_int, cmp_int);
    ll_remove_first(ll_node_int);
    ll_remove_first(ll_node_int);
    ll_remove_first(ll_node_int);
    ll_print(ll_node_int, print_int);
    printf("LINKED LIST REMOVE FIRST END\n\n");

    printf("LINKED LIST REMOVE LAST START\n");
    //ll_bubble_sort(ll_node_int, cmp_int);
    ll_remove_last(ll_node_int);
    ll_remove_last(ll_node_int);
    ll_print(ll_node_int, print_int);
    printf("LINKED LIST REMOVE LAST END\n\n");

    printf("LINKED LIST REVERSE START\n");
    //ll_bubble_sort(ll_node_int, cmp_int);
    ll_reverse(ll_node_int);
    ll_print(ll_node_int, print_int);
    printf("LINKED LIST REVERSE END\n\n");


    printf("LINKED LIST FREE START\n");
    ll_free(ll_node_int);
    printf("LINKED LIST FREE END \n\n");

    printf("LINKED LIST END\n");


    char* string = malloc(sizeof(char));
    *string = 'v';
    char* string2 = malloc(sizeof(char));
    *string2 = 'a';
    Linked_List* ll_node_string = ll_init(sizeof(string));
    ll_append(ll_node_string, string);
    ll_prepend(ll_node_string, string2);
    //swap(ll_node_string->head->next, ll_node_string->head, ll_node_string->data_size); // it works
    //ll_bubble_sort(ll_node_string, cmp_char); // it works
    ll_print(ll_node_string, print_char);
    ll_free(ll_node_string);
    //these need to be allocated seperatly as the linked list does not own the passed in values, as it makes a copy
    free(string);
    free(string2);

    printf("\n");
}


