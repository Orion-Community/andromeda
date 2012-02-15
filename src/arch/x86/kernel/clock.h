/*
 *   Andromeda Project - The Real Time Clock. - Private header.
 *   Copyright (C) 2011  Michel Megens - dev@michelmegens.net
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

/** \file */

#ifndef __X86_CLOCK
#define __X86_CLOCK

#include <stdlib.h>

/**
 * \struct RTC
 * \brief Describes a RTC object.
 * 
 * This structure describes a Real Time Clock object. It should only be used
 * within the arch/ sub-system. Other sub-systems which need time can use the
 * <i>_tm</i> structure.
 */
typedef struct _rtc
{
        /**
         * \var name
         * \brief Specific, unique name for a RTC.
         * 
         * \var timestamp
         * \brief The current timestamp since 1970.
         * 
         * \var frequency
         * \brief The clock frequency in hertz.
         */
        char *name;
        time_t timestamp;
        uint16_t frequency;
        
} RTC;

/**
 * \var rtc
 * \brief Global variable for the real time clock.
 * 
 * At boot, this is the uninitialized rtc. The function <i>setup_rtc()</i> has
 * to be called to initialise the rtc and make sure it is kept up to date.
 */
extern RTC rtc;

/**
 * \fn get_main_rtc()
 * \brief Returns the main rtc.
 * 
 * This function returns a pointer to the main Real Time Clock object. It's
 * a good practice of using this function instead of addressing the object
 * directly.
 */
static inline RTC*
get_main_rtc()
{
        return &rtc;
}

#endif
