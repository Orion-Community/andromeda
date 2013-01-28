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

#ifndef __TREE_H
#define __TREE_H

#define NULL_PTR        -1
#define TREE_CONFLICT   -2
#define TREE_NOTFOUND   -3

#define TREE_BALANCE(a) (a->ldepth - a->rdepth)

#define FLUSH_DEALLOC   1
#define FLUSH_NODEALLOC 0

#define E_INVALID_ARG   4

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

        struct tree* (*add)(int key, void* data, struct tree_root* root);
        struct tree* (*find)(int key, struct tree_root* root);
        int (*delete)(int key, struct tree_root* root);
        int (*flush)(struct tree_root* root, int flags);
};

struct tree_root* tree_new();

#endif
