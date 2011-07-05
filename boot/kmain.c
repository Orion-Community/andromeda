#include <text.h>

unsigned char stack[0x10000];

int core()
{
  // In the future this will do a little more
  for (;;) // Infinite loop, to make the kernel schedule when there is nothing to do
  {
    printf("You can now shutdown your PC\n");
    halt();
  }
}