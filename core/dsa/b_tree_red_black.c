#include "b_tree_red_black.h"

RB_Node* red_black_tree_node_create(void* data)
{
    RB_Node* node = malloc(sizeof(RB_Node));
    node->color = RB_COLOR_RED;
    node->data = data; // TODO: do we allocate memory for this? or keep it as a reference
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    return node;
}

Red_Black_Tree* red_black_tree_create(int (*cmp_function)(void*, void*),  void (*print_func)(void*))
{
    Red_Black_Tree* rbt = malloc(sizeof(Red_Black_Tree));

    rbt->root = red_black_tree_node_create(NULL);
    rbt->nil = red_black_tree_node_create(NULL);
    rbt->nil->color = RB_COLOR_BLACK;
    rbt->nil->left = rbt->nil;
    rbt->nil->right = rbt->nil;
    rbt->nil->parent = rbt->nil;
    rbt->cmp = cmp_function;
    rbt->print = print_func;

    return rbt;
}

static void free_nodes(Red_Black_Tree *tree, RB_Node *n) {
    if (n == tree->nil) return;
    free_nodes(tree, n->left);
    free_nodes(tree, n->right);
    free(n);
}

void red_black_tree_destroy(Red_Black_Tree* rbt)
{
    free_nodes(rbt, rbt->root);

    free(rbt->nil);
    free(rbt);
}

void red_black_tree_rotate_left(Red_Black_Tree* rbt, RB_Node* x)
{
    // Identify the node to be promoted
    RB_Node* y = x->right;

    // 1. HANDOVER: y's left subtree becomes x's right child
    x->right = y->left;
    if (y->left != rbt->nil)
    {
        y->left->parent = x;
    }

    // 2. PARENT LINK: Connect y to the rest of the tree
    y->parent = x->parent;

    if (x->parent == rbt->nil)
    {
        rbt->root = y; // x was the root
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y; // x was a left child
    }
    else
    {
        x->parent->right = y; // x was a right child
    }

    // 3. PIVOT: Finalize the new parent-child bond
    y->left = x;
    x->parent = y;
}

void red_black_tree_rotate_right(Red_Black_Tree* rbt, RB_Node* x)
{
    // Identify the node to be promoted (the left child)
    RB_Node* y = x->left;

    // 1. HANDOVER: y's right subtree becomes x's left child
    x->left = y->right;
    if (y->right != rbt->nil)
    {
        y->right->parent = x;
    }

    // 2. PARENT LINK: Connect y to the rest of the tree
    y->parent = x->parent;

    if (x->parent == rbt->nil)
    {
        rbt->root = y; // x was the root
    }
    else if (x == x->parent->right)
    {
        x->parent->right = y; // x was a right child
    }
    else
    {
        x->parent->left = y; // x was a left child
    }
    // 3. PIVOT: Finalize the new parent-child bond
    y->right = x;
    x->parent = y;
}

void red_black_tree_fix_insert(Red_Black_Tree* rbt, RB_Node* z)
{
    // # Case: Parent is red, needing adjustment
    while (z->parent && z->parent->color == RB_COLOR_RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            RB_Node* y = z->parent->parent->right; // Uncle node
            if (y->color == RB_COLOR_RED)
            {
                // # Case 2: Both parent and uncle are red
                z->parent->color = RB_COLOR_BLACK;
                y->color = RB_COLOR_BLACK;
                z->parent->parent->color = RB_COLOR_RED;
                z = z->parent->parent; // Recurse upward
            }
            else
            {
                // # Case 3: Parent is red, uncle is black, and z is a right child
                if (z == z->parent->right)
                {
                    z = z->parent;
                    red_black_tree_rotate_left(rbt, z); //  # Left-rotate to correct shape
                }

                // # Case 3: Left-rotation done, recolor and rotate
                z->parent->color = RB_COLOR_BLACK;
                z->parent->parent->color = RB_COLOR_RED;
                red_black_tree_rotate_right(rbt, z->parent->parent); //# Right-rotate to fix violation
            }
        }
        else
        {
            // # Symmetric cases for when z's parent is the right child
            RB_Node* y = z->parent->parent->left;
            if (y->color == RB_COLOR_RED)
            {
                // # Case 2: Parent and uncle are both red
                z->parent->color = RB_COLOR_BLACK;
                y->color = RB_COLOR_BLACK;
                z->parent->parent->color = RB_COLOR_RED;
                z = z->parent->parent;
            }
            else
            {
                // # Case 3: Parent is red, uncle is black, and z is a left child
                if (z == z->parent->left)
                {
                    z = z->parent;
                    red_black_tree_rotate_right(rbt, z);
                }
                // # Case 3: Recoloring and left-rotation
                z->parent->color = RB_COLOR_BLACK;
                z->parent->parent->color = RB_COLOR_RED;
                red_black_tree_rotate_left(rbt, z->parent->parent);
            }
        }
    }
    // # Case 1: Root is always black after insertion fix
    rbt->root->color = RB_COLOR_BLACK;
}

