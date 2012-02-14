/*
 *   Andromeda Project - The Real Time Clock.
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

#ifndef __CLOCK
#define __CLOCK

#include <stdlib.h>

/**
 * \struct _tm
 * \brief Describes a moment in time.
 */
typedef struct _tm
{
        /**
         * \var seconds
         * \brief Seconds of a minute.
         * \var minutes
         * \brief Minute of an hour.
         * \var hour
         * \brief Hour of a day.
         * \var w_day
         * \brief Day of the week.
         * \var m_day
         * \brief Day of the month.
         * \var month
         * \brief Month of the year.
         * \var year
         * \brief Year since 1900.
         */
        unsigned char seconds;
        unsigned char minutes;
        unsigned char hour;
        unsigned char w_day;
        unsigned char m_day;
        unsigned char month;
        unsigned int  year;
        
} tm;

/**
 * \brief Initializes the real time clock.
 *
 * \return The error code.
 */
int setup_rtc();

#endif
