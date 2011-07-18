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
    terminals[i].lock       = 0;
    for (j = 0; j < BUFFERS; j++)
    {
      buffer_t *tmp = nalloc(sizeof(buffer_t));
      if (tmp == NULL)
      {
	panic("Out of memory in tty_init");
      }
      terminals[i].buffers[j] = tmp;
      tmp->buffer = nalloc(80);
      if (tmp->buffer == NULL)
      {
	panic("Out of memory in tty_init");
      }
      tmp->size   = 80;
      tmp->cursor = 0;
      tmp->next   = NULL;
      tmp->full   = FALSE;
      tmp->lock   = 0;
    }
  }
}

buffer_t *ttyFindNextBuffer(int tty, int buffer)
{
  if (tty >= TERMINALS || buffer == STDIN)
    return;
  buffer_t *current = terminals[tty].buffers[buffer];
  
  for (; current->full == FALSE; current = current->next)
  {
    if (current->next == NULL)
    {
      mutexEnter(current->lock);
      current->next = initBuffer();
      mutexRelease(current->lock);
    }
  }
  return current;
}

void ttyWrite(int tty, int buffer, char* data)
{
  buffer_t *current = ttyFindNextBuffer(tty, buffer);
  mutexEnter(terminals[tty].lock);
  mutexEnter(current->lock);
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
      mutexRelease(current->lock);
      current = ttyFindNextBuffer(tty, buffer);
      mutexEnter(current->lock);
    }
  }
  mutexRelease(current->lock);
  mutexRelease(terminals[tty].lock);
}

char* ttyRead(int tty, int* buffer, int size)
{
  panic("bufRead hasn't been implemented yet");
}

void ttyUpdate(int tty, int buffer, size_t chars)
{
  buffer_t *current = terminals[tty].buffers[buffer];
  unsigned int done = 0;
  for(;chars != 0; chars -= done)
  {
    
  }
  // Memcpy from buffer, to tty screen buffer untill chars amount is transferred or the buffer is empty
}

void ttyFlush(int tty)
{
  // Set video mode according to tty
  // Memcpy the framebuffer to screenbuffer
}