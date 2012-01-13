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

#define BUFFER_LIST_SIZE 0xFF
#define BUFFER_BLOCK_SIZE 0x1000

#define BUFFER_DYNAMIC_SIZE (~0x0)

#define BUFFER_ALLOW_DUPLICATE  (1<<0)
#define BUFFER_ALLOW_GROWTH     (1<<1)

typedef enum {lists, blocks} buffer_list_t;
typedef enum {lineair_access, random_access} mode_t;

struct buffer_block
{
        mutex_t lock;
        char data[BUFFER_BLOCK_SIZE];
};

/** \struct buffer_list
 *  \brief This one is a list which can have lists or blocks
 */
struct buffer_list
{
        buffer_list_t type; /** \var type
                                \brief What type of list do we have */
        atomic_t used; /** \var used
                           \brief How many entries are currently in use */
        mutex_t lock; /** \var lock */
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

        uint32_t rights; /** \var rights */

        struct buffer_block* direct[BUFFER_LIST_SIZE]; /** \var direct */
        /** \brief The block pointed to directly */
        struct buffer_list* single_indirect; /** \var indirect*/
        struct buffer_list* double_indirect; /** \var double_indirect */
        struct buffer_list* triple_indirect; /** \var triple_indirect */
        /** \brief The blocks that reside in a tree hierarchy */

        /** \var opened
          * \brief Counts the duplications
          * \var cursor
          * \brief Where we're reading in the buffer
          */
        atomic_t opened;
        idx_t cursor;

        /** \fn struct buffer* dulpicate
         *  \param this */
        struct buffer* (*duplicate)(struct buffer* this);
        /** \fn int read
         *  \param this \param buf \param num */
        int (*read)(struct buffer* this, char* buf, size_t num);
        /** \fn int read_rand
         *  \param this \param buf \param num \param idx */
        int (*read_rand)(struct buffer* this, char* buf, size_t num, idx_t idx);
        /** \fn int write
         *  \param this \param buf \param num */
        int (*write)(struct buffer* this, char* buf, size_t num);
        /** \fn write_rand
         *  \param this \param buf \param num \param idx */
        int (*write_rand)(struct buffer* this, char* buf, size_t num,
                                                                     idx_t idx);
        /** \fn seek
         *  \param this \param offset \param from */
        int (*seek)(struct buffer* this, long offset, seek_t from);
        /** \fn close
         *  \param this */
        int (*close)(struct buffer* this);
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
struct buffer* buffer_init(idx_t size, idx_t base_idx);

#ifdef __cplusplus
}
#endif

#endif
