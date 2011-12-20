/*
    Orion OS, The educational operatingsystem
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
#include <fs/file.h>
#include <fs/vfs.h>
#include <Andromeda/error.h>

directory *root = NULL;

int init_vfs(file_t *device, uint32_t inode)
{
	if (root != NULL)
		return -E_FS_INIT;
	directory *vroot = kalloc(sizeof(directory));
	if (vroot == NULL)
		return -E_FS_INIT;
	memset(vroot, 0, sizeof(directory));
	vroot->entries = kalloc(sizeof(struct __DIR_ENTRIES));
	if (vroot->entries == NULL)
	{
		free(vroot);
		return -E_FS_INIT;
	}
	vroot->device = NULL;
	vroot->inode = 0;
	memset(vroot->entries, 0, sizeof(struct __DIR_ENTRIES));

	root = vroot;
	return -E_SUCCESS;
}

int make_dir(char* path, char* name)
{
	return -E_NOFUNCTION;
}