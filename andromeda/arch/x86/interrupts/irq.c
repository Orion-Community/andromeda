#include <interrupts.h>
#include <irq.h>
#include <PIC/PIC.h>
#include <APIC/APIC.h>
#include <types.h>
#include <io.h>

void timerTick(); // The timer function
void picEOI(int irqNo); // Reset the interrupt pin

void irqHandle(isrVal_t regs)
{
  switch (regs.funcPtr)
  {
    case 0x0:
      timerTick();
      break;
    case 0x1:
      printf("Keyboard\n");
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
    picEOI(regs.funcPtr); // Send end of interrupt signal.
  }
}
unsigned long long timer = 0;
void timerTick()
{
  timer+=1;
}

void picEOI(int irqNo) // Send end of interrupt
{
  if (irqNo >= 8)
  {
    outb(PIC2COMMAND, PICEOI);
  }
  outb(PIC1COMMAND, PICEOI);
}