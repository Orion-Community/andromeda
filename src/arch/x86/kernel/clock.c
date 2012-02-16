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

#include <stdlib.h>
#include "clock.h"

#include <andromeda/clock.h>
#include <andromeda/drivers.h>
#include <andromeda/error.h>

#include <drivers/root.h>

/**
 * \var rtc
 * \brief Global variable for the real time clock.
 * 
 * At boot, this is the uninitialized rtc. The function <i>setup_rtc()</i> has
 * to be called to initialise the rtc and make sure it is kept up to date.
 */
struct device *rtc_dev;

/**
 * \fn setup_rtc
 * \brief Setup the RTC.
 * 
 * This function initialises the Real Time Clock object. It also registers and
 * initialises an IRQ/ISR to keep the RTC object up to date.
 */
int
setup_rtc()
{
        rtc_dev = kalloc(sizeof(*rtc_dev));
        if(rtc_dev == NULL)
                panic("No memory during RTC init!\n");
        rtc_create_driver(rtc_dev);
        RTC *rtc = kzalloc(sizeof(rtc));
        rtc->name = "clock0";
        rtc->frequency = 8*1000;
}

static int
rtc_create_driver(struct device *dev)
{
        dev_setup_driver(dev, NULL, NULL);
        dev->type = rtc;
        device_id_alloc(dev);
}
