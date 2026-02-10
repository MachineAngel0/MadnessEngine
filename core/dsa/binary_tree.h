#ifndef BINARY_TREE_H
#define BINARY_TREE_H
#include <stdbool.h>
#include <stddef.h>


typedef struct BT_Node
{
    void* data;
    size_t data_size;
    struct BT_Node* left;
    struct BT_Node* right;
} BT_Node;

//TODO: REFACTOR TO USE THIS STRUCT REPRESENTATION
typedef struct Binary_Tree
{
    BT_Node* root_left;
    BT_Node* root_right;
    size_t data_size;
} Binary_Tree;

typedef struct Binary_Tree_Func // TODO: could also just call this DSA_FUNC
{
    //if I wanted to, I could initialize a bunch of these with their corresponding type,
    //as global variables, and pass them into the functions that need them

    //NOTE: I CANT BELIEVE YOU CAN STORE FUNCTION POINTERS AS VARIABLES THAT'S SO COOL
    void (*print_func)(void*);
    int (*cmp)(void*, void*);
} Binary_Tree_Func;


BT_Node* btree_node_create(void* data, size_t data_size);

//TODO:
void btree_free(BT_Node* node);

//TODO:
//BT_Node* btree_clear(void* data, size_t data_size);


void btree_node_free_individual(BT_Node* node);


bool btree_search_recursive(BT_Node* node, void* data, int (*cmp_func)(void*, void*));


BT_Node* btree_find_min_node(BT_Node* node);
BT_Node* btree_find_max_node(BT_Node* node);

BT_Node* btree_insert_recursive(BT_Node* node, void* data, size_t data_size, int (*cmp_func)(void*, void*));

//TODO:
//BT_Node* btree_insert_iterative(BT_Node* node, void* data, size_t data_size, int (*cmp_func)(void*, void*));

BT_Node* btree_delete_recursive(BT_Node* node, void* data, int (*cmp_func)(void*, void*));


//TODO:
//BT_Node* btree_delete_iterative(BT_Node* node, void* data, size_t data_size, int (*cmp_func)(void*, void*));

void btree_inorder_traversal_recursive(BT_Node* node, void (*print_func)(void*));

void btree_preorder_traversal_recursive(BT_Node* node, void (*print_func)(void*));
void btree_postorder_traversal_recursive(BT_Node* node, void (*print_func)(void*));
// TODO:
// void btree_bfs(BT_Node* node); // also called level order traversal

// TODO:
// void btree_inorder_traversal_iterative(BT_Node* node);
// void btree_preorder_traversal_iterative(BT_Node* node, void (*print_func)(void*))
// void btree_postorder_traversal_iterative(BT_Node* node, void (*print_func)(void*))
// int btree_depth(BT_Node* node);
// int btree_node_count(BT_Node* node);
// int btree_serialize(BT_Node* node); //idk anything about this but would be interesting to implement, something ill need to learn how to do anyway


void binary_tree_test();

#endif //BINARY_TREE_H
