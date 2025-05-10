#ifndef KERNEL_SERIAL_H_
#define KERNEL_SERIAL_H_

#include <kernel/types.h>

#define SERIAL_COM1 0x3F8
#define SERIAL_COM2 0x2F8
#define SERIAL_COM3 0x3E8
#define SERIAL_COM4 0x2E8
#define SERIAL_COM5 0x5F8
#define SERIAL_COM6 0x4F8
#define SERIAL_COM7 0x5E8
#define SERIAL_COM8 0x4E8

void serial_write(u16 com, const char* text);
void serial_write_u32(u16 com, u32 value);
void serial_write_u64(u16 com, u64 value);
void serial_write_addr(u16 com, void* value);

#endif

