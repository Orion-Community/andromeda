/*
 *   Andromeda Project - XORLIST library
 *   Copyright (C) 2011  Michel Megens - dev@michelmegens.net
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIST_H
#define __LIST_H
/**
 * \struct xor_list_head
 * \brief The linked list nodes the library works with.
 */
struct xor_list_head
{
        /**
         * \var pointer
         * \brief In the pointer variable are the previous and next pointer stored.
         * \var value
         * \brief Debug value.
         */
        void *pointer;
        unsigned int value;
} __attribute__((packed));
typedef struct xor_list_head XOR_HEAD;

/**
 * \fn xorll_get_next(XOR_HEAD *prev, XOR_HEAD *this)
 * \return The next node
 * \brief Calculates the next node.
 *
 * Uses the XOR functionality to calculate the next node from the previous and
 * the next pointer.
 */
XOR_HEAD *xorll_get_next(XOR_HEAD *prev, XOR_HEAD *this);

/**
 * \fn xorll_list_insert(XOR_HEAD *prev, XOR_HEAD *this, XOR_HEAD *new)
 *
 * \param prev Previous pointer of <i>this</i>.
 * \param this The node will be after <i>this</i>
 * \param new The list node to add to the list.
 * \brief Adds <b>new</b> to the list.
 *
 * \return The error code.
 * \warning <i>prev</i> and <i>this</i> MUST be consecutive.
 *
 * This function will insert the xornode node after the <i>this</i> node.
 */
static int xorll_list_insert(XOR_HEAD *prev, XOR_HEAD *this, XOR_HEAD *new);

/**
 * \fn xorll_remove_node(XOR_HEAD *prev, XOR_HEAD *this)
 * \param prev Previous node of <i>this</i>
 * \param this Node which has to be removed.
 * \brief Remove node <i>this</i> from the list.
 *
 * xorll_remove_node removes node <i>this</i> from the linked list.
 */
int xorll_remove_node(XOR_HEAD *prev, XOR_HEAD *this);

/**
 * \fn xorll_list_add(XOR_HEAD *list, XOR_HEAD *node, XOR_HEAD *new)
 * \brief Add the node <i>new</i> to <i>list</i>.
 * \param list The list head.
 * \param node The node to add the new node after.
 * \param new The node to add after <i>node</i>.
 * \return ERROR code.
 *
 * The xornode <i>new</i> will be added after <i>node</i> in the list
 * <i>list</i>.
 */
int xorll_list_add(XOR_HEAD *listHead, XOR_HEAD *node, XOR_HEAD *new);

static inline XOR_HEAD*
get_prev_node(XOR_HEAD *this, XOR_HEAD *next)
{
        return (XOR_HEAD*)((this) ? (ulong)this->pointer ^ (ulong)next : 0);
}

#endif