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

#include <kern/timer.h>
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

boolean scheduling = FALSE;

struct __kern_timer *pit_timer;

void setupTimer(unsigned int freq, void* scheduler, void* hwInit)
{
  pit_timer->sentinels = FALSE;
  pit_timer->signals = FALSE;
  pit_timer->hwInit = hwInit;
  pit_timer->scheduler = scheduler;
  pit_timer->time = 0;
  pit_timer->freq = freq;
  pit_timer->microtime = 0;
  pit_timer->quantum = 0;
  pit_timer->hwInit(pit_timer->freq);
}

void setTimerFreq(int frequency)
{
  if (frequency > _TIME_FREQ_MIN && frequency < _TIME_FREQ_MAX)
  {
    pit_timer->freq = frequency;
    pit_timer->hwInit(frequency);
  }
  else
  {
    pit_timer->freq = _TIME_FREQ_MIN;
    pit_timer->hwInit(_TIME_FREQ_MIN);
  }
}

void timerTick()
{
  pit_timer->microtime ++;
  if (pit_timer->microtime%pit_timer->freq == 0)
  {
    pit_timer->time++;
    pit_timer->microtime = 0;
  }
  if (scheduling)
  {
    pit_timer->quantum--;
    if (pit_timer->quantum == 0)
      pit_timer->scheduler(pit_timer);
  }
  if (pit_timer->sentinels)
  {
  }
  if (pit_timer->signals)
  {
  }
}
