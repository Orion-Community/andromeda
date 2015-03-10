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

#include <andromeda/drivers.h>

#define CMOS_RTC_TIMER 0xa
#define CMOS_RTC_ALARM 0xb
#define CMOS_RTC_IRQ 0xc
#define CMOS_RTC_VALID_BYTE 0xd

#define RTC_IRQ_LINE 0x8

/**
 * \def RTC_RATE_MAX
 * \brief Maxiumum RTC rate.
 *
 * The maximum timer rate is 8kHz or 122.070us.
 */
#define RTC_RATE_MAX 0x3

/**
 * \def RTC_RATE_SCHED
 * \brief Timer rate used for the scheduling by default.
 *
 * The schedular is called every 1/256 of a second (aka 3.90625ms).
 */
#define RTC_RATE_SCHED 0x6

/**
 * \def RTC_RATE_MIN
 * \brief Slowest RTC rate possible.
 *
 * The slowest rate possible for the RTC is 2Hz (500ms).
 */
#define RTC_RATE_MIN 0xF

/**
 * \struct RTC
 * \brief Describes a RTC object.
 * 
 * This structure describes a Real Time Clock object. It should only be used
 * within the arch/ sub-system. Other sub-systems which need time can use the
 * <i>_tm</i> structure.
 */
typedef struct _rtc {
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
        uint8_t rate;
        uint16_t frequency;

} rtc_t;

/**
 * \var rtc
 * \brief Global variable for the real time clock.
 * 
 * At boot, this is the uninitialized rtc. The function <i>setup_rtc()</i> has
 * to be called to initialise the rtc and make sure it is kept up to date.
 */
extern struct device *rtc_dev;

#if 0
/**
 * \fn rtc_create_driver(struct device *dev)
 * \brief Allocate the device driver.
 * \param dev Allocated device structure.
 * \return The error code. See error.h
 *
 * This function will allocate a new device driver and add an communication
 * file to the device. Returns 0 on success error code otherwise.
 */
static int rtc_create_driver(struct device *dev);

/**
 * \fn program_rtc(struct device *dev)
 * \param dev The clock device.
 * \brief Config the real time clock.
 * 
 * program_rtc configures the hardware RTC. It will set the clock to the correct
 * frequency and enable the IRQ.
 */
static void program_rtc(struct device *dev);
#endif

/**
 * \fn get_main_rtc()
 * \brief Returns the main rtc.
 * 
 * This function returns a pointer to the main Real Time Clock object. It's
 * a good practice of using this function instead of addressing the object
 * directly.
 */
static inline rtc_t*
get_main_rtc()
{
        return (rtc_t*) (rtc_dev->device_data);
}

/**
 * \fn read_rtc_value(RTC *clock)
 * \brief Get the current count of the real time clock.
 * \param clock The clock to read the timestamp from.
 * \return Current value of the real time clock.
 *
 * This function returns the current time stamp of the real time clock. This count
 * are the seconds since 1970.
 */
static inline unsigned long long read_rtc_value(rtc_t *clock)
{
        return clock->timestamp;
}

/**
 * \fn set_rtc_value(RTC *clock, unsigned long long timestamp)
 * \brief Set RTC stamp.
 * \param clock The real time clock to set.
 * \param timestamp The offset to set to the clock.
 * \warning <b>This function will overwrite the current count on the clock!</b>
 * 
 * With this function you can set an offset for the real time clock.
 */
static inline void set_rtc_value(rtc_t *clock, unsigned long long timestamp)
{
        clock->timestamp = timestamp;
}

/**
 * \fn get_rtc_frq(RTC *rtc)
 * \param rtc Configured real time clock object.
 * \brief Returns the frequention of a real time clock object.
 * \return Clock frequention in Hertz
 *
 * This function returns the configured frequention in Hertz of the given RTC
 * object. If the RTC is not configured correctly or when it is turned of the
 * return value will be 0.
 */
inline unsigned short get_rtc_frq(rtc_t *rtc);

#endif
