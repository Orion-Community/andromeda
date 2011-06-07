/*
 *   Functions under the chapter memory map.
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

#include "include/mmap.h"
#include <sys/stdlib.h>

// unsigned short getentrycount()
// {
// 	return *(getmmr()+4);
// }
unsigned int getmmapentries()
{
	unsigned int * mmr = getmmr();
	unsigned int * entryp = (unsigned int *) *(mmr);
	unsigned int ret = *(entryp+5);
	return ret;
// 	struct GEBL_MMR * mmr = (struct GEBL_MMR *) getmmr();
// 	struct GEBL_ENTRY * entry = mmr->entry;
// 	return entry->acpi;
}