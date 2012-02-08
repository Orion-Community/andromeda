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
    Copyright (C) 2010-2011 - Bart Kuivenhoven, Michel Megens, Steven vd Schoot

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

To test:

* qemu

Contributing
------------

This kernel is free software and we welcome contributions of any form. Below is
a list of possible things you could help with. Or you could just scratch your
own itch, that's just fine.

Sprint 1
--------

Deadline: 07-03-2012

* provide task switching mechanism (Mechanism)
* Make a multitasking system (Policy)
* Fork the first kernel process (Policy)
* Implement systemcalls policy (Policy)
* Make stdio go through streams (stdin, stdout, stderr) (Policy)
* Implement pipes and streams through files (Policy)

Sprint 2
--------

Deadline: 07-09-2012

* Allow kernel to use provided memory map (Mechanism)
* enable process protection mechanism (userspace) (Mechanism)
* Make the kernel actually use the memory map (Dynamically scale heap) (Policy)
* Build process protection policy (userspace) (Policy)

Sprint 3
--------

Deadline: 07-03-2013

* Read an initrd from Grub (Mechanism)
* Mount the initrd (Policy)
* start the first task from initrd (Policy)