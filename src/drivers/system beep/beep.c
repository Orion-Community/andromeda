/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Steven van der Schoot

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

#include "Include/beep.h"

/**
 * play tone
 * 
 * @param unsigned int nFrequence:
 *   frequentie for the tone.
 * 
 */
static void play_tone(unsigned int nFrequence) {
  unsigned int Div;
  unsigned char tmp;
  
  //Set the PIT to the desired frequency
  Div = 1193180 / nFrequence;
  outb(0x43, 0xb6);
  outb(0x42, (unsigned char) (Div) );
  outb(0x42, (unsigned char) (Div >> 8));
  
  //Play the tone using the PC speaker
  tmp = inb(0x61);
  if (tmp != (tmp | 3)) {
    outb(0x61, tmp | 3);
  }
}

/**
 * Stop sound
 */
static void stop_tone() {
  unsigned char tmp = (inb(0x61) & 0xFC);
  outb(0x61, tmp);
}

/**
 * make system beep
 */
void beep() { /*   SEEMS TO NOT WORK ON QEMU, BUT DOES WORK ON REAL HARDWARE!   */
  play_tone(1000);
  //WAIT(10);
  int i = 0;
  for(;i<0x0fffffff;i++)
  {
    ;
  }
  stop_tone();
  //set_PIT_2(old_frequency);
}
