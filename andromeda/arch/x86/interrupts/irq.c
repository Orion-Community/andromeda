#include <interrupts.h>
#include <irq.h>
#include <types.h>

void timerTick();
void picIntA(int irqNo);

void irqHandle(isrVal_t regs)
{
  switch (regs.funcPtr)
  {
    case 0x0:
      timerTick();
      break;
    case 0x1:
      break;
    case 0x2:
      break;
    case 0x3:
      break;
    case 0x4:
      break;
    case 0x5:
      break;
    case 0x6:
      break;
    case 0x7:
      break;
    case 0x8:
      break;
    case 0x9:
      break;
    case 0xA:
      break;
    case 0xB:
      break;
    case 0xC:
      break;
    case 0xD:
      break;
    case 0xE:
      break;
    case 0xF:
      break;
    default:
      break;
  }
  if (pic == PIC)
  {
    picIntA(regs.funcPtr);
  }
}
unsigned long long timer = 0;
void timerTick()
{
  timer+=1;
}

void picIntA(int irqNo)
{
  if (irqNo >= 8)
  {
    // Send irq acknowledge to slave PIC.
  }
  // Send irq acknowledge to master PIC.
}