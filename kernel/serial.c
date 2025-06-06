#include <kernel/serial.h>
#include <kernel/io.h>

#define SERIAL_DATA_PORT(base) (base)
#define SERIAL_FIFO_COMMAND_PORT(base) (base + 2)

// Bits for the command line
// 7 - DLAB config
// 6 - Break control config
// 5, 4, 3 - parity bit count
// 2 - Stop bit count + 1
// 1, 0 - Data length
#define SERIAL_LINE_COMMAND_PORT(base) (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base) (base + 5)

// Set the highest bit of the command line to swap byte order
#define SERIAL_LINE_ENABLE_DLAB 0x80

void serial_configure_baud_rate(u16 com, u16 divisor) {
  outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
  outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0xFF);
  outb(SERIAL_DATA_PORT(com), divisor & 0xFF);
}

// Setup the line to be 8 bits
void serial_configure_line(u16 com) {
  // This is configured by the 2 bottom bits being set:
  // 0, 0 = 5 bit line
  // 0, 1 = 6 bit line
  // 1, 0 = 7 bit line
  // 1, 1 = 8 bit line
  outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

void serial_configure_fifo(u16 com) {
  // Fifo config bits:
  // 7, 6 - Fifo buffer byte size
  // 5 - 16 or 64 byte large buffers
  // 4 - reserved
  // 3 - DMA mode selection
  // 2 - clear transmission buffer
  // 1 - clear receiver buffer
  // 0 - Enabled fifo buffer 

  // C7 = Enable fifo, clear both queues, 14 byte queue size
  outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}

void serial_configure_modem(u16 com) {
  // Model config bits:
  // 7, 6 - reserved
  // 5 - Autoflow control
  // 4 - Loopback enabled
  // 3 - Auxiliary output 2
  // 2 - Auxiliary output 1
  // 1 - Ready to transmit
  // 0 - Data terminal ready

  // 3 = Ready to transmit and data terminal ready
  outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

int serial_is_transmit_fifo_empty(u16 com) {
  return (inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20) != 0;
}

void serial_push(u16 com, char data) {
  // Spinlock until we have space to put data
  while (!serial_is_transmit_fifo_empty(com)) {}

  outb(SERIAL_DATA_PORT(com), data);
}

void serial_write(u16 com, const char* text) {
  serial_configure_baud_rate(com, 2);
  serial_configure_line(com);
  serial_configure_fifo(com);
  serial_configure_modem(com);

  while (*text) {
    serial_push(com, *text);
    ++text;
  }
}

void serial_write_u32(u16 com, u32 value) {
  char output[12] = {'0', 0};
  int count = 0;
  
  u32 divisor = 1000000000;
  
  while (divisor > 0) {
    u32 digit = value / divisor;
    
    value %= divisor;
    divisor /= 10;
    
    if (count > 0 || digit != 0) {
      output[count++] = '0' + digit;
    }
  }
  
  if (count == 0) ++count;
  output[count] = '\n';
  
  serial_write(com, output);
}

void serial_write_u64(u16 com, u64 value) {
  char output[24] = {'0', 0};
  int count = 0;
  
  u64 divisor = 10000000000000000000ULL;
  
  while (divisor > 0) {
    u64 digit = value / divisor;
    
    value %= divisor;
    divisor /= 10;
    
    if (count > 0 || digit != 0) {
      output[count++] = '0' + digit;
    }
  }
  
  if (count == 0) ++count;
  output[count] = '\n';
  
  serial_write(com, output);
}

void serial_write_addr(u16 com, void* addr) {
  const char* digits = "0123456789abcdef";
  char output[24] = {'0', 'x', '0', 0};
  int count = 2;
  
  u32 shift = 0;
  u64 value = (u64)addr;
  
  while (shift < 64) {
    u8 digit = (value >> (60 - shift)) & 0x0F;
    
    char c = digits[digit];      
    output[count++] = c;
    
    shift += 4;
  }
  
  if (count == 2) ++count;
  output[count] = '\n';
  output[count + 1] = 0;
  
  serial_write(com, output);
}
