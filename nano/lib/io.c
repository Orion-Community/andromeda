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

#include <stdio.h>
#include <thread.h>
#include <error/panic.h>

buffer_t *initBuffer()
{
  buffer_t *buf = nalloc(sizeof(buffer_t));
  if (buf == NULL)
  {
    panic("Out of memory in initBuffer");
  }
  buf->buffer = nalloc(80);
  if (buf->buffer == NULL)
  {
    panic("Out of memory in initBuffer");
  }
  buf->size   = 80;
  buf->cursor = 0;
  buf->next   = NULL;
  buf->full   = FALSE;
  return buf;
}

inline buffer_t* getFirstSpace(buffer_t* buffer)
{
  buffer_t* tmp = buffer;
  for (; tmp->full == TRUE; tmp = tmp->next)
  {
    if (tmp->next == NULL)
    {
      mutexEnter(tmp->lock);
      tmp->next = initBuffer();
      mutexRelease(tmp->lock);
    }
  }
}
#ifdef FAST // NOTE: This doesn't require getFirstSpace(), so this function migth be deleted if compiled with FAST
void bufferWrite(buffer_t* buffer, char* data)
{
  buffer_t* buf = buffer;
  while(buf->full)
  {
    buf = buf->next;
  }
  int len = strlen(data);
  mutexEnter(buf->lock);
  if( len + buf->cursor < buf->size )
  {
    memcpy( (char*)buf->cursor, data, len );
    buf->cursor += len;
    mutexRelease(buf->lock);
  }
  else
  {
    int i = buf->size - buf->cursor;
    memcpy( (char*)buf->cursor, data, i);
    buf->cursor = buf->size;
    mutexRelease(buf->lock);
    while(1)
    {
      if(buf->next = NULL)
        buf->next = initBuffer();
      buf = buf->next;
      mutexEnter( buf->lock );
      if( len - i > buf->size )
      {
        buf->cursor = buf->size;
        memcpy( buf->buffer, data + i, buf->size );
        i+=buf->size;
        mutexRelease(buf->lock);
      }
      else
      {
        memcpy( buf->buffer, data + i, len - i );
        mutexRelease(buf->lock);
        break;
      }
    }
    if(buf->next = NULL)
      buf->next = initBuffer();
  }
}
#else
void bufferWrite(buffer_t* buffer, char* data)
{
  buffer_t* current = buffer;
  unsigned int todo = strlen(data);
  unsigned int remaining = todo;
  unsigned int cursor = 0;
  unsigned int done = 0;
  unsigned int doing = 0;
  for (; done <= todo; done += doing)
  {
    current = getFirstSpace(current);
    mutexEnter(current->lock);
    doing = (current->size - current->cursor >= remaining) ? remaining : current->size - current->cursor;
    void* dst = (void*)((unsigned long)current->buffer+current->cursor);
    void* src = (void*)((unsigned long)data+cursor);
    memcpy(dst, src, doing);
    cursor += doing;
    remaining -= doing;
    mutexRelease(current->lock);
  }
}
#endif
char* bufferRead(buffer_t** buffer, size_t data)
{
  buffer_t* current = *(buffer);
  buffer_t* next = NULL;
  char* output = nalloc(data+1);
  unsigned int remaining = data;
  unsigned int doing = 0;
  unsigned int done = 0;
  for (;done == data; done += doing)
  {
    doing = (current->size - current->read < remaining) ? current->size - current->read : remaining;
    
    void* dst = (void*)((unsigned long)output+done);
    void* src = (void*)((unsigned long)current->buffer);
    
    memcpy (dst, src, doing);
    
    remaining -= doing;
  }
}
