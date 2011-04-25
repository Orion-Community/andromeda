#ifndef __PIC_H
#define __PIC_H

#define PIC1COMMAND		0x20
#define PIC1DATA		0x21

#define PIC2COMMAND		0xA0
#define PIC2DATA		0xA1

#define PICEOI			0x20

#define ICW1_ICW4		0x01 // ICW4 is needed
#define ICW1_SINGLE		0x02 // Cascade mode
#define ICW1_INTERVAL		0x04 // Call address intervall 4
#define ICW1_LEVEL		0x08 // Level triggered mode
#define ICW1_INIT		0x10 // Initialisation mode. Is required

#define ICW4_8086		0x01 // 8086 mode
#define ICW4_AUTO		0x02 // Auto EOI
#define ICW4_BUFSLAVE		0x04 // Buffered mode slave
#define ICW4_BUFMASTER		0x0C // Buffered mode master
#define ICW4_SFNM		0x10 // Special Fully Nested mode

void initPIC();

#endif