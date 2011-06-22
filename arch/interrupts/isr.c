/*
 *   GoldenEagle Bootloader C entry point.
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

#include <sys/stdlib.h>
#include <error/panic.h>
#include "include/interrupts.h"
#include <textio.h>

bool inKernelRing()
{
	struct gebl_segment_pack * segs = getsegs();
	uint8_t * error = "Not in kernel ring!";
	
	if((segs->cs & 0x8) == 0)
	{
		panic(error);
	}
	if((segs->ds & 0x10) == 0)
	{
		panic(error);
	}
	
	return TRUE;
}

void cDivByZero()
{
	panic("I define you as idiot. You just tried to divede by zero.");
}
void cNmi()
{
	panic("Not yet implemented");
}
void cBreakp()
{
	panic("Not yet implemented");
}
void cOverflow()
{
	panic("Not yet implemented");
}
void cBound()
{
	panic("Not yet implemented");
}
void cInvalOp()
{
	panic("Not yet implemented");
}
void cNoMath()
{
	panic("Not yet implemented");
}
void cDoubleFault()
{
	panic("Not yet implemented");
}
void cDepricated()
{
	panic("Not yet implemented");
}
void cInvalidTSS()
{
	panic("Not yet implemented");
}
void cSnp()
{
	panic("Not yet implemented");
}
void cStackFault()
{
	panic("Not yet implemented");
}
void cGenProt()
{
	panic("Not yet implemented");
}
void cPageFault()
{
	panic("Not yet implemented");
}
void cFpu()
{
	panic("Not yet implemented");
}
void cAlligned()
{
	panic("Not yet implemented");
}
void cMachine()
{
	panic("Not yet implemented");
}
void cSimd()
{
	panic("Not yet implemented");
}