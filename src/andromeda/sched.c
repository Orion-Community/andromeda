/*
    Andromeda
    Copyright (C) 2011  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <andromeda/sched.h>

volatile boolean scheduling = FALSE;

unsigned char stack[STD_STACK_SIZE];

struct __TASK_BRANCH_NODE *task_stack = NULL;
uint32_t current = 0; /** Index into the task stack (or tree) */

mutex_t sched_lock = mutex_unlocked;

struct __TASK_STATE*
current_task()
{
	return (find_task(current));
}

int
init_node(parent, parent_idx, type)
struct __TASK_BRANCH_NODE *parent;
uint16_t parent_idx;
enum task_list_type type;
{
	if (parent == NULL)
		return -E_NULL_PTR;

	if (parent->full &&
							  (1 << parent_idx-1) ||
					     parent->branch[parent_idx] != NULL)
	{
	mutex_unlock(sched_lock);
	return -E_ALREADY_INITIALISED;
	}

	parent->branch[parent_idx] = kalloc(sizeof(struct __TASK_BRANCH_NODE));
	memset(parent->branch[parent_idx], 0, sizeof(struct __TASK_BRANCH_NODE));
	parent->branch[parent_idx]->type = type;

	return -E_SUCCESS;
}

int find_free_pid()
{
	struct __TASK_BRANCH_NODE *itterator = NULL;
	uint32_t idx = 0;
	for (; idx < 0xFFFF; idx ++)
	{
	if (task_stack->full && (1 << (idx >> 8)))
		continue;
	itterator = task_stack->branch[idx >> 8];
	if (itterator == NULL)
		return idx;

	if (itterator->task[(idx & 0xFF)] != NULL)
		continue;
	else
		return idx;
	}
	return -1;
}

int
add_task(task)
struct __TASK_STATE* task;
{
	if (task_stack == NULL)
		return -E_NULL_PTR;

	mutex_lock(sched_lock);
	int pid = find_free_pid();
	if (pid == -1)
	{
		mutex_unlock(sched_lock);
		return -E_TASK_NOSPACE;
	}

	uint32_t branch_idx =(pid >> 8) & 0xFF;
	uint32_t task_idx = pid & 0xFF;

	if (task_stack->branch[branch_idx] == NULL)
	{
		task_stack->branch[branch_idx] =
				      kalloc(sizeof(struct __TASK_BRANCH_NODE));
		if (task_stack->branch[branch_idx] == NULL)
		{
			mutex_unlock(sched_lock);
			return -1;
		}
		memset(task_stack->branch[branch_idx], 0,
					     sizeof(struct __TASK_BRANCH_NODE));
		task_stack->branch[branch_idx]->type = task_list;
	}

	if (task_stack->branch[branch_idx]->type != task_list)
	{
		mutex_unlock(sched_lock);
		return -1;
	}

	task_stack->branch[branch_idx]->task[task_idx] = task;
	task_stack->branch[branch_idx]->full |= (1 << task_idx);

	if (task_stack->branch[branch_idx]->full == 0xFF)
		task_stack->full |= (1 << branch_idx);

	mutex_unlock(sched_lock);
	return pid;
}

/**
 * task_setup_tree actually only initialises the first node of the tree, the
 * rest is up to other, more detailed functions
 */
int task_setup_tree()
{
	if (task_stack != NULL)
		return -E_ALREADY_INITIALISED;

	task_stack = kalloc(sizeof(struct __TASK_BRANCH_NODE));

	if (task_stack == NULL)
		return -E_NULL_PTR;

	memset(task_stack, 0, sizeof(struct __TASK_BRANCH_NODE));

	return -E_SUCCESS;
}

/**
 * Set up a new scheduling quantum
 */
void get_new_quantum()
{
	panic("No tasks to build quantum");
}

/**
 * Stop and save the last task and load the next one from the quantum, if
 * scheduling conditions are met.
 */
void sched(current_state)
isrVal_t *current_state;
{
	panic("Scheduling not supported!");
}

/**
 * Remove all threads from task
 */
void
thread_cleanup_all(task)
struct __TASK_STATE* task;
{
	struct __THREAD_LIST *itterator = task->threads;
	int i;
	for (; itterator != NULL; itterator = itterator->next)
	{
		for (i=0; i < SCHED_LIST_SIZE; i++)
		{
			if (itterator->thread[i] == NULL)
				continue;
			free (itterator->thread[i]);
		}
	}
	struct __THREAD_LIST *tmp;
	for (itterator = task->threads; itterator != NULL; itterator = tmp)
	{
		tmp = itterator->next;
		free (itterator);
	}
}

/**
 * Copy all threads from task 1 to task 2
 */
static int
thread_copy(src, dest)
struct __TASK_STATE* src;
struct __TASK_STATE* dest;
{
	struct __THREAD_LIST *carriage = src->threads;
	if (carriage == NULL)
		return -E_NULL_PTR;

	struct __THREAD_LIST *carriage2 = kalloc(sizeof(struct __THREAD_LIST));
	if (carriage2 == NULL)
		return -E_NULL_PTR;
	dest->threads = carriage2;

