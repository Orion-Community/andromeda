/*
 *   PIC header
 *   Copyright (C) 2011  Michel Megens
 *   Copyright (C) 2015  Bart Kuivenhoven
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>

#ifndef __H_PIC
#define __H_PIC

#ifdef __cplusplus
extern "C" {
#endif

void pic_8259_remap(uint32_t set1, uint32_t set2);
void pic_8259_eoi(uint8_t irq);
void pic_8259_init();

#define X86_8259_INTERRUPT_BASE 0x20

#define X86_8259_PIC1_COMMAND 0x20
#define X86_8259_PIC2_COMMAND 0xa0

#define X86_8259_PIC1_DATA 0x21
#define X86_8259_PIC2_DATA 0xA1

// A few pic commands

#define X86_8259_PIC_EOI 0x20         /* end of interrupt */
#define X86_8259_ICW1_ICW4 0x01       /* ICW4 (not) needed */
#define X86_8259_ICW1_SINGLE 0x02     /* Single (cascade) mode */
#define X86_8259_ICW1_INTERVAL4 0x04  /* Call address interval 4 (8) */
#define X86_8259_ICW1_LEVEL 0x08      /* Level triggered (edge) mode */
#define X86_8259_ICW1_INIT 0x10       /* Initialization - required! */

#define X86_8259_ICW4_8086 0x01       /* 8086/88 (MCS-80/85) mode */
#define X86_8259_ICW4_AUTO 0x02       /* Auto (normal) EOI */
#define X86_8259_ICW4_BUF_SLAVE 0x08  /* Buffered mode/slave */
#define X86_8259_ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define X86_8259_ICW4_SFNM 0x10       /* Special fully nested (not) */
#define X86_8259_ICW3_MASTER 0x4
#define X86_8259_ICW3_SLAVE 0x2

#define X86_8259_PIC_DISABLE    0xFF
#define X86_8259_PIC_READ_ISR   0x0B

/*
 * This will initiate the pic. Panics on failure.
 */
void pic_8259_init();

/**
 * \fn pic_clear_irq_mask(uint8_t irq)
 * \brief Enables an irq.
 * \param irq The irq to enable.
 * 
 * Clears the bit mask in the PIC to enable the irq.
 */
int pic_8259_clear_irq_mask(uint8_t irq);
int pic_8259_set_irq_mask(uint8_t irq);

/**
 *  \fn pic_9259_disable
 *  \brief Disables the 8259 PIC, use only when switching to APIC
 */
void pic_8259_disable();
int pic_8259_detect_spurious(uint8_t irq);

#ifdef __cplusplus
}
#endif

#endif
