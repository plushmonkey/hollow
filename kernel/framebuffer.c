#include <kernel/framebuffer.h>
#include <kernel/io.h>

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

u16* framebuffer_memory = (u16*)0xB8000;

void framebuffer_put(u8 x, u8 y, FramebufferColor fg, FramebufferColor bg, char c) {
  if (x >= FRAMEBUFFER_WIDTH) return;
  if (y >= FRAMEBUFFER_HEIGHT) return;

  u16* mem = framebuffer_memory + (y * FRAMEBUFFER_WIDTH) + x;
  u16 val = c | ((((bg & 0x0F) << 4) | (fg & 0x0F)) << 8);

  *mem = val;
}

void framebuffer_clear() {
  for (int i = 0; i < FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT; ++i) {
    framebuffer_memory[i] = 0;
  }
}

void framebuffer_move_cursor(u8 x, u8 y) {
  u16 val = ((u16)y * FRAMEBUFFER_WIDTH) + (u16)x;

  outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  outb(FB_DATA_PORT, (val >> 8) & 0xFF);

  outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  outb(FB_DATA_PORT, val & 0xFF);
}

FramebufferWriter framebuffer_writer_create(FramebufferColor fg, FramebufferColor bg) {
  FramebufferWriter writer;

  writer.x = 0;
  writer.y = 0;
  writer.fg = fg;
  writer.bg = bg;

  return writer;
}

void framebuffer_writer_write(FramebufferWriter* writer, const char* text) {
  while (*text) {
    if (*text == '\n') {
      writer->x = 0;
      writer->y++;
    } else {
      framebuffer_put(writer->x, writer->y, writer->fg, writer->bg, *text);
      writer->x++;
    }

    if (writer->x >= FRAMEBUFFER_WIDTH) {
      writer->x = 0;
      writer->y++;
    }

    if (writer->y >= FRAMEBUFFER_HEIGHT) {
      writer->y = FRAMEBUFFER_HEIGHT - 1;
    }

    ++text;
  }
}

