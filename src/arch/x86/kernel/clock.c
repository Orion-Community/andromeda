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
#include <io.h>
#include "clock.h"

#include <andromeda/clock.h>
#include <andromeda/drivers.h>
#include <andromeda/error.h>
#include <andromeda/panic.h>
#include <andromeda/irq.h>
#include <andromeda/system.h>

#include <drivers/root.h>

/**
 * \var rtc_dev
 * \brief Global variable for the real time clock.
 * 
 * At boot, this is the uninitialized rtc. The function <i>setup_rtc()</i> has
 * to be called to initialise the rtc and make sure it is kept up to date.
 */
struct device *rtc_dev;

/**
 * \var hz_table[]
 * \brief Rate to hertz converter.
 * 
 * This array (/table) converts a RTC rate to the associated frequention in
 * Hertz.
 */
static const unsigned short hz_table[] =
{
        0,      256,   128,   0x2000, 
        0x1000, 0x800, 0x400, 0x200, 
        0x100,  0x80,  0x40,  0x20, 
        0x10,   8,     4,     2
};

/**
 * \fn get_rtc_frq(RTC *clock)
 * \param clock Configured real time clock object.
 * \brief Returns the frequention of a real time clock object.
 * \return Clock frequention in Hertz
 * 
 * This function returns the configured frequention in Hertz of the given RTC
 * object. If the RTC is not configured correctly or when it is turned of the
 * return value will be 0.
 */
inline unsigned short 
get_rtc_frq(RTC *clock)
{
        return hz_table[clock->rate];
}

/**
 * \fn setup_rtcan
 * \brief Setup the RTC.
 * 
 * This function initialises the Real Time Clock object. It also registers and
 * initialises an IRQ/ISR to keep the RTC object up to date.
 */
int
setup_rtc(void)
{
        rtc_dev = kmalloc(sizeof(*rtc_dev));
        if(rtc_dev == NULL)
                panic("No memory during RTC init!\n");
        rtc_create_driver(rtc_dev);
        RTC *clock = kmalloc(sizeof(*clock));
        if(clock == NULL)
                return -E_NOMEM;
        memset(clock, 0, sizeof(*clock));
        clock->name = "clock0";
        clock->rate = RTC_RATE_SCHED;
        rtc_dev->device_data = clock;
        program_rtc(rtc_dev);
}

/**
 * \fn rtc_create_driver(struct device *dev)
 * \brief Allocate the device driver.
 * \param dev Allocated device structure.
 * \return The error code. See error.h
 *
 * This function will allocate a new device driver and add an communication
 * file to the device. Returns 0 on success error code otherwise.
 */
static int
rtc_create_driver(struct device *dev)
{
        dev_setup_driver(dev, NULL, NULL);
        dev->type = rtc;
        device_id_alloc(dev);
}

/**
 * \fn program_rtc(struct device *dev)
 * \param dev The clock device.
 * \brief Config the real time clock.
 * 
 * program_rtc configures the hardware RTC. It will set the clock to the correct
 * frequency and enable the IRQ. The RTC will be programmed to a periodic
 * clock/interrupt.
 */
static void
program_rtc(struct device *dev)
{
        RTC *rtc = (RTC*)dev->device_data;
        disable_irqs();
        
        /* select CMOS register B to set the RTC to a periodic clock. */
        outb(CMOS_SELECT, CMOS_RTC_ALARM);
        unsigned char val = inb(CMOS_DATA);
        
        outb(CMOS_SELECT, CMOS_RTC_ALARM);
        outb(CMOS_DATA, val |0x40);

        /* set the RTC frequency */
        outb(CMOS_SELECT, CMOS_RTC_TIMER);
        val = inb(CMOS_DATA);

        outb(CMOS_SELECT, CMOS_RTC_TIMER);
        outb(CMOS_DATA, (val & 0xF0) | (rtc->rate & 0xF));
        enable_legacy_irq(RTC_IRQ_LINE);
        enable_irqs();
}
