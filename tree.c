/*
 *  Andromeda - AVL tree
 *  Copyright (C) 2011  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tree.h>

static int tree_depth(struct tree* tree)
{
        if (tree == NULL)
                return NULL_PTR;

        if (tree->left != NULL)
        {
                int a = tree->left->ldepth;
                int b = tree->left->rdepth;
                tree->ldepth = ((a > b) ? a : b) + 1;
        }
        else
                tree->ldepth = 0;
        if (tree->right != NULL)
        {
                int a = tree->right->ldepth;
                int b = tree->right->rdepth;
                tree->rdepth = ((a > b) ? a : b) + 1;
        }
        else
                tree->rdepth = 0;
}

int tree_rotate_right(struct tree* tree)
{
        if (tree == NULL)
                return NULL_PTR;

        struct tree* parent = tree->parent;
        struct tree* right = tree->right;
        struct tree* left = tree->left;

        left->parent = parent;
        tree->left = left->right;
        left->right = tree;
        tree->parent = left;

        tree_depth(tree);
        tree_depth(left);

        if (parent == NULL)
                tree->root->tree = left;
        else
        {
                if (tree->key < parent->key)
                        parent->left = left;
                else
                        parent->right = left;
                tree_depth(parent);
        }

        return EXIT_SUCCESS;
}

int tree_rotate_left(struct tree* tree)
{
        if (tree == NULL)
                return NULL_PTR;

        struct tree* parent = tree->parent;
        struct tree* right = tree->right;
        struct tree* left = tree->left;

        right->parent = parent;
        tree->right = right->left;
        right->left = tree;
        tree->parent = right;

        if (parent == NULL)
                tree->root->tree = right;
        else
        {
                if (tree->key < parent->key)
                        parent->left = right;
                else
                        parent->right = right;
        }

        tree_depth(tree);
        tree_depth(right);

        return EXIT_SUCCESS;
}

int tree_balance(struct tree* tree)
{
        if (tree == NULL)
                return NULL_PTR;

        printf("tree balance: %d\n", TREE_BALANCE(tree));
        printf("left: %d\tright: %d\n", tree->ldepth, tree->rdepth);

        switch (TREE_BALANCE(tree))
        {
        case -2:
                printf("Rotating: Imbalance -2\n");
                if (TREE_BALANCE(tree->right) == 1)
                {
                        printf("double rotation, right first\n");
                        tree_rotate_right(tree->right);
                }
                printf("Left rotate.\n");
                tree_rotate_left(tree);
                break;
        case 2:
                printf("Rotating: Imbalance 2\n");
                if (TREE_BALANCE(tree->left) == -1)
                {
                        printf("double rotation, left first\n");
                        tree_rotate_left(tree->left);
                }
                printf("Right rotate.\n");
                tree_rotate_right(tree);
                break;
        case -1:
        case 0:
        case 1:
        default:
                printf("Not rotating!!!\n");
                break;
        }
        return EXIT_SUCCESS;
}

int tree_add_node(struct tree* parent, struct tree* t)
{
        if (parent == NULL || t == NULL)
                return NULL_PTR;

        if (t->key < parent->key)
        {
                int s = tree_add_node(parent->left, t);
                switch (s) {
                case NULL_PTR:
                        parent->left = t;
                        t->parent = parent;
                        printf("Adding node left!\n");
                case EXIT_SUCCESS:
                        break;
                default:
                        return s;
                }
        }
        else if(t->key > parent->key)
        {
                int s = tree_add_node(parent->right, t);
                switch (s)
                {
                case NULL_PTR:
                        parent->right = t;
                        t->parent = parent;
                        printf("Adding node right!\n");
                case EXIT_SUCCESS:
                        break;
                default:
                        return s;
                }
        }
        else
                return TREE_CONFLICT;

        tree_depth(parent);
        tree_balance(parent);

        return EXIT_SUCCESS;
}

int tree_add(struct tree_root* root, struct tree* tree)
{
        if (root == NULL || tree == NULL)
                return NULL_PTR;

        if (root->tree != NULL)
                return tree_add_node(root->tree, tree);

        root->tree = tree;
        return EXIT_SUCCESS;
}

struct tree* tree_new_node(int key, void* data, struct tree_root* root)
{
        struct tree* t = malloc(sizeof(*t));
        if (t == NULL)
                return NULL;
        memset(t, 0, sizeof(*t));
        t->key = key;
        t->data = data;
        t->root = root;

        if (root == NULL)
                return t;

        return (tree_add(root, t) == EXIT_SUCCESS) ? root->tree : t;
}

struct tree_root* tree_new()
{
        struct tree_root* t = malloc(sizeof(*t));
        if (t != NULL)
                memset(t, 0, sizeof(*t));
        return t;
}

struct tree* tree_find_node(int key, struct tree* tree)
{
        if (tree == NULL)
                return NULL;

        if (tree->key == key)
                return tree;


        if (key < tree->key)
                return tree_find_node(key, tree->left);
        else
                return tree_find_node(key, tree->right);
}

struct tree* tree_find(int key, struct tree_root* t)
{
        if (t == NULL)
                return NULL;

        return tree_find_node(key, t->tree);
}

struct tree* tree_inorder_successor(struct tree* tree)
{
        if (tree == NULL)
                return NULL;


        struct tree* tmp = tree_inorder_successor(tree->left);
        if (tmp == NULL)
                return tree;

        return tmp;
}

int tree_delete_node(int key, struct tree* tree)
{
        if (tree == NULL)
                return NULL_PTR;

        struct tree* t = tree;
        if (tree->key != key)
        {
                if (key < tree->key)
                {
                        key = tree_delete_node(key, tree->left);
                        tree_depth(tree);
                        return key;
                }
                else
                {
                        key = tree_delete_node(key, tree->right);
                        tree_depth(tree);
                        return key;
                }
        }
        printf("Found the damn basterd!\n");
        if (t == NULL)
                return TREE_NOTFOUND;

        if (t->ldepth == 0 && t->rdepth == 0)
        {
                /* If neither of the subtrees are present, tested */
                if (t->key < t->parent->key)
                        t->parent->left = NULL;
                else
                        t->parent->right = NULL;
        }
        else if (t->right == NULL && t->left != NULL)
        {
                /* if only left subtree is present, still untested */
                if (t->key < t->parent->key)
                        t->parent->left = t->left;
                else
                        t->parent->right = t->left;
        }
        else if (t->right != NULL && t->left == NULL)
        {
                /* If only right subtree is present, still untested */
                if (t->key < t->parent->key)
                        t->parent->left = t->right;
                else
                        t->parent->right = t->right;
        }
        else
        {
                /* If both subtrees are present ... Tested */
                struct tree* successor = tree_inorder_successor(t->right);
                if (successor == NULL)
                        return NULL_PTR;

                printf("Successor: %X\n", successor->key);

                struct tree* walker = successor->parent;

                /* Detach successor */
                successor->parent->left = successor->right;
                tree_depth(successor->parent);


                /* Set up successor */
                successor->parent = t->parent;
                if (t->right == successor)
                        successor->right = NULL;
                else
                        successor->right = t->right;
                successor->left = t->left;
                if (successor->parent == NULL)
                        successor->root->tree = successor;
                else if (successor->key < successor->parent->key)
                        successor->parent->left = successor;
                else
                        successor->parent->right = successor;

                if (successor->right != NULL)
                        successor->right->parent = successor;
                if (successor->left != NULL)
                        successor->left->parent = successor;

                /* Update meta data and balance */

                tree_depth(successor);
                tree_depth(successor->parent);
                printf("Attempting balance!\n");
                if (walker != t)
                {
                        while(walker != NULL && walker != successor)
                        {
                                printf("Balancing: %X\n", walker->key);
                                tree_depth(walker);
                                tree_balance(walker);
                                walker = walker->parent;
                        }
                }
                printf("Stuff complete!\n");
        }
        /* Free the deleted node */
        tree_depth(t->parent);
        tree_balance(t->parent);
        memset(t, 0, sizeof(*t));
        free(t);
        return EXIT_SUCCESS;
}

