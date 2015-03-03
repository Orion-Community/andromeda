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

#ifdef SLAB
#include <mm/cache.h>
static struct mm_cache* avl_root_cache = NULL;
static struct mm_cache* avl_node_cache = NULL;

static mutex_t avl_cache_init_lock = mutex_unlocked;
#endif

/**
 * \addtogroup tree
 * @{
 * \fn avl_depth
 * \brief Recalculate the depth counters of this tree
 * \param tree
 */
static int avl_depth(struct tree* tree)
{
        if (tree == NULL)
                return -E_NULL_PTR;

        /*printf("Tree: %X\tTree root: %X\t", (int) tree, (int) tree->root);
         printf("Tree->parent: %X\n", (int) tree->parent);
         printf("Tree->left: %X\t", (int) tree->left); */
        /* Update left counter */
        if (tree->left != NULL) {
                int a = tree->left->ldepth;
                int b = tree->left->rdepth;
                tree->ldepth = ((a > b) ? a : b) + 1;
        } else
                tree->ldepth = 0;
        /*
         printf("Tree->left->depth: %X\n", tree->ldepth);
         printf("Tree->right: %X\t", (int) tree->left); */
        /* Update right counter */
        if (tree->right != NULL) {
                int a = tree->right->ldepth;
                int b = tree->right->rdepth;
                tree->rdepth = ((a > b) ? a : b) + 1;
        } else
                tree->rdepth = 0;
        /*      printf("Tree->right->depth: %X\n", tree->rdepth); */

        if (tree->root == NULL) {
                panic("Tree root == NULL");
        }
        /* And we're done again */
        return -E_SUCCESS;
}

/**
 * \fn avl_rotate_right
 * \brief Rotate right with the root at tree
 * \param tree
 */
static int avl_rotate_right(struct tree* tree)
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
        else {
                if (tree->key < parent->key)
                        parent->left = left;
                else
                        parent->right = left;
                avl_depth(parent);
        }

        /* Reconsider the depth */
        avl_depth(tree);
        avl_depth(left);

        return -E_SUCCESS;
}

/**
 * \fn avl_rotate_left
 * \brief Rotate left with tree as root
 * \param tree
 */
static int avl_rotate_left(struct tree* tree)
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
        else {
                if (tree->key < parent->key)
                        parent->left = right;
                else
                        parent->right = right;
        }

        /* Reconsider the depth */
        avl_depth(tree);
        avl_depth(right);

        return -E_SUCCESS;
}

/**
 * \fn avl_balance
 * \brief Balance tree at this node
 * \param tree
 */
