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

#ifndef __VERSION_H
#define __VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VERSION
#define VERSION "'custom release'"
#endif

#define NAME "Betelgeuse (Alpha Orionis)"
#define YEARS "2011 - 2013"

#define WELCOME "Andromeda " VERSION " - " NAME\
"\nCopyright (C) " YEARS " - Michel Megens, Bart Kuivenhoven, Steven vd Schoot\n"\
"This program comes with ABSOLUTELY NO WARRANTY;\n"\
"This is free software, and you are welcome to redistribute it.\n"\
"For more info refer to the COPYING file in the source repository or look at\n"\
"http://www.gnu.org/licenses/gpl-3.0.html\n"

#ifdef __cplusplus
}
#endif

#endif
