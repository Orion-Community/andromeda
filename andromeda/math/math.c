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
#include <math/math.h>
#include <mm/memory.h>

extern long long timer;

unsigned int seedTwo = 1; // Store the seeds
unsigned int seedOne = 1;

void randomize(unsigned int s) // Create the seed
{
  if (s!=0)
  {
    seedTwo = s;
    seedOne = s/2;
    if (seedOne == 0)
    {
      seedOne = 0x0BADB002;
    }
  }
  else
  {
    seedTwo = timer;
    seedOne = timer;
  }
}

int randomA() // Hash the stuff and make it random.
{
  // Doesn't need to be thread safe for extra randomness.
  if (seedTwo == 0)
  {
    seedTwo = 1;
  }
  if (seedOne == 0)
  {
    seedOne = 1;
  }
  int ret1 = 36969 * (seedOne & 0xFFFF) + (seedTwo >> 16);
  int ret2 = 18000 * (seedTwo & 0xFFFF) + (seedOne >> 16);
  
  int ret = (ret1 << 16) + ret2;
  
  seedOne = ret1; // Set the seeding values
  seedTwo = ret2;
  
  return ret;
}

int random()
{
  int ret = randomA();
  ret %= RANDMAX;
  ret += RANDMIN;
  
  if (ret < 0)
  {
    ret *= -1;
  }
}