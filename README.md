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

Contributing
------------

This kernel is free software and we welcome contributions of any form. The todo
file should contain more info, or you can just scratch your own itch with the
kernel and send us a pull request. You're welcome to do so.

Bootloader info
---------------

This is a legacy field, still in place for the possibility that the kernel will
once be made bootable, without extra bootloader

Error table:

* 0x0 -> Error in the masterboot, probably because you haven't marked a partition 
    as active.
* 0x1 -> Stage 1 error. Serious failure, checks which succeeded in the masterboot 
    failed in stage 1. Should not occur, actually pure a leftover from early 
    debugging.
* 0x1.5 -> Look above
* 0x2 -> The second stage loader couldn't enable a feature such as the A20 line.
    This might indicate hardware problems if it occurs, or your hardware doesn't
    support the way they are supported.