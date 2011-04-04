#ifndef __IO_H_
#define __IO_H_

extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char msg);

#endif