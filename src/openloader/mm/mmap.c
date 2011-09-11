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

#include <mm/mmap.h>
#include <stdlib.h>
#include <textio.h>

ol_mmap_entry_t getmmapentry(ol_mmap_register_t mmr, uint32_t idx)
{
	return mmr->entry+idx;
}

void init_mmap(ol_mmap_register_t mmr)
{
	if(mmr->ecount <= 1) updatecmosmmap(mmr);
}

void display_mmap(ol_mmap_register_t mmr)
{
	ol_mmap_entry_t entry;
	
	int i = 0;
	uint32_t count = mmr->ecount;
	print("Base address");
	writeat('|', 20);
	putc(0x20);
	print("Length");
	writeat('|', 40);
	putc(0x20);
	print("Type");

	putc(0xa);
	for(; i < count; i++)
	{
		entry = getmmapentry(mmr, i);
		printnum((int)entry->base, 16, FALSE, FALSE);
		writeat('|', 20);
		putc(0x20);
		printnum((int)entry->len, 16, FALSE, FALSE);
		writeat('|', 40);
		putc(0x20);
		printnum(entry->type, 16, FALSE, FALSE);
		putc(0xa);
	}
}
