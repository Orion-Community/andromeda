#include <math/math.h>
#include <mm/memory.h>

extern long long timer;
int seedNew = 1;
int seedOld = 1;

void randomise(int s)
{
  if (s!=0)
  {
    seedNew = s;
    seedOld = s;
  }
  else
  {
    seedNew = timer;
    seedOld = timer;
  }
}

void random()
{
  #ifndef TEST
  if (seedNew == 0)
  {
    seedNew = timer;
  }
  if (seedOld == 0)
  {
    seedOld = timer;
  }
  #else
  if (seedNew == 0)
  {
    seedNew = 1;
  }
  if (seedOld == 0)
  {
    seedOld = 1;
  }
  #endif
  int ret1 = 36969 * (seedOld & 65535) + (seedOld >> 16);
  int ret2 = 18000 * (seedNew & 65535) + (seedNew >> 16);
  
  int ret = (seedOld << 16) + seedNew;
  
  seedOld = seedNew;
  seedNew = ret;
  
  ret %= RANDMAX;
  ret += RANDMIN;
  
  return ret;
}