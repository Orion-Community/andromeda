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

#include <kern/syscall.h>

void syscall(int call_number, int arg1, int arg2, int arg3)
{
  switch(call_number)
  {
    case SYS_WRITE:
    case SYS_READ:
    case SYS_OPEN:
    case SYS_CLOSE:
    case SYS_CREATE:
      panic("File system related system calls aren't supported yet!");
      break;

    case SYS_YIELD:
    case SYS_FORK:
    case SYS_KILL:
    case SYS_SIG:
    case SYS_EXIT:
    case SYS_EXEC:
    case SYS_NICE:
      panic("Process related system calls aren't supported yet!");
      break;

    case SYS_BRK:
      panic("Memory management related system calls aren't supported yet!");
      break;

    case SYS_SHUTDOWN:
      shutdown();
      break;
    case SYS_REBOOT:
      reboot();
      break;
    case SYS_HIBERNATE:
    case SYS_STANDBY:
      panic("These system management system calls aren't supported yet!");
      break;
  }
}
