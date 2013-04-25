/*
 *  Andromeda - AVL tree
 *  Copyright (C) 2013  Bart Kuivenhoven
 *
 *  This program is kfree software: you can redistribute it and/or modify
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
#include <lib/tree.h>
#include <andromeda/system.h>

/**
 * \addtogroup tree
 * @{
 * \fn tree_depth
 * \brief Recalculate the depth counters of this tree
 * \param tree
 */
static int tree_depth(struct tree* tree)
{
        if (tree == NULL)
                return -E_NULL_PTR;

        /* Update left counter */
        if (tree->left != NULL)
        {
                int a = tree->left->ldepth;
                int b = tree->left->rdepth;
                tree->ldepth = ((a > b) ? a : b) + 1;
        }
        else
                tree->ldepth = 0;

        /* Update right counter */
        if (tree->right != NULL)
        {
                int a = tree->right->ldepth;
                int b = tree->right->rdepth;
                tree->rdepth = ((a > b) ? a : b) + 1;
        }
        else
                tree->rdepth = 0;

        /* And we're done again */
        return -E_SUCCESS;
}

/**
 * \fn tree_rotate_right
 * \brief Rotate right with the root at tree
 * \param tree
 */
static int tree_rotate_right(struct tree* tree)
{
        if (tree == NULL)
                return -E_NULL_PTR;

        struct tree* parent = tree->parent;
        struct tree* left = tree->left;

        /* The rotating bit */
        left->parent = parent;
        tree->left = left->right;
        left->right = tree;
        tree->parent = left;
        if (tree->left != NULL)
                tree->left->parent = tree;

        /* Update the parents on the new status */
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

        /* Reconsider the depth */
        tree_depth(tree);
        tree_depth(left);

        return -E_SUCCESS;
}

/**
 * \fn tree_rotate_left
 * \brief Rotate left with tree as root
 * \param tree
 */
static int tree_rotate_left(struct tree* tree)
{
        if (tree == NULL)
                return -E_NULL_PTR;

        struct tree* parent = tree->parent;
        struct tree* right = tree->right;

        /* The rotating bit */
        right->parent = parent;
        tree->right = right->left;
        right->left = tree;
        tree->parent = right;
        if (tree->right != NULL)
                tree->right->parent = tree;

        /* Update the parents on the new situation */
        if (parent == NULL)
                tree->root->tree = right;
        else
        {
                if (tree->key < parent->key)
                        parent->left = right;
                else
                        parent->right = right;
        }

        /* Reconsider the depth */
        tree_depth(tree);
        tree_depth(right);

        return -E_SUCCESS;
}

/**
 * \fn tree_balance
 * \brief Balance tree at this node
 * \param tree
 */
static int tree_balance(struct tree* tree)
{
        if (tree == NULL)
                return -E_NULL_PTR;

        /* Determine the need of rotating */
        switch (TREE_BALANCE(tree))
        {
        case -2:
                /* Yep rotations are necessary */
                if (TREE_BALANCE(tree->right) == 1)
                {
                        /* If right heavy, rotate right */
                        tree_rotate_right(tree->right);
                }
                /* And rotate left to balance things out */
                tree_rotate_left(tree);
                break;
        case 2:
                /* Yep rotations are necessary */
                if (TREE_BALANCE(tree->left) == -1)
                {
                        /* If left heavy, rotate left */
                        tree_rotate_left(tree->left);
                }
                /* Now rotate right */
                tree_rotate_right(tree);
                break;
        case -1:
        case 0:
        case 1:
        default:
                /* Nope, we're nicely balanced */
                break;
        }
        /* And return! */
        return -E_SUCCESS;
}

/**
 * \fn tree_find_leftmost
 * \brief Find the left most node from tree
 */
static struct tree* tree_find_leftmost(struct tree* tree)
{
        if (tree == NULL)
                return NULL;

        /* Recurse through the tree to find the node */
        struct tree* tmp = tree_find_leftmost(tree->left);
        /* If the recursion didn't work, return this node */
        if (tmp == NULL)
                return tree;

        /* Return the result of the recursion */
        return tmp;
}

/**
 * \fn tree_find_rightmost
 * \brief Find the rightmost node from tree
 */
static struct tree* tree_find_rightmost(struct tree* tree)
{
        if (tree == NULL)
                return NULL;

        /* Recurse through the tree to find the node */
        struct tree* tmp = tree_find_rightmost(tree->right);
        /* If the recursion didn't work, return this node */
        if (tmp == NULL)
                return tree;

        /* Return the result of the recursion */
        return tmp;
}

/**
 * \fn tree_find_next
 * \brief Find the successor to tree
 */