int tree_delete(int idx, struct tree_root* root)
{
        if (root == NULL)
                return NULL_PTR;
        return tree_delete_node(idx, root->tree);
}

int tree_dump_node(struct tree* tree)
{
        if (tree == NULL)
        {
                printf("null");
                return NULL_PTR;
        }

        printf("d:[");
        printf("%X,%X,%X", tree->ldepth, tree->key, tree->rdepth);
        printf("]");
        if (tree->left != NULL)
        {
                printf("l:[");
                tree_dump_node(tree->left);
                printf("]");
        }
        if (tree->right != NULL)
        {
                printf("r:[");
                tree_dump_node(tree->right);
                printf("]");
        }
        return EXIT_SUCCESS;
}

int tree_dump(struct tree_root* root)
{
        if (root == NULL)
                return NULL_PTR;

        printf("Dumping tree!\n");
        tree_dump_node(root->tree);
        printf("\nDone dumping tree!\n");

        return EXIT_SUCCESS;
}

int main()
{
        struct tree_root* t = tree_new();
        if (t == NULL)
        {
                fprintf(stderr, "An error occured, no memory!\n");
                return -1;
        }
        int i = 0;

        for (; i <= 16; i++)
        {
                tree_new_node(i, NULL, t);
                tree_dump(t);
        }
        tree_delete(8, t);
        tree_delete(9, t);
        tree_dump(t);
        for (; i <= 32; i++)
        {
                tree_new_node(i, NULL, t);
                tree_dump(t);
        }
        tree_new_node(8, NULL, t);
        tree_new_node(9, NULL, t);
        tree_dump(t);

        /*
        tree_new_node (3, NULL, t);
        tree_new_node (2, NULL, t);
        tree_dump(t);
        tree_new_node (1, NULL, t);
        tree_dump(t);
        tree_new_node (0, NULL, t);
        tree_new_node (10, NULL, t);
        tree_dump(t);
        tree_new_node (8, NULL, t);
        tree_dump(t);
        */
        return EXIT_SUCCESS;
}
