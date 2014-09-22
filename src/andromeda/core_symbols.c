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

#include <lib/tree.h>
#include <boot/mboot.h>
#include <andromeda/elf.h>

static struct tree_root* kernel_symbols;

int core_symbols_init(struct multiboot_elf_section_header_table* table)
{
        if (table == NULL || table->num == 0) {
                printf("table: %X\n", (int)table);
                if (table != NULL)
                        printf("num: %X\n", table->num);
                return -E_NOT_FOUND;
        }

        kernel_symbols = tree_new_avl();
        if (kernel_symbols == NULL)
                return -E_NOMEM;

        Elf32_Shdr* array = (Elf32_Shdr*)table->addr;
        Elf32_Shdr* a;

        Elf32_Shdr* symbol_table = NULL;
        Elf32_Shdr* string_table = NULL;

        idx_t i = 0;
        for (; i < table->num; i++)
        {
                a = &array[i];
                if (a->sh_type == SHT_SYMTAB) {
                        symbol_table = a;
                } else if (a->sh_type == SHT_STRTAB) {
                        printf("Setting string table!\n");
                        string_table = a;
                }
        }

        printf("Symbol table: %X\nString table: %X\n", (int)symbol_table, (int)string_table);
        return -E_SUCCESS;
}

char* get_symbol (void* addr __attribute__((unused)))
{
        return NULL;
}

int set_symbol (void* addr __attribute__((unused)), char* name __attribute__((unused)), int len __attribute__((unused)))
{
        return -E_NOFUNCTION;
}
