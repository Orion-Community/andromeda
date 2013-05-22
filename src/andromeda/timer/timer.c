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

#include <andromeda/timer.h>
#include <stdlib.h>

struct __kern_timer
{
	boolean sentinels;
	boolean signals;
	void (*hwInit)(unsigned int freq);
	void (*scheduler)(struct __kern_timer*);
	time_t time;
	time_t freq;
	time_t microtime;
	time_t quantum;
};

struct __kern_sig
{
	time_t time;
	time_t microtime;
	void (*function)();
};

struct __kern_timer *timer;

void setupTimer(unsigned int freq, void* scheduler, void* hwInit)
{
	timer->sentinels = FALSE;
	timer->signals = FALSE;
	timer->hwInit = hwInit;
	timer->scheduler = scheduler;
	timer->time = 0;
	timer->freq = freq;
	timer->microtime = 0;
	timer->quantum = 0;
	timer->hwInit(timer->freq);
}

void setTimerFreq(unsigned int frequency)
{
	if (frequency > _TIME_FREQ_MIN && frequency < _TIME_FREQ_MAX)
	{
		timer->freq = frequency;
		timer->hwInit(frequency);
	}
	else
	{
		timer->freq = _TIME_FREQ_MIN;
		timer->hwInit(_TIME_FREQ_MIN);
	}
}

void timerTick()
{
	timer->microtime ++;
	if ((timer->microtime%timer->freq) == 0)
	{
		timer->time++;
		timer->microtime = 0;
	}
	if (scheduling)
	{
		timer->quantum--;
		if (timer->quantum == 0)
			timer->scheduler(timer);
	}
	if (timer->sentinels)
	{
	}
	if (timer->signals)
	{
	}
}
