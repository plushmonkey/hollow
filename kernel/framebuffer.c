#include <kernel/framebuffer.h>
#include <kernel/io.h>

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

// TODO: This was written for old framebuffer given by BIOS. Needs to be rewritten for UEFI

Framebuffer framebuffer;

void framebuffer_put(u8 x, u8 y, FramebufferColor fg, FramebufferColor bg, char c) {
  if (x >= framebuffer.width) return;
  if (y >= framebuffer.height) return;

  u16* mem = ((u16*)framebuffer.addr) + (y * framebuffer.pitch) + x;
  u16 val = c | ((((bg & 0x0F) << 4) | (fg & 0x0F)) << 8);

  *mem = val;
}

void framebuffer_clear() {
  for (u32 i = 0; i < framebuffer.width * framebuffer.height; ++i) {
    ((u16*)framebuffer.addr)[i] = 0;
  }
}

void framebuffer_move_cursor(u8 x, u8 y) {
  u16 val = ((u16)y * framebuffer.width) + (u16)x;

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

    if (writer->x >= framebuffer.width) {
      writer->x = 0;
      writer->y++;
    }

    if (writer->y >= framebuffer.height) {
      writer->y = framebuffer.height - 1;
    }

    ++text;
  }
}

