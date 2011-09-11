/*
 *   The openLoader project - General system functions
 *   Copyright (C) 2011  Michel Megens
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

/* MultiProcessor defines */
#define OL_CPU_MP_FPS_SIGNATURE 0x5F504D5F
#define OL_CPU_MP_CONFIG_TABLE_HEADER_SIGNATURE 0x50434d50

#ifndef SYS_H
#define	SYS_H

#ifdef	__cplusplus
extern "C"
{
#endif

        static void
        ol_cpu_search_signature(void*, uint32_t);
        
        void
        ol_get_system_tables();
        
        static int
        ol_validate_table(char*);

#ifdef	__cplusplus
}
#endif

#endif	/* SYS_H */

