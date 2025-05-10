#ifndef KERNEL_IO_H_
#define KERNEL_IO_H_

#include <kernel/types.h>

void outb(u16 port, u8 data);
u8 inb(u16 port);

#endif