void red_black_tree_fix_delete(Red_Black_Tree* rbt, RB_Node* x)
{
    while (x != rbt->root && x->color == RB_COLOR_BLACK)
    {
        if (x == x->parent->left)
        {
            RB_Node* w = x->parent->right; // # Sibling node
            if (w->color == RB_COLOR_RED)
            {
                // # Case 1: Sibling is red
                w->color = RB_COLOR_BLACK;;
                x->parent->color = RB_COLOR_RED;
                red_black_tree_rotate_left(rbt, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == RB_COLOR_BLACK && w->right->color == RB_COLOR_BLACK)
            {
                // # Case 2: Sibling and its children are black
                w->color = RB_COLOR_RED;
                x = x->parent; //# Move up the tree
            }
            else
            {
                if (w->right->color == RB_COLOR_BLACK)
                {
                    // # Case 3: Sibling is black, left child is red, right is black
                    w->left->color = RB_COLOR_BLACK;
                    w->color = RB_COLOR_RED;
                    red_black_tree_rotate_right(rbt, w);
                    w = x->parent->right;
                }
                // # Case 3: Right child of sibling is red
                w->color = x->parent->color;
                x->parent->color = RB_COLOR_BLACK;
                w->right->color = RB_COLOR_BLACK;
                red_black_tree_rotate_left(rbt, x->parent);
                x = rbt->root;
            }
        }
        else
        {
            //# Symmetric cases for when x is the right child
            RB_Node* w = x->parent->left;
            if (w->color == RB_COLOR_RED)
            {
                // # Case 1: Sibling is red
                w->color = RB_COLOR_BLACK;
                x->parent->color = RB_COLOR_RED;
                red_black_tree_rotate_right(rbt, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == RB_COLOR_BLACK && w->left->color == RB_COLOR_BLACK)
            {
                // # Case 2: Sibling and its children are black
                w->color = RB_COLOR_RED;
                x = x->parent;
            }
            else if (w->left->color == RB_COLOR_BLACK)
            {
                //# Case 3: Sibling is black, right child is red, left is black
                w->right->color = RB_COLOR_BLACK;;
                w->color = RB_COLOR_RED;
                red_black_tree_rotate_left(rbt, w);
                w = x->parent->left;
            }
            // # Case 3: Left child of sibling is red
            w->color = x->parent->color;
            x->parent->color = RB_COLOR_BLACK;;
            w->left->color = RB_COLOR_BLACK;;
            red_black_tree_rotate_right(rbt, x->parent);
            x = rbt->root;
        }
    }
    // # Ensure the final node is black
    x->color = RB_COLOR_BLACK;;
}

RB_Node* red_black_tree_minimum(Red_Black_Tree* rbt, RB_Node* node)
{
    while (node->left != rbt->nil)
    {
        node = node->left;
    }
    return node;
}

void red_black_tree_transplant(Red_Black_Tree* rbt, RB_Node* u, RB_Node* v)
{
    if (u->parent == rbt->nil)
    {
        rbt->root = v;
    }
    else if (u == u->parent->left)
    {
        u->parent->left = v;
    }
    else
    {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

RB_Node* red_black_tree_search(Red_Black_Tree* rbt, RB_Node* node, void* data)
{
    if (node == rbt->nil)
    {
        return node;
    }

    //a less than b = -1
    //-1 if a less than b, 0 equal, 1 if a greater than b
    int cmp_val = rbt->cmp(data, node->data);
    if (cmp_val == 0)
    {
        printf("NODE FOUND\n");
        return node;
    }
    if (cmp_val > 0)
    {
        return red_black_tree_search(rbt, node->right, data);
    }
    if (cmp_val < 0)
    {
        return red_black_tree_search(rbt, node->left, data);
    }
    return NULL;
}

void red_black_tree_inorder_traversal(Red_Black_Tree* rbt, RB_Node* node)
{
    if (node == rbt->nil) return;
    red_black_tree_inorder_traversal(rbt, node->left);
    rbt->print(node->data);
    red_black_tree_inorder_traversal(rbt, node->right);

}

void red_black_tree_insert(Red_Black_Tree* rbt, void* data)
{
    // insert node, and color it red
    // recolor and rotate nodes to fix voilations
    // 4 scenarios
    // 1. z = root -> color it black
    // 2. z.uncle = red -> recolor z's parent, grandparent, and uncle
    // 3. z.uncle = black(triangle) ->   rotate z.parent
    // 4. z.uncle = black(line) ->   rotate z.grandparent and recolor

    RB_Node* new_node = red_black_tree_node_create(data);

    RB_Node* parent = NULL;
    RB_Node* current = rbt->root;

    while (current != rbt->nil)
    {
        parent = current;
        if (new_node->data < current->data)
        {
            current = current->left;
        }
        else
        {
            current = current->right;
        }
    }

    new_node->parent = parent;
    if (parent == rbt->nil)
    {
        rbt->root = new_node;
    }
    else if (new_node->data < parent->data)
    {
        parent->left = new_node;
    }
    else
    {
        parent->right = new_node;
    }

    red_black_tree_fix_insert(rbt, new_node);
}

void red_black_tree_remove(Red_Black_Tree* rbt, void* data)
{
    RB_Node* z = red_black_tree_search(rbt, rbt->root, data);
    if (z == rbt->nil)
    {
        DEBUG("Value not found in the tree.")
        return;
    }

    RB_Node* y = z;
    RB_Color y_original_color = y->color;
    RB_Node* x;

    if (z->left == rbt->nil)
    {
        x = z->right;
        red_black_tree_transplant(rbt, z, z->right);
    }
    else if (z->right == rbt->nil)
    {
        x = z->left;
        red_black_tree_transplant(rbt, z, z->left);
    }
    else
    {
        y = red_black_tree_minimum(rbt, z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z)
        {
            x->parent = y;
        }
        else
        {
            red_black_tree_transplant(rbt, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        red_black_tree_transplant(rbt, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_original_color == RB_COLOR_BLACK)
    {
        red_black_tree_fix_delete(rbt, x);
    }
}

void red_black_tree_test()
{
    TEST_START("RED BLACK TREE START");


    TEST_END("RED BLACK TREE END");
}
