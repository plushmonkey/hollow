#include <kernel/framebuffer.h>
#include <kernel/serial.h>

void kmain() {
  framebuffer_clear();

  FramebufferWriter writer = framebuffer_writer_create(FRAMEBUFFER_COLOR_LIGHT_BLUE, FRAMEBUFFER_COLOR_BLACK);

  framebuffer_writer_write(&writer, "testing\nmultiline\n");

  writer.fg = FRAMEBUFFER_COLOR_RED;
  framebuffer_writer_write(&writer, "abc");

  framebuffer_move_cursor(3, 1);

  serial_write(SERIAL_COM1, "testing\n");
}
