/*
 *  Andromeda - AVL tree
 *  Copyright (C) 2013  Bart Kuivenhoven
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

#include <thread.h>

#ifndef __TREE_H
#define __TREE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup tree
 * @{
 */

#define AVL_BALANCE(a) (a->ldepth - a->rdepth)

#define TREE_EARLY_ALLOC        (1 << 0)

struct tree_root;

struct tree {
        struct tree* parent;
        struct tree* left;
        struct tree* right;
        struct tree_root* root;

        struct tree* next;
        struct tree* prev;

        int rdepth;
        int ldepth;

        int key;
        void* data;
};

struct tree_root {
        struct tree* tree;
        unsigned int nodes;

        unsigned int flags;

        int (*add)(int key, void* data, struct tree_root* root);
        void* (*find)(int key, struct tree_root* root);
        struct tree* (*find_close)(int key, struct tree_root* root);
        void* (*find_smaller)(int key, struct tree_root* root);
        void* (*find_larger)(int key, struct tree_root* root);
        int (*delete)(int key, struct tree_root* root);
        int (*purge)(struct tree_root*, int (dtor)(void*,void*), void*);

        mutex_t mutex;
};

struct tree_root* tree_new_avl();
struct tree_root* tree_new_avl_early();

/**
 * @} \file
 */

#ifdef __cplusplus
}
#endif

#endif