static struct tree* tree_find_next(struct tree* tree)
{
        if (tree == NULL)
                return NULL;

        if (tree->right == NULL)
        {
                struct tree* runner = tree->parent;
                struct tree* tmp = tree;
                while (runner != NULL && runner->left != tmp)
                {
                        tmp = runner;
                        runner = runner->parent;
                }
                return runner;
        }

        return tree_find_leftmost(tree->right);
}

/**
 * \fn tree_find_prev
 * \brief Find the predecessor to tree
 */
static struct tree* tree_find_prev(struct tree* tree)
{
        if (tree == NULL)
                return NULL;

        if (tree->left == NULL)
        {
                struct tree* runner = tree->parent;
                struct tree* tmp = tree;

                while (runner != NULL && runner->right != tmp)
                {
                        tmp = runner;
                        runner = runner->parent;
                }
                return runner;
        }

        return tree_find_rightmost(tree->left);
}

/**
 * \fn tree_update_list
 * \brief Update the next and previous pointers within the tree
 */
static int tree_update_list(struct tree* tree)
{
        if (tree == NULL)
                return -E_NULL_PTR;

        tree->prev = tree_find_prev(tree);
        tree->next = tree_find_next(tree);
        return -E_SUCCESS;
}

/**
 * \fn tree_add_node
 * \brief Add node t to tree at parent
 * \param parent
 * \param t
 */
static int tree_add_node(struct tree* parent, struct tree* t)
{
        if (parent == NULL || t == NULL)
                return -E_NULL_PTR;

        /* Does the node go on the left */
        if (t->key < parent->key)
        {
                /* Try to delegate downwards */
                int s = tree_add_node(parent->left, t);
                switch (s) {
                case -E_NULL_PTR:
                        /* Couldn't delegate, need to insert here it seems*/
                        parent->left = t;
                        t->parent = parent;
                        t->root->nodes++;

                        /* Update list like pointers */
                        tree_update_list(t);
                        tree_update_list(t->prev);
                        tree_update_list(t->next);
                case -E_SUCCESS:
                        /* Yep, we have a success */
                        break;
                default:
                        /* We don't know what happened, move the code upward */
                        return s;
                }
        }
        /* Or does the node go on the right */
        else if(t->key > parent->key)
        {
                /* Try to delegate downwards */
                int s = tree_add_node(parent->right, t);
                switch (s)
                {
                case -E_NULL_PTR:
                        /* Couldn't delegate, so insert here */
                        parent->right = t;
                        t->parent = parent;
                        t->root->nodes++;

                        /* Update list like pointers */
                        tree_update_list(t);
                        tree_update_list(t->prev);
                        tree_update_list(t->next);
                case -E_SUCCESS:
                        /* Seems like we have a succcess on our hands */
                        break;
                default:
                        /* We don't know what happened, move the code upward */
                        return s;
                }
        }
        else
                /* Key already exists, can't have a conflict */
                return -E_CONFLICT;

        /* Recalculate the depth */
        tree_depth(parent);
        /* Balance if necessary */
        tree_balance(parent);

        /* And we're done for now */
        return -E_SUCCESS;
}

/**
 * \fn tree_find_node
 * \brief Find the node with the desired key
 */
static struct tree* tree_find_node(int key, struct tree* tree)
{
        if (tree == NULL)
                return NULL;

        /* If the keys match, return this entry */
        if (tree->key == key)
                return tree;


        /* If key is smaller, try to find it on the left */
        if (key < tree->key)
                return tree_find_node(key, tree->left);
        else
                /* else try to find it on the right */
                return tree_find_node(key, tree->right);
}

/**
 * \fn tree_delete_node
 * \brief Delete one single node with key: key int the subtree tree
 */
static int tree_delete_node(int key, struct tree* tree)
{
        if (tree == NULL)
                return -E_NULL_PTR;

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
        if (t == NULL)
                return -E_NOTFOUND;

        if (t->ldepth == 0 && t->rdepth == 0)
        {
                /* If neither of the subtrees are present */
                if (t->key < t->parent->key)
                        t->parent->left = NULL;
                else
                        t->parent->right = NULL;
        }
        else if (t->right == NULL && t->left != NULL)
        {
                /* if only left subtree is present */
                if (t->key < t->parent->key)
                        t->parent->left = t->left;
                else
                        t->parent->right = t->left;
                t->left->parent = t->parent;
        }
        else if (t->right != NULL && t->left == NULL)
        {
                /* If only right subtree is present */
                if (t->key < t->parent->key)
                        t->parent->left = t->right;
                else
                        t->parent->right = t->right;
                t->right->parent = t->parent;
        }
        else
        {
                /* If both subtrees are present */
                struct tree* successor = tree_find_leftmost(t->right);
                if (successor == NULL)
                        return -E_NULL_PTR;

                struct tree* walker = successor->parent;

                /* Detach successor */
                successor->parent->left = successor->right;
                if (successor->right != NULL)
                        successor->right->parent = successor->parent;
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
                successor->left->parent = successor;

                /* Update meta data and balance */

                tree_depth(successor);
                tree_depth(successor->parent);
                if (walker != t)
                {
                        while(walker != NULL && walker != successor)
                        {
                                tree_depth(walker);
                                tree_balance(walker);
                                walker = walker->parent;
                        }
                }
        }
        tree_update_list(t->prev);
        tree_update_list(t->next);

        /* Free the deleted node */
        tree_depth(t->parent);
        tree_balance(t->parent);
        t->root->nodes--;

        /* Free the detached node */
        memset(t, 0, sizeof(*t));
        kfree(t);

        /* Return */
        return -E_SUCCESS;
}

