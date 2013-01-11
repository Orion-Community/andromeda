/*
 *  Andromeda
 *  Copyright (C) 2011  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** \file */

#include <stdlib.h>

#include <andromeda/drivers.h>
#include <andromeda/irq.h>

#ifndef __KERN_TIMER_H
#define __KERN_TIMER_H

#define _TIME_FREQ_MAX 0xFFFFFFFF
#define _TIME_FREQ_MIN 0x0

#ifdef __cplusplus
extern "C" {
#endif

struct timer;

typedef void (*timer_tick_t)(struct timer*);

/**
 * \typedef timer_state_t
 * \brief Describes the current state of a timer.
 * \see struct timer
 */
typedef enum
{
        /**
         * \enum TIMER_STOPPED
         * \brief Indicates that the timer should not fire anymore
         */
        TIMER_STOPPED,
        /**
         * \enum TIMER_ACTIVE
         * \brief Indicates that the timer is active and running.
         */
        TIMER_ACTIVE,
} timer_state_t;

/**
 * \struct timer
 * \brief Basic timer structure.
 * \var TIMER
 * \brief Typedef of struct timer.
 */
typedef struct timer
{
        /**
         * \var name
         * \brief Unique name for the timer
         *
         * \var frq
         * \brief The timer frequency.
         *
         * \var tick
         * \brief The timer tick.
         *
         * \var handle
         * \brief The irq handle which is called every time.
         *
         * \var timer_data
         * \brief Private data specific for each different timer.
         */
        /* object specific shizzle */
        char *name;
        unsigned long long id;

        /* timer config */
        unsigned int frq;
        unsigned long long tick;
        timer_tick_t tick_handle;
        void *timer_data;
        unsigned char config,
                      mode;

        /* configurators */
        void (*set_frq)(struct timer*);
        void (*set_mode)(struct timer*);
        void (*set_tick)(struct timer*, unsigned long long);
} TIMER;

/**
 * \var scheduling
 * \brief Indicates the activity of the scheduler.
 *
 * When set to false, the scheduler is not acitve and there should be performed
 * any I/O actions with the scheduler before it is (re-)enabled.
 * TRUE means full functionality of the scheduler.
 */
extern volatile boolean scheduling;
#if 0
/**
 * \fn dev_timer_setup_io(struct device *dev)
 * \brief Setup the device I/O.
 * \param dev Device to create I/O for.
 * \param read File read function.
 * \param write File write routine.
 * \return Error code.
 *
 * This function creates a virtual file to communicate with the device.
 */
static int dev_timer_setup_io(struct device *, vfs_read_hook_t, vfs_write_hook_t);

/**
 * \fn init_timer_obj(char *name, timer_tick_t tick_handle, void *data)
 * \brief Create a new timer.
 * \param name Unique name for this timer.
 * \param tick_handle The routine which will handle timer IRQ's for this timer.
 * \param data Additional data to store.
 * \return The created timer object. NULL if creation failed.
 *
 * This function will create a new TIMER object and initialise a correspondending
 * device structure. Also it will setup an interrupt.
 */
static TIMER *create_timer_obj(char *name, timer_tick_t tick_handle, void *data);

/**
 * \fn dev_timer_init(struct device *dev, struct device *parent)
 * \brief Setup the timer device.
 * \param dev The timer device.
 * \param parent The parent device of the timer.
 * \return The error code. Zero on success, non-zero on failure.
 *
 * This function will setup the device structure for a timer.
 */
static int dev_timer_init(struct device* dev, struct device* parent);

static int setup_timer_irq(TIMER *timer, bool forse_vec, unsigned char vector);
#endif
TIMER *init_timer_obj(char *name, timer_tick_t tick_handle, void *data,
                                        bool forse_vec, unsigned char vec);

static inline unsigned long long
get_timer_tick(TIMER *timer)
{
        return timer->tick;
}

static inline void
set_timer_tick(TIMER *timer, unsigned long long tick)
{
        timer->tick = tick;
}

static inline unsigned short
get_timer_frq(TIMER *timer)
{
        return timer->frq;
}

void setupTimer(unsigned int freq, void* scheduler, void* hwInit);
void setTimerFrequency(int);
extern struct __kern_timer *timer;

#ifdef __cplusplus
}
#endif

#endif
