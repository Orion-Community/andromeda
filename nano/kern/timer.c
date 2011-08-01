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

boolean scheduling = FALSE;
extern boolean sentinels;
extern boolean signals;
void (*scheduler) ();
time_t timer = 0;
time_t freq = 0;
time_t microtime = 0;
time_t quantum = 0;

void setTimerFreq(int frequency)
{
  if (frequency > _TIME_FREQ_MIN && frequency < _TIME_FREQ_MAX)
  {
    freq = frequency;
  }
}

void timerTick()
{
  microtime--;
  if (!microtime)
  {
    timer ++;
    microtime = freq;
  }
  if (scheduling)
  {
    quantum--;
    if (quantum == 0)
      scheduler();
  }
  if (sentinels)
  {
  }
  if (signals)
  {
  }
}
