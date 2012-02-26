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

#ifndef __ERROR_H
#define __ERROR_H

typedef enum
{
        OK = 0,
        NULL_PTR,
        GENERAL_ERR,
        NO_MEM,
} error_t;

typedef enum
{
        HOOK_DONE,
        HOOK_NOT_DONE,
} hook_result_t;

#endif