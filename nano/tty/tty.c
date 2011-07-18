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

#include <tty/tty.h>
#include <stdio.h>
#include <stdlib.h>
#include <error/panic.h>

tty_t terminals[TERMINALS];

void tty_init()
{
  int i = 0;
  int j;
  for (; i < TERMINALS; i++)
  {
    terminals[i].frameBuf   = nalloc(2*VGA_HEIGHT*VGA_WIDTH);
    terminals[i].cursor_x   = 0;
    terminals[i].cursor_y   = 0;
    terminals[i].size_x     = VGA_WIDTH;
    terminals[i].size_y     = VGA_HEIGHT;
    terminals[i].screenmode = 0;
    for (j = 0; j < BUFFERS; j++)
    {
      terminals[i].buffers[j].buffer = nalloc(80*sizeof(char));
      if (terminals[i].buffers[j].buffer == NULL)
      {
	panic("Out of memory in tty_init");
      }
      terminals[i].buffers[j].size   = 80;
      terminals[i].buffers[j].cursor = 0;
      terminals[i].buffers[j].next   = NULL;
      terminals[i].buffers[j].full   = FALSE;
    }
  }
}

buffer_t *ttyFindNextBuffer(int tty, int buffer)
{
  if (tty >= TERMINALS || buffer == STDIN)
    return;
  buffer_t *current = &terminals[tty].buffers[buffer];
  
  for (; current->full == FALSE; current = current->next)
  {
    if (current->next == NULL)
    {
      current->next = initBuffer();
    }
  }
  return current;
}

void ttyWrite(int tty, int buffer, char* data)
{
  buffer_t *current = ttyFindNextBuffer(tty, buffer);
  
  unsigned int tomove = strlen(data);
  unsigned int index = 0;
  unsigned int moved = 0;
  for (; tomove!=0; tomove -= moved)
  {
    moved = (tomove >= current->size-current->cursor) ? current->size-current->cursor : tomove;
    void* dst = (void*)((unsigned long)current->buffer + current->cursor);
    void* src = (void*)((unsigned long)data + index);
    memset(dst, src, moved);
    index += moved;
    current->cursor += moved;
    if (current -> cursor == current -> size)
    {
      current->full = TRUE;
      current = ttyFindNextBuffer(tty, buffer);
    }
  }
}

void ttyUpdate(int tty, int buffer, size_t chars)
{
  // Memcpy from buffer, to tty screen buffer untill chars amount is transferred or the buffer is empty
}

void ttyFlush(int tty)
{
  // Set video mode according to tty
  // Memcpy the screenbuffer to framebuffer
}

char* ttyRead(int tty, int* buffer, int size)
{
  panic("bufRead hasn't been implemented yet");
}