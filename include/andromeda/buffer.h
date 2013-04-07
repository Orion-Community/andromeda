/*
 *  Andromeda
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

#ifndef __ANDROMEDA_BUFFER_H
#define __ANDROMEDA_BUFFER_H

#include <fs/vfs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \def BUFFER_OFFSET_BITS
 * \brief No. bits in BUFFER_LIST_SIZE
 * \def BUFFER_LIST_SIZE
 * \brief Size of buffer branch list. HAS TO BE POWER OF 2!!!!
 * \def BUFFER_BLOCK_SIZE
 * \brief Size of buffer block. HAS TO BE POWER OF 2!!!!
 */
#define BUFFER_OFFSET_BITS 0x8
#define BUFFER_LIST_SIZE 0x100
#define BUFFER_BLOCK_SIZE 0x1000

#define BUFFER_DYNAMIC_SIZE (0x0)

#define BUFFER_ALLOW_DUPLICATE  (1<<0)
#define BUFFER_ALLOW_GROWTH     (1<<1)

#define BUFFER_TREE_DEPTH       6

typedef enum {lineair_access, random_access} mode_t;

struct buffer_block
{
        mutex_t lock;
        char data[BUFFER_BLOCK_SIZE];
};

/** \struct buffer_list
 *  \brief This one is a list which can have lists or blocks
 * The depth of the list determines the type of list we are.
 * If we're the deepest one, we're holding blocks, else we're holding lists.
 */
struct buffer_list
{
        /**
         * \var used
         * \brief How many entries are currently in use
         * \var lock
         * \var parent
         * \brief A link to the parent buffer
         */
        atomic_t used;
        mutex_t lock;
        struct buffer* parent;
        /** \union
         *  \var lists Lists
         *  \brief the sub-lists
         *  \var blocks
         *  \brief Lists the blocks we're using
         */
        union
        {
                struct buffer_block* blocks[BUFFER_LIST_SIZE]; /** The leaves */
                struct buffer_list*  lists[BUFFER_LIST_SIZE]; /** The branches*/
        };
};

/**
 * \struct buffer
 * \brief The buffer descriptor
 */
struct buffer
{
        mutex_t lock; /** \var lock */
        size_t  size; /** \var size */
        size_t  base_idx; /** \var base_idx */
        idx_t   cleaned; /** \var cleaned */
        uint32_t rights; /** \var rights */

        /**
         * \var blocks
         * \brief A tree of blocks consisting out of 4 layers and the blocks
         *
         * This means a list of lists of lists of blocks ...
         */
        struct buffer_list *blocks;

        /** \var opened
          * \brief Counts the duplications
          */
        atomic_t opened;

        /** \fn struct buffer* dulpicate
         *  \param this */
        struct vfile* (*duplicate)(struct buffer* this);
};

/**
 * \fn buffer_init
 * \brief Initialise a new buffer
 * takes 2 arguments:
 *
 * \param size, sets the size of the buffer.
 *      If size == BUFFER_DYNAMIC_SIZE, the size will be set to 0 and the buffer
 *      now is allowed to grow dynamically.
 *
 * \param base_idx, tells us up to which point we're allowed to clean up.
 *      From 0 untill base_idx, nothing will be written.
 *      It will also set the standard cursor.
 *
 * This function returns the newly created buffer.
 */
int buffer_init(struct vfile* this, idx_t size, idx_t base_idx);

#ifdef __cplusplus
}
#endif

#endif