/**
 * \fn tree_add
 * \brief Add a node into a tree
 */
static int tree_add(struct tree_root* root, struct tree* tree)
{
        if (root == NULL || tree == NULL)
                return -E_NULL_PTR;

        int ret = 0;
        mutex_lock(&root->mutex);
        /* Add the node into the tree if there already is one */
        if (root->tree != NULL)
        {
                ret = tree_add_node(root->tree, tree);
                goto success;
        }

        /* There is no subtree, so create the first one */
        root->tree = tree;
        ret = -E_SUCCESS;
success:
        mutex_unlock(&root->mutex);
        return ret;
}

/**
 * \fn tree_new_node
 * \brief Create a new node and insert it into the tree
 */
static struct tree* tree_new_node(int key, void* data, struct tree_root* root)
{
        /* Create new tree */
        struct tree* t = kmalloc(sizeof(*t));
        if (t == NULL)
                return NULL;
        memset(t, 0, sizeof(*t));

        /* Set up the data */
        t->key = key;
        t->data = data;
        t->root = root;

        /* If does not exist, just return the tree */
        if (root == NULL)
                return t;

        /* Try to add the node into the tree, or if all else fails, return t */
        return (tree_add(root, t) == -E_SUCCESS) ? root->tree : t;
}

/**
 * \fn tree_flush_node
 * \brief Flush everything below this node and the node itself
 */
static int
tree_flush_node(struct tree* tree, int (*dtor)(void*, void*), void* dtor_arg)
{
        if (tree == NULL)
                return -E_NULL_PTR;

        tree_flush_node(tree->left, dtor, dtor_arg);
        tree_flush_node(tree->right, dtor, dtor_arg);

        if (dtor != NULL)
        {
                dtor(tree->data, dtor_arg);
        }

        printf("Flushing %X\n", tree->key);
        memset(tree, 0, sizeof(*tree));

        kfree(tree);
        return -E_SUCCESS;
}

/**
 * \fn tree_flush
 * \brief Delete the tree and its content
 */
int tree_flush(struct tree_root* root, int (dtor)(void*,void*), void* dtor_arg)
{
        if (root == NULL)
                return -E_NULL_PTR;

        mutex_lock(&root->mutex);
        tree_flush_node(root->tree, dtor, dtor_arg);
        mutex_unlock(&root->mutex);

        memset(root, 0, sizeof(*root));
        kfree(root);
        return -E_SUCCESS;
}

/**
 * \fn tree_find
 * \brief Find a node in the tree
 */
static struct tree* tree_find(int key, struct tree_root* t)
{
        if (t == NULL)
                return NULL;

        mutex_lock(&t->mutex);
        struct tree* ret = tree_find_node(key, t->tree);
        mutex_unlock(&t->mutex);
        return ret;
}

/**
 * \fn tree_delete
 * \brief Delete a node from the tree
 */
static int tree_delete(int key, struct tree_root* root)
{
        if (root == NULL)
                return -E_NULL_PTR;

        mutex_lock(&root->mutex);
        int ret = tree_delete_node(key, root->tree);
        mutex_unlock(&root->mutex);

        return ret;
}

/**
 * \fn tree_new_avl
 * \brief Set up a new avl tree
 */
struct tree_root* tree_new_avl()
{
        /* Create the new tree */
        struct tree_root* t = kmalloc(sizeof(*t));
        if (t != NULL)
                memset(t, 0, sizeof(*t));

        /* Set up the function pointers */
        t->add = tree_new_node;
        t->find = tree_find;
        t->delete = tree_delete;
        t->flush = tree_flush;

        t->mutex = mutex_unlocked;

        /* And we're done! */
        return t;
}

/**
 * @} \file
 */
