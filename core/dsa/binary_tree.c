#include "binary_tree.h"

BT_Node* btree_node_create(void* data, size_t data_size)
{
    BT_Node* node = (BT_Node *) malloc(sizeof(BT_Node));

    node->data = malloc(data_size);
    memcpy(node->data, data, data_size);

    node->data_size = data_size;

    node->left = NULL;
    node->right = NULL;

    return node;
}

//TODO:
void btree_free(BT_Node* node)
{
}

//TODO:
//BT_Node* btree_clear(void* data, size_t data_size);


void btree_node_free_individual(BT_Node* node)
{
    node->right = NULL;
    node->left = NULL;
    free(node->data);
    node->data = NULL;
    free(node);
    node = NULL;

}


bool btree_search_recursive(BT_Node* node, void* data, int (*cmp_func)(void*, void*))
{

    //up to me the traversal method, resc
    if (node == NULL)
    {
        printf("NODE NOT FOUND\n");
        return false;
    }

    //-1 if a less than b, 0 equal, 1 if a greater than b
    int cmp_val = cmp_func(data, node->data);
    if (cmp_val == 0)
    {
        printf("NODE FOUND\n");
        return true;
    }
    if (cmp_val > 0)
    {
        return btree_search_recursive(node->right, data, cmp_func);
    }
    if (cmp_val < 0)
    {
        return btree_search_recursive(node->left, data, cmp_func);
    }

    return false;
}


BT_Node* btree_find_min_node(BT_Node* node)
{
    //find the node farthest to the left
    BT_Node* temp = node;
    while (temp->left != NULL)
    {
        temp = temp->left;
    }
    return temp;
}
BT_Node* btree_find_max_node(BT_Node* node)
{
    //find the node farthest to the right
    //find the node farthest to the left
    BT_Node* temp = node;
    while (temp->right != NULL)
    {
        temp = temp->right;
    }
    return temp;
}


BT_Node* btree_insert_recursive(BT_Node* node, void* data, size_t data_size, int (*cmp_func)(void*, void*))
{
    if (node == NULL)
    {
        //insert
        return btree_node_create(data, data_size);
    }

    //-1 if a less than b, 0 equal, 1 if a greater than b
    // if passed in value is greater than node value then we want to proceed right
    const int cmp_val = cmp_func(data, node->data);

    if (cmp_val > 0)
    {
        node->right = btree_insert_recursive(node->right, data, data_size, cmp_func);
    }
    if (cmp_val < 0)
    {
        node->left = btree_insert_recursive(node->left, data, data_size, cmp_func);
    }

    return node;
}

//TODO:
//BT_Node* btree_insert_iterative(BT_Node* node, void* data, size_t data_size, int (*cmp_func)(void*, void*));

BT_Node* btree_delete_recursive(BT_Node* node, void* data, int (*cmp_func)(void*, void*))
{
    if (node == NULL)
    {
        printf("NODE NOT FOUND FOR DELETION\n");
        return NULL;
    }

    //-1 if a less than b, 0 equal, 1 if a greater than b
    // if passed in value is greater than node value then we want to proceed right
    const int cmp_val = cmp_func(data, node->data);
    if (cmp_val > 0)
    {
        node->right = btree_delete_recursive(node->right, data, cmp_func);
    }
    if (cmp_val < 0)
    {
        node->left = btree_delete_recursive(node->left, data, cmp_func);
    }
    if (cmp_val == 0)
    {
        printf("NODE FOUND FOR DELETION\n");

        //TODO: These freeing of memory is probably wrong
        //check null for the left and right combined, then individually
        if (node->left == NULL && node->right == NULL)
        {
            free(node);
            return NULL;
        }
        if (node->left == NULL)
        {
            //delete the current node and return the right node
            BT_Node* temp = node->right;
            free(node);
            node = NULL;
            return temp;
        }
        if (node->right == NULL)
        {
           //delete the current node and return the left node
            BT_Node* temp = node->left;
            free(node);
            node = NULL;
            return temp;
        }
        if (node->left != NULL && node->right != NULL)
        {
            // find the smallest value in the right branch
            BT_Node* temp = btree_find_min_node(node->right);
            // replace the current node we want to delete with the new value
            node->data = temp->data;
            // delete the node temp node, using the delete function, so its also doesn't have children, to preserve the tree
            node->right = btree_delete_recursive(node->right, temp->data, cmp_func);
            return node;
        }

        //Should never reach this point
    }

    return node;
}


//TODO:
//BT_Node* btree_delete_iterative(BT_Node* node, void* data, size_t data_size, int (*cmp_func)(void*, void*));

void btree_inorder_traversal_recursive(BT_Node* node, void (*print_func)(void*))
{
    if (node == NULL) return;

    btree_inorder_traversal_recursive(node->left, print_func);
    print_func(node->data);
    btree_inorder_traversal_recursive(node->right, print_func);
}

void btree_preorder_traversal_recursive(BT_Node* node, void (*print_func)(void*))
{
    if (node == NULL) return;

    print_func(node->data);
    btree_preorder_traversal_recursive(node->left, print_func);
    btree_preorder_traversal_recursive(node->right, print_func);
}

void btree_postorder_traversal_recursive(BT_Node* node, void (*print_func)(void*))
{
    if (node == NULL) return;

    btree_postorder_traversal_recursive(node->left, print_func);
    btree_postorder_traversal_recursive(node->right, print_func);
    print_func(node->data);
}

// TODO:
// void btree_bfs(BT_Node* node); // also called level order traversal

// TODO:
// void btree_inorder_traversal_iterative(BT_Node* node);
// void btree_preorder_traversal_iterative(BT_Node* node, void (*print_func)(void*))
// void btree_postorder_traversal_iterative(BT_Node* node, void (*print_func)(void*))
// int btree_depth(BT_Node* node);
// int btree_node_count(BT_Node* node);
// int btree_serialize(BT_Node* node); //idk anything about this but would be interesting to implement, something ill need to learn how to do anyway


void binary_tree_test()
{

    int num15 = 15;
    int num20 = 20;
    int num30 = 30;
    int num80 = 80;
    int num10 = 10;

    int v[] = {10, 20, 30, 40, 35};
    BT_Node* root = NULL;
    // Insertion of nodes
    for (int i = 0; i < ARRAY_SIZE(v); i++)
    {
        root = btree_insert_recursive(root, &v[i], sizeof(v[i]), cmp_int);
    }



    printf("Inorder traversal: \n");
    btree_inorder_traversal_recursive(root, print_int);
    printf("\n");

    printf("Preorder Traversal traversal: \n");
    btree_preorder_traversal_recursive(root, print_int);
    printf("\n");

    printf("\nPostorderTraversal traversal: \n");
    btree_postorder_traversal_recursive(root, print_int);
    printf("\n");

    /*
    printf("\nlevelorderTraversal traversal: \n");
    levelOrderTraversal(root);
    printf("\n");*/

    printf("SEARCHING: \n");
    btree_search_recursive(root, &num15, cmp_int);
    btree_search_recursive(root, &num20, cmp_int);
    btree_search_recursive(root, &num30, cmp_int);
    btree_search_recursive(root, &num80, cmp_int);
    printf("\n");


    printf("DELETE: \n");
    root = btree_delete_recursive(root, &num10, cmp_int);
    root = btree_delete_recursive(root, &num15, cmp_int);
    root = btree_delete_recursive(root, &num30, cmp_int);
    //should be left with 20,40,50,35
    printf("\n");


    printf("Inorder traversal: \n");
    btree_inorder_traversal_recursive(root, print_int);
    printf("\n");

}
