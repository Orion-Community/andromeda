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

#include <fs/vfs.h>
#include <andromeda/error.h>
#include <andromeda/syscall.h>
#include <lib/tree.h>

static struct tree_root* file_descriptors = NULL;

static unsigned int fd_cnt = 1;

static unsigned int fd_alloc()
{
        while (file_descriptors->find(fd_cnt, file_descriptors) != NULL)
                fd_cnt ++;
        return fd_cnt;
}

static int fopen_sc(int name, int path, int arg3)
{
        printf("Open syscall!\n");

        unsigned int fd = fd_alloc();
        struct vfile* file = NULL;

        return -E_NOFUNCTION;
}

static int fclose_sc(int fd, int arg2, int arg3)
{
        printf("Close syscall!\n");
        return -E_NOFUNCTION;
}

static int fwrite_sc(int fd, int str, int cnt)
{
        printf("Write syscall!\n");
        return -E_NOFUNCTION;
}

static int fread_sc(int fd, int str, int cnt)
{
        printf("Read syscall!\n");
        return -E_NOFUNCTION;
}

static int fseek_sc(int fd, int mode, int cnt)
{
        printf("Seek syscall!\n");
        return -E_NOFUNCTION;
}

int file_sc_init()
{
        int ret = -E_SUCCESS;

        /* Set up the file IO system calls */
        ret |= sc_install(SYS_WRITE, fwrite_sc, 3);
        ret |= sc_install(SYS_READ,  fread_sc,  3);
        ret |= sc_install(SYS_OPEN,  fopen_sc,  3);
        ret |= sc_install(SYS_CLOSE, fclose_sc, 3);
        ret |= sc_install(SYS_SEEK,  fseek_sc,  3);

        /* Make sure the file descriptors can be stored somewhere */
        file_descriptors = tree_new_avl();

        /* Create a file descriptor for kernel stdio */
        /**
         * \todo Create a kernel file descriptor here
         */

        return ret;
}

