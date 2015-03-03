/*
 *  Andromeda
 *  Copyright (C) 2014 - 2015 Bart Kuivenhoven
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
static semaphore_t fd_cnt;

static unsigned int fd_alloc(struct vfile* file)
{
        if (file_descriptors == NULL)
                return 0;

        unsigned int attempt = 0;
        while (1) {
                attempt = (unsigned int) semaphore_inc(&fd_cnt);
                if (file_descriptors->find(attempt, file_descriptors) == NULL) {
                        file_descriptors->add(attempt, file, file_descriptors);
                        return attempt;
                }
        }
}

int fopen_sc(int arg_path, int path_len, int arg3 __attribute__((unused)))
{
        printf("Open syscall!\n");
        char* path = (char*) arg_path;

        if (path == NULL)
                return -E_NULL_PTR;
        if (path_len <= 0)
                return -E_INVALID_ARG;

        /**
         * \todo Add path pointer location check
         * Is the calling process allowed to even access this memory?
         * \todo Add invalid character check to file path
         */

        struct vfile* file = vfs_create();
        if (file == NULL)
                return -E_GENERIC;

        unsigned int fd = fd_alloc(file);

        file->open(file, path, path_len);

        return fd;
}

static int fclose_sc(int fd __attribute__((unused)),
                int arg2 __attribute__((unused)),
                int arg3 __attribute__((unused)))
{
        printf("Close syscall!\n");
        printf("FCLOSE_SC unimplemented!\n");

        return -E_NOFUNCTION;
}

static int fwrite_sc(int fd __attribute__((unused)),
                int str __attribute__((unused)),
                int cnt __attribute__((unused)))
{
        printf("Write syscall!\n");
        printf("FWRITE_SC unimplemented!\n");
        return -E_NOFUNCTION;
}

static int fread_sc(int fd __attribute__((unused)),
                int str __attribute__((unused)),
                int cnt __attribute__((unused)))
{
        printf("Read syscall!\n");
        printf("FREAD_SC unimplemented!\n");
        return -E_NOFUNCTION;
}

static int fseek_sc(int fd __attribute__((unused)),
                int mode __attribute__((unused)),
                int cnt __attribute__((unused)))
{
        printf("Seek syscall!\n");
        printf("FSEEK unimplemented\n");
        return -E_NOFUNCTION;
}

int file_sc_init()
{
        int ret = -E_SUCCESS;

        /* Set up the file IO system calls */
        ret |= sc_install(SYS_WRITE, fwrite_sc, 3);
        ret |= sc_install(SYS_READ, fread_sc, 3);
        ret |= sc_install(SYS_OPEN, fopen_sc, 3);
        ret |= sc_install(SYS_CLOSE, fclose_sc, 3);
        ret |= sc_install(SYS_SEEK, fseek_sc, 3);

        /* Make sure the file descriptors can be stored somewhere */
        file_descriptors = tree_new_avl();

        semaphore_init(&fd_cnt, 1, 0, (1 << (sizeof(void*) * 8 - 1)));

        struct vfile* stdin = vfs_create();
        struct vfile* stdout = vfs_create();
        struct vfile* stderr = vfs_create();
        if (stdin == NULL)
                printf("Error stdin\n");
        if (stdout == NULL)
                printf("Error stdout\n");
        if (stderr == NULL)
                printf("Error stderr\n");
        if (stdin == NULL || stdout == NULL || stderr == NULL)
                panic("Could not initialise stdio");

        file_descriptors->add(0, (void*) stdin, file_descriptors);
        file_descriptors->add(1, (void*) stdout, file_descriptors);
        file_descriptors->add(2, (void*) stderr, file_descriptors);

        /* Create a file descriptor for kernel stdio */
        /**
         * \todo Create a kernel file descriptor here
         */

        return ret;
}

