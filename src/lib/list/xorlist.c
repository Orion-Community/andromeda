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
/**
 * \AddToGroup xor_linked_list
 * @{
 */
#include <stdlib.h>
#include "error.h"

#include <lib/xorlist.h>

/**
 * \fn xorll_get_next(XOR_HEAD *prev, XOR_HEAD *this)
 * \return The next node
 * \brief Calculates the next node.
 *
 * Uses the XOR functionality to calculate the next node from the previous and
 * the next pointer.
 */
XOR_HEAD *
xorll_get_next(XOR_HEAD *prev, XOR_HEAD *this)
{
        if(this == NULL)
                return this;
        ulong uprev = (ulong)prev;
        ulong uthis = (ulong)((this) ? this->pointer : NULL);

        ulong next = uprev ^ uthis;

        return (XOR_HEAD*)next;
}

/**
 * \fn xorll_list_insert(XOR_HEAD *prev, XOR_HEAD *this, XOR_HEAD *new)
 * \brief Adds <b>node</b> to <b>list</b>.
 * \param prev The previous node
 * \param this The node will be after this node.
 * \param new The list node to add to the list.
 * \warning <i>prev</i> and <i>this</i> MUST be consecutive nodes.
 *
 * This function will insert the xornode node between alpha and beta.
 */
int
xorll_list_insert(XOR_HEAD *prev, XOR_HEAD *this, XOR_HEAD *new)
{
        ulong uprev = (ulong)prev;
        ulong uthis = (ulong)this;
        ulong unew  = (ulong)new;
        XOR_HEAD *next  = xorll_get_next(prev, this);
        ulong unext = (ulong)next;

        ulong pNext_next = (next) ? (ulong)next->pointer ^ uthis : 0 ^ uthis;

        if(NULL == next)
        {
                this->pointer = (void*)(uprev ^ unew);
                new->pointer = (XOR_HEAD*)(uthis ^ 0);
                return OK;
        }

        /* set the node pointer of this */
        this->pointer = (XOR_HEAD*)(uprev ^ unew);
        new->pointer = (XOR_HEAD*)(uthis ^ unext);
        next->pointer = (XOR_HEAD*)(pNext_next ? unew ^ pNext_next : unew ^ 0);
        return OK;
}

/**
 * \fn xorll_remove_node(XOR_HEAD *prev, XOR_HEAD *this)
 * \param prev Previous node of <i>this</i>
 * \param this Node which has to be removed.
 * \brief Remove node <i>this</i> from the list.
 *
 * xorll_remove_node removes node <i>this</i> from the linked list.
 */
int
xorll_remove_node(XOR_HEAD *prev, XOR_HEAD *this)
{
        if(NULL == this)
                return NULL_PTR;

        XOR_HEAD *next = xorll_get_next(prev, this);
        ulong uprev_prev = (ulong) ((prev) ? get_prev_node(prev, this) : NULL);

        ulong unext = (ulong)next;
        ulong uprev = (ulong)prev;
        ulong uthis = (ulong)this;
        ulong unext_next = (ulong) ((next) ? (ulong)next->pointer ^ uthis : 0);

        if(NULL != prev)
                prev->pointer = (void*)(uprev_prev ^ unext);

        if(NULL != next)
                next->pointer = (void*)(unext_next ^ uprev);
        this->pointer = NULL;

        return OK;
}

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
int
xorll_list_add(XOR_HEAD *listHead, XOR_HEAD *node, XOR_HEAD *new)
{
        XOR_HEAD *prev = NULL,*carriage = listHead, *tmp;

        if(!new)
                return NULL_PTR;
        while(carriage)
        {
                if(carriage == node)
                {
                        xorll_list_insert(prev, carriage, new);
                        break;
                }
                tmp = carriage;
                carriage = xorll_get_next(prev, tmp);
                prev = tmp;

                if(!carriage && !node && new)
                {
                        carriage = tmp;
                        prev = get_prev_node(carriage, NULL);
                        xorll_list_insert(prev, carriage, new);
                }
        }

        return OK;
}

/**
 * \fn iterate_xor_list(XOR_HEAD *prev, XOR_HEAD *head, xor_list_iterator_t hook)
 * \param prev Previous node of the starting point <i>head</i>
 * \param head Iterate starting point.
 * \param hook Will be called every iteration.
 * \return An error code.
 * \brief Iterates trough a XOR linked list.
 *
 * This function returns trough a XOR-linkedlist and it will call hook on every
 * iteration.
 */
int
iterate_xor_list(XOR_HEAD *prev, XOR_HEAD *head, xor_list_iterator_t hook)
{
        XOR_HEAD *carriage = head, *tmp;
        int result = -1;

        if(!hook)
                return NULL_PTR;
        while(carriage)
        {
                tmp = carriage; // save to set prev later
                carriage = xorll_get_next(prev, tmp); // get next one..
                if(prev)
                        if(HOOK_DONE == (result = hook(prev)))
                                break;
                prev = tmp;
        }
        result = hook(prev);

        return result;
}

/** @} \file */
