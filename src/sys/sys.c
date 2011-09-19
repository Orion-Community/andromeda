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

#include <stdlib.h>
#include <text.h>
#include <sys/sys.h>

#include <arch/x86/cpu.h>
#include <arch/x86/acpi/acpi.h>

static void
ol_cpu_search_signature(void* mem, uint32_t c)
{
  int i;
  for (i = 0; i < c; i += 16)
  {
    ol_validate_table(mem + i);
  }
}

void
ol_get_system_tables()
{ /* get the ebda pointer */
  uint16_t ebda = *((uint16_t*) ((uint32_t) (0x040E)));
  uint16_t len = ((ebda << 4) + 0x400)-(ebda << 4);

#if 0
  test();
#endif

  /* search */
  ol_cpu_search_signature((void*) (ebda << 4), len);
  ol_cpu_search_signature((void*) 0x9fc00, 0x400);
  ol_cpu_search_signature((void*) 0xe0000, 0x20000);
}

static uint8_t
ol_validate_table(uint8_t* table)
{
  int i;
  uint8_t checksum, length;
  if (!memcmp(table, "_MP_", 4))
  {
    checksum = 0;
    length = *(table + 8)*16;
    for (i = 0; i < length; i++)
    {
      checksum += *(table + i);
    }
    if (!checksum)
      mp = (ol_cpu_mp_fps_t) table;
  }
  else if (!memcmp(table, "_SM_", 4))
  {
    if ((addr_t) table > 0xf0000)
    {
      checksum = 0;
      length = *(table + 5);
      for (i = 0; i < length; i++)
      {
        checksum += *(table + i);
      }
      if (!checksum);
        putc('c');
    }
  }
  else if (!memcmp(table, "RSD PTR ", 8))
  {
    length = 20;
    checksum = 0;
    for (i = 0; i < length; i++)
    {
      checksum += *(table + i);
    }
    if (!checksum)
      rsdp = (ol_acpi_rsdp_t) table;
  }

  return checksum;
}
