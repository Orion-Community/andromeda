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

#include <mm/paging.h>
#include <stdlib.h>

#ifdef __INTEL

pageDir_t* setupPageDir()
{
  pageDir_t* pageDir = alloc(sizeof(pageDir_t), TRUE);
  if(pageDir == NULL)
  {
    panic("Aieee, Null pointer!!! Paging");
  }
  #ifdef __COMPRESSED
  #ifdef DBG
  printf("Start pointer: "); printhex((int)&mboot); putc('\n');
  printf("End pointer: "); printhex((int)&end); putc('\n');
  int j = 0;
  for (; j < 0x1FFFFFFF; j++);
  #endif
  #endif
  return pageDir;
}

void initPaging ()
{
  #ifdef WARN
  printf("Warning! The paging code hasn't been written yet\n");
  int i = 0;
  for (; i < 0x1FFFFFFF; i++);
  #else
  panic("Paging wasn't initialised!");
  #endif
  
  pageDir_t* kernDir = setupPageDir();
  
}

#endif