	int idx = 0;
	for (; carriage != NULL;
						      carriage = carriage->next,
						    carriage2 = carriage2->next)
	{
		for (idx = 0; idx < SCHED_LIST_SIZE; idx++)
		{
			if (carriage->thread[idx] == NULL)
				continue;

		carriage2->thread[idx] = kalloc(sizeof(struct __THREAD_STATE));
		if (carriage2->thread[idx] == NULL)
			return -E_NULL_PTR;
		memcpy(carriage2->thread[idx],
                                                          carriage->thread[idx],
                                                 sizeof(struct __THREAD_STATE));
		}

		if (carriage->next != NULL)
		{
			carriage2->next = kalloc(sizeof(struct __THREAD_LIST));
			if (carriage2->next == NULL)
				return -E_NULL_PTR;
		}
  }

  return -E_SUCCESS;
}

/**
 * Basically make a copy of the current process
 */
int fork()
{
        warning("WARNING! In fork the registers copied probably aren't "
                                                               "up-to-date");

	struct __TASK_STATE *new = kalloc(sizeof(struct __TASK_STATE));
	if (new == NULL)
		goto err;

	struct __TASK_STATE* running = current_task();
	if (running == NULL)
		return;
	memcpy (new, running, sizeof (struct __TASK_STATE));
		if (thread_copy(running, new) != -E_SUCCESS)
		{
			thread_cleanup_all(new);
			free(new);
			goto err;
		}

	new->parent_id = current;

	int pid = add_task(new);
	if (pid == -1)
	{
		thread_cleanup_all(new);
		free(new);
		goto err;
	}

	if (pid == current)
		return 0;
	return pid;

	err:
	panic("No more space for tasks!"); // Will do for now
	return -1; // Keep the compiler happy at all times ...
}

struct __TASK_STATE*
find_task(uint32_t pid)
{
	if (pid > 0xFFFF)
		return NULL;

	struct __TASK_BRANCH_NODE *level2 =
					task_stack->branch[((pid >> 8) & 0xFF)];
	struct __TASK_STATE *task = level2->task[(pid & 0xFF)];

	return task;
}

/**
 * Send a kill signal to a process (it has to handle that immediately)
 */
void sig (int signal)
{
	panic("signals not supported yet!");
}

/**
 * Shoot a process (guess they're not as bulletproof as they claim they are)
 */
void kill(int pid)
{
	mutex_lock(sched_lock);

	struct __TASK_BRANCH_NODE *level2 =
					  task_stack->branch[(pid >> 8) & 0xFF];
	if (level2 == NULL)
		return;
	struct __TASK_STATE *task = level2->task[(pid & 0xFF)];
	if (task == NULL)
	return;

	level2->task[(pid & 0xFF)] = NULL;

	free(task);

	uint16_t tmp = ~(1 << (pid & 0xFF));
	level2 -> full &= tmp;

	if (level2->full != 0xFF)
	{
		tmp = ~(1 << ((pid >> 8) & 0xFF));
		task_stack->full &= tmp;
	}

	if (level2->full == 0)
		free(task_stack->branch[(pid >> 8) & 0xFF]);

	mutex_unlock(sched_lock);
}

/**
 * Start the basic multi-tasking administration
 */
int task_init()
{
	if (current != 0)
		panic("Trying to init scheduling on a running system!");

	/** Building the first task */
	struct __TASK_STATE *kern = kalloc(sizeof(struct __TASK_STATE));
	struct __THREAD_STATE *thread = kalloc(sizeof(struct __THREAD_STATE));
	struct __THREAD_LIST *list = kalloc(sizeof(struct __THREAD_LIST));
	if (thread == NULL || kern == NULL || list == NULL)
		goto err;

	/** Set them all to 0 */
	memset(kern, 0, sizeof(struct __TASK_STATE));
	memset(thread, 0, sizeof(struct __THREAD_STATE));
	memset(list, 0, sizeof(struct __THREAD_LIST));

	kern->threads = list;
	list->thread[0] = thread;

	/** Panic if something went horribly wrong */
	if (task_setup_tree() != -E_SUCCESS)
		goto err;
	if (add_task(kern) != 0)
		goto err;

	/** Set up the segments */
	kern->code = &higherhalf;
	kern->code_size = ((addr_t)&rodata - (addr_t)&higherhalf);
	kern->data = &rodata;
	kern->data_size = 0 - (addr_t)&rodata;

	/** Set up the first stack */
	thread->ss = stack;
	thread->ss_size = STD_STACK_SIZE;

	/** Where can we find more info if swapped out? */
        warning("WARNING! No path to kernel binary");
	kern->path_to_bin = NULL;

	kern->state = runnable;

	return -E_SUCCESS; /** A success error code? Sure ... */

err:
	panic("Could not initialise task administration");
	return -1; // Keep the compiler happy at all times
}

void print_task_stack()
{
	struct __TASK_BRANCH_NODE *itterator;
	int idx = 0;
	for (; idx < 0xFFFF; idx++)
	{
		if (task_stack->full && (1 << (idx >> 8)))
		{
			idx += (0xFF - idx % 0xFF);
			continue;
		}
		itterator = task_stack->branch[(idx >> 8)];

		if (itterator == NULL)
		{
			idx += (0xFF - idx % 0xFF);
			continue;
		}

		if (itterator->task[idx & 0xFF] != NULL)
		{
			printf("Task %X at address %X\n",
									    idx,
					  (addr_t) itterator->task[idx & 0xFF]);
			printf("Task parent: %X\n",
					itterator->task[idx & 0xFF]->parent_id);
		}
	}
}
