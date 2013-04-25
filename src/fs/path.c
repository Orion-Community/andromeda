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
#include <fs/path.h>
#include <andromeda/system.h>

void clean_path(struct __PATH_ELEMENT* elements)
{
	struct __PATH_ELEMENT *carriage = elements;
	for (; carriage != NULL; carriage = carriage->next)
	{
		kfree(carriage);
	}
}

void add_character(struct __PATH_ELEMENT* element, char c)
{
	if (element->cursor == 0xff)
		return;
	element->name[element->cursor] = c;
	element->cursor++;
}

struct __PATH_ELEMENT *parse_path(char* path)
{
	if (path == NULL)
		return NULL;
	if (strlen(path) == 0)
		return NULL;

	struct __PATH_ELEMENT *list = kmalloc(sizeof(struct __PATH_ELEMENT));
	memset(list, 0, sizeof(struct __PATH_ELEMENT));

	struct __PATH_ELEMENT *carriage = list;
	int idx = 0;
	boolean escaped = FALSE;

	for (; path[idx] != '\0'; idx++)
	{
		switch(path[idx])
		{
		case '\\':
			if (escaped)
			{
				add_character(carriage, '\\');
				escaped = FALSE;
			}
			else
				escaped = TRUE;
		break;
		case '/':
			add_character(carriage, '/');
			if (!escaped)
			{
				carriage->next =
					  kmalloc(sizeof(struct __PATH_ELEMENT));
				if (carriage->next == NULL)
				{
					clean_path(list);
					return NULL;
				}
				memset(carriage->next, 0,
						 sizeof(struct __PATH_ELEMENT));
				carriage = carriage->next;
			}
			escaped = FALSE;
			break;
		default:
			add_character(carriage, path[idx]);
			escaped = FALSE;
		}
	}
	return list;
}
