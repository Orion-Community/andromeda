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

#include <andromeda/syscall.h>
#include <andromeda/sched.h>
#include <andromeda/error.h>
#include <stdlib.h>

/* Gnu compiler bug seems to find all sorts of bogus out of bounds ... */
#pragma GCC diagnostic ignored "-Warray-bounds"

#define SC_LIST_SIZE 0x100

struct syscall sc_list[SC_LIST_SIZE];
int sc_initialised = 0;

int sc_write(int file, int string, int count)
{
        char* str = (char*)string;
        if (file == 0)
        {
                int i = 0;
                for (; i < count; i++)
                        putc(str[i]);
        }
        else
                printf("Write system call not yet implemented!\n");

        return -E_SUCCESS;
}

int sc_init()
{
        memset(&sc_list, 0, SC_LIST_SIZE*sizeof(sc_list[0]));

        sc_initialised = 1;
        if (file_sc_init() != -E_SUCCESS)
                panic("File calls not initialised!");

        return 0;
}

int sc_install (uint16_t idx, sc call, uint8_t cpl)
{
        if (sc_initialised == 0)
                sc_init();

        if (idx >= SC_LIST_SIZE)
                return -E_INVALID_ARG;
        if (call == NULL)
                return -E_NULL_PTR;
        if (sc_list[idx].syscall != NULL)
                return -E_ALREADY_INITIALISED;

        sc_list[idx].syscall = call;
        sc_list[idx].cpl = cpl;
        return -E_SUCCESS;
}

int sc_uninstall(uint16_t idx)
{
        if (idx < SC_LIST_SIZE)
                return -E_INVALID_ARG;

        sc_list[idx].cpl = 0;
        sc_list[idx].syscall = NULL;
        return -E_SUCCESS;
}

extern int arch_syscall(int,int,int,int);

int sc_call(uint16_t idx, uint8_t cpl, reg reg1, reg reg2, reg reg3)
{
        if (idx >= SC_LIST_SIZE)
                return -E_INVALID_ARG;

        if (sc_list[idx].syscall == NULL)
                return -E_NULL_PTR;
        if (sc_list[idx].cpl < cpl)
                return -E_UNAUTHORISED;

        return arch_syscall(idx, reg1, reg2, reg3);
}
