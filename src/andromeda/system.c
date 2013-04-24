/*
 *  Andromeda
 *  Copyright (C) 2013  Bart Kuivenhoven
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

#include <andromeda/system.h>
#include <andromeda/error.h>
#include <types.h>

struct system core = {NULL, NULL, NULL, NULL, NULL, NULL};

int sys_setup_mm()
{
        return -E_NOFUNCTION;
}
int sys_setup_arch()
{
        return -E_NOFUNCTION;
}
int sys_setup_devices()
{
        return -E_NOFUNCTION;
}
int sys_setup_modules()
{
        return -E_NOFUNCTION;
}
int sys_setup_fs()
{
        return -E_NOFUNCTION;
}
int sys_setup_net()
{
        return -E_NOFUNCTION;
}
