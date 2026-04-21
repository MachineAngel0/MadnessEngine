#ifndef B_TREE_RED_BLACK_H
#define B_TREE_RED_BLACK_H


// 1. a node is either red or black
// 2. the root and leaves (NIL) are black
// 3. if a node is red, then its children are black
// 4. all paths from a node to its NIL descemdamts contain the same number of black nodes

//Notes: the longest path (root to farthest NIL) is no more than twice the length of the shortest path (root to nearest NIL)
// Time complexity:
// Search, insert, remove -> O(log n)
// Rotation -> 0(1)
// Space complexity -> O(n)

typedef enum RB_Color
{
    RB_COLOR_RED,
    RB_COLOR_BLACK,
} RB_Color;

typedef struct RB_Node
{
    struct RB_Node* parent;
    struct RB_Node* left;
    struct RB_Node* right;
    void* data;
    RB_Color color; // red or black
} RB_Node;

typedef struct Red_Black_Tree
{
    RB_Node* root;
    RB_Node* nil; //sentinel node
    int (*cmp)(void*, void*); //compare function -1, less than, 0 equal, 1 greater than
    void (*print)(void*);
} Red_Black_Tree;


RB_Node* red_black_tree_node_create(void* data);


Red_Black_Tree* red_black_tree_create(int (*cmp_function)(void*, void*), void (*print_func)(void*));

void red_black_tree_destroy(Red_Black_Tree* rbt);

void red_black_tree_rotate_left(Red_Black_Tree* rbt, RB_Node* x);

void red_black_tree_rotate_right(Red_Black_Tree* rbt, RB_Node* x);

void red_black_tree_fix_insert(Red_Black_Tree* rbt, RB_Node* z);

void red_black_tree_fix_delete(Red_Black_Tree* rbt, RB_Node* x);

RB_Node* red_black_tree_minimum(Red_Black_Tree* rbt, RB_Node* node);

void red_black_tree_transplant(Red_Black_Tree* rbt, RB_Node* u, RB_Node* v);

RB_Node* red_black_tree_search(Red_Black_Tree* rbt, RB_Node* node, void* data);
void red_black_tree_inorder_traversal(Red_Black_Tree* rbt, RB_Node* node);


void red_black_tree_insert(Red_Black_Tree* rbt, void* data);

void red_black_tree_remove(Red_Black_Tree* rbt, void* data);



void red_black_tree_test();


#endif //B_TREE_RED_BLACK_H
