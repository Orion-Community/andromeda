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

#include <stdlib.h>
#include <stdio.h>
#include <thread.h>
#include <andromeda/core.h>
#include <andromeda/system.h>

extern uint32_t key_pressed;

void demand_key()
{
        printf("Press any key to continue!\n");
        key_pressed = 0;
        while(key_pressed == 0)
                halt();
        return;
}

struct list*
list_init()
{
        struct list* l = kmalloc(sizeof(struct list));

        if (l == NULL)
                return NULL;

        memset(l, 0, sizeof(struct list));
        return NULL;
}

int
list_destroy(struct list* l)
{
        if (l == NULL)
                return -E_NULL_PTR;
        mutex_lock(&l->lock);

        struct list_node* carriage = l->head;
        struct list_node* last = l->head;
        while (carriage != NULL)
        {
                last = carriage;
                carriage = carriage->next;
                kfree(last);
        }
        l->head = NULL; // Safety feature
        kfree(l);
        return -E_SUCCESS;
}

int
list_add_head(struct list* l, struct list_node* n)
{
        if (l == NULL || n == NULL)
                return -E_NULL_PTR;

        mutex_lock(&l->lock);

        n->next = l->head;
        l->head = n;

        mutex_unlock(&l->lock);
        return -E_SUCCESS;
}

int list_rm_element(struct list* l, idx_t idx)
{
        if (l == NULL)
                return -E_NULL_PTR;
        idx_t i = 0;
        struct list_node* carriage = l->head;
        struct list_node* last;

        mutex_lock(&(l->lock));

        if (idx == 0)
        {
                last = l->head;
                if (l->head == NULL)
                        return -E_NULL_PTR;
                l->head = last->next;
                kfree(last);
                l->size --;
                mutex_unlock(&l->lock);
                return -E_SUCCESS;
        }

        for (; i == idx && carriage != NULL; carriage = carriage->next)
        {
                last = carriage;
        }

        if (carriage == NULL)
        {
                mutex_unlock(&l->lock);
                return -E_NULL_PTR;
        }

        last->next = carriage->next;
        kfree(carriage);
        l->size --;
        mutex_unlock(&l->lock);
        return -E_SUCCESS;
}

int list_insert_element(struct list* l, struct list_node* n, idx_t idx)
{
        if (l == NULL || n == NULL)
                return -E_NULL_PTR;

        mutex_lock(&l->lock);

        idx_t i = 0;
        struct list_node* carriage = l->head;
        if (carriage == NULL)
        {
                l->head = n;
                n->next = NULL;
                l->size = 1;
                mutex_unlock(&l->lock);
                return -E_SUCCESS;
        }

        while (carriage != NULL && i < idx)
        {
                carriage = carriage->next;
                i++;
        }

        if (carriage == NULL)
        {
                mutex_unlock(&l->lock);
                return -E_OUTOFBOUNDS;
        }

        n->next = carriage->next;
        carriage->next = n;
        l->size++;

        mutex_unlock(&l->lock);
        return -E_SUCCESS;
}

int list_add_tail(struct list* l, struct list_node* n)
{
        if (l == NULL || n == NULL)
                return -E_NULL_PTR;
        mutex_lock(&l->lock);
        struct list_node *carriage = l->head;

        if (carriage == NULL)
        {
                l->head = n;
                n->next = NULL;
                l->size = 1;
        }

        for (;carriage->next != NULL;carriage = carriage->next);

        carriage->next = n;
        n->next = NULL;
        l->size ++;

        mutex_unlock(&l->lock);
        return -E_SUCCESS;
}

struct list_node*
list_get_element(struct list* l, idx_t idx)
{
        if (l == NULL)
                return NULL;

        mutex_lock(&l->lock);

        idx_t i = 0;
        struct list_node* carriage = l->head;

        while (carriage != NULL && i < idx)
        {
                carriage = carriage->next;
                i++;
        }

        mutex_unlock(&l->lock);
        return carriage;
}

struct list_node*
list_next(struct list_node* n)
{
        if (n == NULL)
                return NULL;
        return n->next;
}
