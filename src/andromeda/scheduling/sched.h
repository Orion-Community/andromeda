/*
    Andromeda, Educative Kernel System.
    Copyright (C) 2012 Michel Megens

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


/**
 * \fn sched_switch_epoch()
 * \brief Switch to the next epoch.
 * \return Returns the next epoch head.
 * \warning UNTESTED!
 *
 * This function will reset and resort the current epoch and create a new one.
 */
static struct task_head *sched_switch_epoch();

/**
 * \fn starvation_watchdog()
 * \brief Checks if there is no starvation.
 * \TODO: Write the actual function.
 *
 * This function will set tasks temporarily to a lower priority if there is
 * starvation.
 */
static int sched_starvation_watchdog();

