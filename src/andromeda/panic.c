/*
    Andromeda
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
#include <stdlib.h>

// Claim to have panicked, show the message and enter an infinite loop.
void panicDebug(char* msg, char* file, int line)
{
	stack_dump((int)&msg, 8);
#ifdef CAS
	// Little easter egg, a request from Cas van Raan
	printf("Shit's fucked up at line %i in file %s\n%s\nTry again!", line, file, msg);
#else
	printf("Andromeda panic in %s at line %i\n%s\n", file, line, msg);
#endif
	endProg(); // Halt and catch fire!
}

void asm_panic(char* msg)
{
	printf("Andromeda panic in assembly file!\n%s\n", msg);
	endProg();
}