static int avl_balance(struct tree* tree)
{
        if (tree == NULL)
                return -E_NULL_PTR;

        /* Determine the need of rotating */
        switch (AVL_BALANCE(tree)) {
        case -2:
                /* Yep rotations are necessary */
                if (AVL_BALANCE(tree->right) == 1) {
                        /* If right heavy, rotate right */
                        avl_rotate_right(tree->right);
                }
                /* And rotate left to balance things out */
                avl_rotate_left(tree);
                break;
        case 2:
                /* Yep rotations are necessary */
                if (AVL_BALANCE(tree->left) == -1) {
                        /* If left heavy, rotate left */
                        avl_rotate_left(tree->left);
                }
                /* Now rotate right */
                avl_rotate_right(tree);
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
 * \fn avl_find_leftmost
 * \brief Find the left most node from tree
 */
static struct tree* avl_find_leftmost(struct tree* tree)
{
        if (tree == NULL)
                return NULL ;

        /* Recurse through the tree to find the node */
        struct tree* tmp = avl_find_leftmost(tree->left);
        /* If the recursion didn't work, return this node */
        if (tmp == NULL)
                return tree;

        /* Return the result of the recursion */
        return tmp;
}

/**
 * \fn avl_find_rightmost
 * \brief Find the rightmost node from tree
 */
static struct tree* avl_find_rightmost(struct tree* tree)
{
        if (tree == NULL)
                return NULL ;

        /* Recurse through the tree to find the node */
        struct tree* tmp = avl_find_rightmost(tree->right);
        /* If the recursion didn't work, return this node */
        if (tmp == NULL)
                return tree;

        /* Return the result of the recursion */
        return tmp;
}

/**
 * \fn avl_find_next
 * \brief Find the successor to tree
 */
static struct tree* avl_find_next(struct tree* tree)
{
        if (tree == NULL)
                return NULL ;

        if (tree->right == NULL) {
                struct tree* runner = tree->parent;
                struct tree* tmp = tree;
                while (runner != NULL && runner->left != tmp) {
                        tmp = runner;
                        runner = runner->parent;
                }
                return runner;
        }

        return avl_find_leftmost(tree->right);
}

/**
 * \fn avl_find_prev
 * \brief Find the predecessor to tree
 */
static struct tree* avl_find_prev(struct tree* tree)
{
        if (tree == NULL)
                return NULL ;

        if (tree->left == NULL) {
                struct tree* runner = tree->parent;
                struct tree* tmp = tree;

                while (runner != NULL && runner->right != tmp) {
                        tmp = runner;
                        runner = runner->parent;
                }
                return runner;
        }

        return avl_find_rightmost(tree->left);
}

/**
 * \fn avl_update_list
 * \brief Update the next and previous pointers within the tree
 */
static int avl_update_list(struct tree* tree)
{
        if (tree == NULL)
                return -E_NULL_PTR;

        tree->prev = avl_find_prev(tree);
        tree->next = avl_find_next(tree);
        return -E_SUCCESS;
}

/**
 * \fn avl_add_node
 * \brief Add node t to tree at parent
 * \param parent
 * \param t
 */
static int avl_add_node(struct tree* parent, struct tree* t)
{
        if (parent == NULL || t == NULL)
                return -E_NULL_PTR;

        /* Does the node go on the left */
        if (t->key < parent->key) {
                /* Try to delegate downwards */
                int s = avl_add_node(parent->left, t);
                switch (s) {
                case -E_NULL_PTR:
                        /* Couldn't delegate, need to insert here it seems*/
                        parent->left = t;
                        t->parent = parent;
                        t->root->nodes++;

                        /* Update list like pointers */
                        avl_update_list(t);
                        avl_update_list(t->prev);
                        avl_update_list(t->next);
                case -E_SUCCESS:
                        /* Yep, we have a success */
                        break;
                default:
                        /* We don't know what happened, move the code upward */
                        return s;
                }
        }
        /* Or does the node go on the right */
        else if (t->key > parent->key) {
                /* Try to delegate downwards */
                int s = avl_add_node(parent->right, t);
                switch (s) {
                case -E_NULL_PTR:
                        /* Couldn't delegate, so insert here */
                        parent->right = t;
                        t->parent = parent;
                        t->root->nodes++;

                        /* Update list like pointers */
                        avl_update_list(t);
                        avl_update_list(t->prev);
                        avl_update_list(t->next);
                case -E_SUCCESS:
                        /* Seems like we have a succcess on our hands */
                        break;
                default:
                        /* We don't know what happened, move the code upward */
                        return s;
                }
        } else {
                /* Key already exists, can't have a conflict */
                return -E_CONFLICT;
        }

        /* Recalculate the depth */
        avl_depth(parent);
        /* Balance if necessary */
        avl_balance(parent);

        /* And we're done for now */
        return -E_SUCCESS;
}

/**
 * \fn avl_find_node
 * \brief Find the node with the desired key
 */
static struct tree* avl_find_node(int key, struct tree* tree)
{
        if (tree == NULL)
                return NULL ;

        /* If the keys match, return this entry */
        if (tree->key == key)
                return tree;

        /* If key is smaller, try to find it on the left */
        if (key < tree->key)
                return avl_find_node(key, tree->left);
        else
                /* else try to find it on the right */
                return avl_find_node(key, tree->right);
}

static struct tree* avl_find_closest_node(int key, struct tree* tree)
{
        if (tree == NULL)
                return NULL ;

        if (tree->key == key)
                return tree;

        struct tree* ret = NULL;
        if (key < tree->key)
                ret = avl_find_closest_node(key, tree->left);
        else
                ret = avl_find_node(key, tree->right);

        if (ret == NULL)
                ret = tree;
        return ret;
}

/**
 * \fn avl_delete_node_simple
 * \brief Delete node if no or only one side is used
 */
static int avl_delete_node_simple(struct tree* tree)
{
        if (tree == NULL || tree->parent == NULL) {
                return -E_NULL_PTR;
        }

        if (tree->ldepth == 0 && tree->rdepth == 0) {
                /* If neither of the subtrees are present */
                if (tree->key < tree->parent->key) {
                        tree->parent->left = NULL;
                } else {
                        tree->parent->right = NULL;
                }
        } else if (tree->right == NULL && tree->left != NULL) {
                /* if only left subtree is present */
                if (tree->key < tree->parent->key) {
                        tree->parent->left = tree->left;
                } else {
                        tree->parent->right = tree->left;
                }
                tree->left->parent = tree->parent;
        } else if (tree->right != NULL && tree->left == NULL) {
                /* If only right subtree is present */
                if (tree->key < tree->parent->key) {
                        tree->parent->left = tree->right;
                } else {
                        tree->parent->right = tree->right;
                }
                tree->right->parent = tree->parent;
        }
        return -E_SUCCESS;
}

/**
 * \fn avl_delete_node_orphan
 * \brief Delete node without parent and with one or no child node
 */
static int avl_delete_node_orphan(struct tree* tree)
{
        if (tree == NULL) {
                return -E_NULL_PTR;
        }
        if (tree->parent != NULL) {
                return -E_CORRUPT;
        }

        if (tree->ldepth == 0 && tree->rdepth == 0 ) {
                tree->root->tree = NULL;
        }
        else if (tree->ldepth != 0 && tree->rdepth == 0) {
                tree->root->tree = tree->left;
                tree->left->parent = NULL;
        }
        else if (tree->rdepth != 0 && tree->ldepth == 0) {
                tree->root->tree = tree->right;
                tree->right->parent = NULL;
        } else {
                return -E_CORRUPT;
        }
        return -E_SUCCESS;
}

/**
 * \fn avl_delete_node
 * \brief Delete one single node with key:
 * \param key int
 * \param tree tree
 */
static int avl_delete_node(int key, struct tree* tree)
{
        if (tree == NULL) {
                return -E_NULL_PTR;
        }

        struct tree* t = tree;
        if (tree->key != key) {
                if (key < tree->key) {
                        key = avl_delete_node(key, tree->left);
                        avl_depth(tree);
                        return key;
                } else {
                        key = avl_delete_node(key, tree->right);
                        avl_depth(tree);
                        return key;
                }
        }
        if (t == NULL) {
                return -E_NOTFOUND;
        }
        int ret = -E_SUCCESS;
        if ((t->left == NULL || t->right == NULL ) && t->parent != NULL) {
                ret = avl_delete_node_simple(t);
        } else if ((t->left == NULL || t->right == NULL ) && t->parent == NULL) {
                ret = avl_delete_node_orphan(tree);
        } else {
                /* If both subtrees are present */
                struct tree* successor = avl_find_leftmost(tree->right);
                if (successor == NULL) {
                        return -E_NULL_PTR;
                }

                struct tree* walker = successor->parent;

                /* Detach successor */
                successor->parent->left = successor->right;
                if (successor->right != NULL) {
                        successor->right->parent = successor->parent;
                }
                avl_depth(successor->parent);

                /* Set up successor */
                successor->parent = tree->parent;
                if (tree->right == successor) {
                        successor->right = NULL;
                } else {
                        successor->right = tree->right;
                }

                successor->left = tree->left;

                if (successor->parent == NULL) {
                        successor->root->tree = successor;
                } else if (successor->key < successor->parent->key) {
                        successor->parent->left = successor;
                } else {
                        successor->parent->right = successor;
                }

                if (successor->right != NULL) {
                        successor->right->parent = successor;
                }
                successor->left->parent = successor;

                /* Update meta data and balance */

                avl_depth(successor);
                avl_depth(successor->parent);
                if (walker != tree) {
                        while (walker != NULL && walker != successor) {
                                avl_depth(walker);
                                avl_balance(walker);
                                walker = walker->parent;
                        }
                }
        }
        avl_update_list(tree->prev);
        avl_update_list(tree->next);

        /* Free the deleted node */
        avl_depth(tree->parent);
        avl_balance(tree->parent);
        tree->root->nodes--;

        /* Free the detached node */
        memset(tree, 0, sizeof(*tree));
        kfree(tree);

        /* Return */
        return ret;
}
/**
 * \fn avl_add
 * \brief Add a node into a tree
 */
static int avl_add(struct tree_root* root, struct tree* tree)
{
        if (root == NULL || tree == NULL)
                return -E_NULL_PTR;

        int ret = 0;
//tree->root = root;
        mutex_lock(&root->mutex);
        /* Add the node into the tree if there already is one */
        if (root->tree != NULL) {
                ret = avl_add_node(root->tree, tree);
                goto success;
        }

        /* There is no subtree, so create the first one */
        root->tree = tree;
        ret = -E_SUCCESS;
        success: mutex_unlock(&root->mutex);
        return ret;
}

/**
 * \fn avl_new_node
 * \brief Create a new node and insert it into the tree
 */
static int avl_new_node(int key, void* data, struct tree_root* root)
{
        /* If does not exist, just return the tree */
        if (root == NULL)
                return -E_NOT_YET_INITIALISED;

        /* Create new tree */
        struct tree* t;
#ifdef SLAB
        if (root->flags & TREE_EARLY_ALLOC)
                t = kmalloc(sizeof(*t));
        else
                t = mm_cache_alloc(avl_node_cache, 0);
#else
        t = kmalloc(sizeof(*t));
#endif

        if (t == NULL)
                return -E_NOMEM;
        memset(t, 0, sizeof(*t));

        /* Set up the data */
        t->key = key;
        t->data = data;
        t->root = root;

        /* Try to add the node into the tree, or if all else fails, return t */
        if (avl_add(root, t) != -E_SUCCESS) {
#ifdef SLAB
                if (root->flags & TREE_EARLY_ALLOC)
                        kfree(t);
                else
                        mm_cache_free(avl_node_cache, t);
#else
                kfree(t);
#endif
                return -E_GENERIC;
        }
        return -E_SUCCESS;
}

/**
 * \fn avl_flush_node
 * \brief Flush everything below this node and the node itself
 */
static int avl_flush_node(struct tree* tree, int (*dtor)(void*, void*),
                void* dtor_arg)
{
        if (tree == NULL)
                return -E_NULL_PTR;

        int ret = -E_SUCCESS;

        if ((ret = avl_flush_node(tree->left, dtor, dtor_arg)) != -E_SUCCESS)
                return ret;
        if ((ret = avl_flush_node(tree->right, dtor, dtor_arg)) != -E_SUCCESS)
                return ret;

        if (dtor != NULL) {
                if ((ret = dtor(tree->data, dtor_arg)) != -E_SUCCESS)
                        return ret;
        }

        memset(tree, 0, sizeof(*tree));

#ifdef SLAB
        if (tree->root->flags & TREE_EARLY_ALLOC)
                kfree(tree);
        else
                mm_cache_free(avl_node_cache, tree);
#else
        kfree(tree);
#endif
        return -E_SUCCESS;
}

/**
 * \fn avl_flush
 * \brief Delete the tree and its content
 */
int avl_flush(struct tree_root* root, int (dtor)(void*, void*), void* dtor_arg)
{
        if (root == NULL)
                return -E_NULL_PTR;

        mutex_lock(&root->mutex);
        avl_flush_node(root->tree, dtor, dtor_arg);
        mutex_unlock(&root->mutex);

        memset(root, 0, sizeof(*root));
#ifdef SLAB
        if (root->flags & TREE_EARLY_ALLOC)
                kfree(root);
        else
                mm_cache_free(avl_root_cache, root);
#else
        kfree(root);
#endif
        return -E_SUCCESS;
}

static void* avl_find_smaller(int key, struct tree_root* t)
{
        if (t == NULL) {
                return NULL;
        }

        mutex_lock(&t->mutex);
        struct tree* ret = avl_find_closest_node(key, t->tree);
        if (ret != NULL && ret->key < key) {
                ret = ret->prev;
        }
        mutex_unlock(&t->mutex);

        return (ret == NULL) ? NULL : ret->data;
}

static void* avl_find_larger(int key, struct tree_root* t)
{
        if (t == NULL) {
                return NULL;
        }

        mutex_lock(&t->mutex);
        struct tree* ret = avl_find_closest_node(key, t->tree);

        if (ret != NULL && ret->key > key) {
                ret = ret->next;
        }
        mutex_unlock(&t->mutex);

        return (ret == NULL) ? NULL : ret->data;
}

/**
 * \fn avl_find
 * \brief Find a node in the tree
 */
static void* avl_find(int key, struct tree_root* t)
{
        if (t == NULL) {
                return NULL;
        }

        mutex_lock(&t->mutex);
        struct tree* ret = avl_find_node(key, t->tree);
        mutex_unlock(&t->mutex);
        return (ret == NULL ) ? NULL : ret->data;
}

/**
 * \fn avl_find_close
 * \param key
 * \param t
 * \return The structure that is closest to the key to be found
 */
static struct tree* avl_find_close(int key, struct tree_root* t)
{
        if (t == NULL)
                return NULL ;

        mutex_lock(&t->mutex);
        struct tree* ret = avl_find_closest_node(key, t->tree);
        mutex_unlock(&t->mutex);

        return ret;
}

/**
 * \fn avl_delete
 * \brief Delete a node from the tree
 */
static int avl_delete(int key, struct tree_root* root)
{
        if (root == NULL)
                return -E_NULL_PTR;

        mutex_lock(&root->mutex);
        int ret = avl_delete_node(key, root->tree);
        mutex_unlock(&root->mutex);

        return ret;
}

static void tree_avl_init(struct tree_root* t)
{
        memset(t, 0, sizeof(*t));

        /* Set up the function pointers */
        t->add = avl_new_node;
        t->find = avl_find;
        t->find_close = avl_find_close;
        t->find_smaller = avl_find_smaller;
        t->find_larger = avl_find_larger;
        t->delete = avl_delete;
        t->purge = avl_flush;

        t->mutex = mutex_unlocked;
}

/**
 * \fn avl_new_avl
 * \brief Set up a new avl tree
 */
struct tree_root* tree_new_avl()
{
        /* Create the new tree */
        struct tree_root* t;
#ifdef SLAB
        if (avl_root_cache == NULL) {
                mutex_lock(&avl_cache_init_lock);

                if (avl_root_cache == NULL)
                        avl_root_cache = mm_cache_init("avl roots", sizeof(*t),
                                        0, NULL,
                                        NULL);
                if (avl_node_cache == NULL)
                        avl_node_cache = mm_cache_init("avl nodes",
                                        sizeof(struct tree), 0, NULL, NULL);

                mutex_unlock(&avl_cache_init_lock);
        }
        t = mm_cache_alloc(avl_root_cache, 0);
#else
        t = kmalloc(sizeof(*t));
#endif
        if (t == NULL)
                return NULL ;

        tree_avl_init(t);

        return t;
}

struct tree_root* tree_new_avl_early()
{
        struct tree_root* t = kmalloc(sizeof(*t));
        if (t == NULL)
                return NULL ;

        tree_avl_init(t);
        t->flags |= TREE_EARLY_ALLOC;

        return t;
}

/**
 * @} \file
 */
