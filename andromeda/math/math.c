#include <math/math.h>
#include <mm/memory.h>

int seedNew = 1; // Store the seeds
int seedOld = 1;

void randomise(int s) // Create the seed
{
  if (s!=0)
  {
    seedNew = s;
    seedOld = s/2;
    if (seedOld == 0)
    {
      seedOld = 0x0BADB002;
    }
  }
  else
  {
    seedNew = timer;
    seedOld = timer;
  }
}

void random() // Hash the stuff and make it random.
{
  // Doesn't need to be thread safe for extra randomness.
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
  
  seedOld = seedNew; // Set the seeding values
  seedNew = ret;
  
  ret %= RANDMAX;
  ret += RANDMIN;
  
  return ret;
}