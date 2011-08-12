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

#define __COMPRESSED

#ifndef __COMPRESSED
extern long long timer;
#endif

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
    #ifndef __COMPRESSED
    seedTwo = timer;
    seedOne = timer;
    #else
    seedTwo = 0xDEADBEEF;
    seedOne = 0xCAFEBABE;
    #endif
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

#define PI 3.14159265358979

double sin(double x)
{
  int            i   =  5,
                 c   = -1;
  long long int  iF  =  6;  // should be seen as i! ( = i* (i-1) * (i-2) * ... * 2 * 1 )
  double         ret =  x,
                 x2  =  ret*ret,
                 xF  =  x2 *ret;
  if ( x < 0 )
  {
    ret *= -1;
    c    =  1;
    x    = -x;
  }
  while ( x > PI )
    x -= PI;
  for (;i<20;i+=2) // if iF<0xffffffff/(i * (i - 1)) if true iF whould become > 0xffffffff (size of 1 int) after multiplying with i * (i - 1)
  {
    ret += c * xF / iF;
    xF  *= x2;
    iF  *= i * (i - 1);
    c   *= -1;
  }
  ret += c * xF / iF;
  //printf("%d\n",ret); // Calculated value is corrent, printed value is correct, but returned value is incoreect and even changes if by commenting/uncommenting this rule...
  return ret;
}

/**
 * Less presies, but faster.
 * 
double sin(double x) // Doesn't work jet :(...
{
  int    i   =  5,
         c   = -1,
         iF  =  6;  // should be seen as i! ( = i* (i-1) * (i-2) * ... * 2 * 1 )
  double ret =  x,
         x2  =  ret*ret,
         xF  =  x2 *ret;
  if ( x < 0 )
  {
    ret *= -1;
    c    =  1;
    x    = -x;
  }
  while ( x > PI )
    x -= PI;
  for (;iF<0xffffffff/(i * (i - 1));i+=2) // if iF<0xffffffff/(i * (i - 1)) if true iF whould become > 0xffffffff (size of 1 int) after multiplying with i * (i - 1)
  {
    ret += c * xF / iF;
    xF  *= x2;
    iF  *= i * (i - 1);
    c   *= -1;
  }
  ret += c * xF / iF;
  //printf("%d\n",ret);
  return ret;
}
*/
float sinf(float x)
{
  int    i   =  3,
         c   = -1;
  int    iF  =  6;  // should be seen as i! ( = i* (i-1) * (i-2) * ... * 2 * 1 )
  float  ret =  x,
         x2  =  x*x,
         xF  =  x2*x;
  if ( x < 0 )
  {
    ret *= -1;
    c    =  1;
    x    = -x;
  }
  while ( x > 2*PI )
    x -= 2*PI;
  for (;iF<0xffffffff/(i * (i - 1));i+=2) // if iF<0xffffffff/(i * (i - 1)) if true iF whould become > 0xffffffff (size of 1 int) after multiplying with i * (i - 1)
  {
    ret += c * xF / iF;
    xF  *= x2;
    iF  *= i * (i - 1);
    c   *= -1;
  }
  return ret + c * xF / iF;
}

long double sinl(long double x) // Doesn't work jet :(...
{
  int         i   =  3,
              c   = -1,
              iF  =  6;  // should be seen as i! ( = i* (i-1) * (i-2) * ... * 2 * 1 )
  long double ret =  x,
              x2  =  ret*ret,
              xF  =  x2 *ret;
  if ( x < 0 )
  {
    ret *= -1;
    c    =  1;
    x    = -x;
  }
  while ( x > 2*PI )
    x -= 2*PI;
  for (;iF<0xffffffff/(i * (i - 1));i+=2) // if iF<0xffffffff/(i * (i - 1)) if true iF whould become > 0xffffffff (size of 1 int) after multiplying with i * (i - 1)
  {
    ret += c * xF / iF;
    xF  *= x2;
    iF  *= i * (i - 1);
    c   *= -1;
  }
  return ret + c * xF / iF;
}

/*
 * This is kind of a problem, because by 'standart' formats all 3 functions
 * whould exist. Only gcc doesn't allow that...
 * 
double sin(double x)
{
  double ret = 1.0; //I know, this is incorect
  return ret;
}

long double sin(long double x)
{
  long double ret = 1.0; //I know, this is incorect
  return ret;
}
*/
