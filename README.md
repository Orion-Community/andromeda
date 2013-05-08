Andromeda Readme
================

Andromeda History
-----------------

The andromeda kernel is a system built by:

 * Bart Kuivenhoven.
 * Michel Megens
 * Steven van der Schoot.

We built this system to expand our knowledge about operating system theories.

Copying
-------

The Andromeda kernel is licenced as GNU General Public Licence version 3 or
newer. The COPYING file should contian more information about the licence, or
if it's not included, look at http://www.gnu.org/licences/.

    Andromeda kernel
    Copyright (C) 2011-2013 - Bart Kuivenhoven, Michel Megens, Steven vd Schoot

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

Building
--------

The requirements to build the kernel are relatively light.

To retreive the kernel:

* git

To compile:

* gcc
* g++
* ld
* nasm
* A working linux installation (for compilation without tweaks to the Makefiles)

To test:

* qemu

Contributing
------------

This kernel is free software and we welcome contributions of any form. Below is
a list of possible things you could help with, or you could just scratch your
own itch, that's just fine.

There's a number of ways to get your changes into the kernel code. One of those
is to send us a pull-request, and then send this to us in the google group/
mailing list. Of course you can also format patches and send those in to the
same group.

Github pull requests might work for some projects, but in order to keep the
development in a central place, the preference goes out to patches and pull
requests in the google group mailing list.

orion-os@googlegroups.com


TODO
----

* Provide a task swiching mechanism (in progress)
* Make a multitasking policy (in progress)
* Fork the first process
* Implement systemcalls (made start, no progress)
* Make stdin, stdout and stderr go through streams
* Implement pipes and stream through files
* Make kernel interpret memory map (complete)
* Make kernel use memory map (complete)
* Enable process protection mechanism (in progress)
* Build process protection policy
* Get the initrd from Grub
* Mount the initrd
* Start first task (/bin/init) from initrd
