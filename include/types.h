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
#ifndef __TYPES_H
#define __TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char boolean;
#define bool boolean
#define FALSE 0
#define TRUE (!FALSE)
#define false FALSE
#define true TRUE

typedef char           byte;
typedef unsigned short word;
typedef unsigned long  dword;
typedef unsigned long  ulong;

/*
 * Code untill end nicked, is nicked from FreeBSD
 */

#ifdef __GNUCLIKE_BUILTIN_VARARGS
typedef __builtin_va_list	__va_list;	/* internally known to gcc */
#else
typedef	char *			__va_list;
#endif /* __GNUCLIKE_BUILTIN_VARARGS */
#if defined(__GNUC_VA_LIST_COMPATIBILITY) && !defined(__GNUC_VA_LIST) \
    && !defined(__NO_GNUC_VA_LIST)
#define __GNUC_VA_LIST
typedef __va_list		__gnuc_va_list;	/* compatibility w/GNU headers*/
#endif

#ifndef _VA_LIST_DECLARED
#define	_VA_LIST_DECLARED
typedef	__va_list	va_list;
#endif

#ifdef __GNUCLIKE_BUILTIN_STDARG

#define	va_start(ap, last) \
	__builtin_va_start((ap), (last))

#define	va_arg(ap, type) \
	__builtin_va_arg((ap), type)

#if __ISO_C_VISIBLE >= 1999
#define	va_copy(dest, src) \
	__builtin_va_copy((dest), (src))
#endif

#define	va_end(ap) \
	__builtin_va_end(ap)

#else	/* !__GNUCLIKE_BUILTIN_STDARG */

#define	__va_size(type) \
	(((sizeof(type) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#ifdef __GNUCLIKE_BUILTIN_NEXT_ARG
#define va_start(ap, last) \
	((ap) = (va_list)__builtin_next_arg(last))
#else	/* !__GNUCLIKE_BUILTIN_NEXT_ARG */
#define	va_start(ap, last) \
	((ap) = (va_list)&(last) + __va_size(last))
#endif	/* __GNUCLIKE_BUILTIN_NEXT_ARG */

#define	va_arg(ap, type) \
	(*(type *)((ap) += __va_size(type), (ap) - __va_size(type)))

#if __ISO_C_VISIBLE >= 1999
#define	va_copy(dest, src) \
	((dest) = (src))
#endif

#define	va_end(ap)

#endif /* __GNUCLIKE_BUILTIN_STDARG */

/*
 * End nicked
 */
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned int uint;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

// typedef struct __TYPE_REGISTERS isrVal_t;
typedef unsigned long size_t;
typedef unsigned long long idx_t;
typedef long time_t; // Time variables are specified to be signed

typedef unsigned long addr_t;
typedef long reg;

#ifdef __cplusplus
}
#define NULL 0
#else
#define NULL (void*)0
#endif

#endif
