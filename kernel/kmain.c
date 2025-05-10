#include <kernel/framebuffer.h>
#include <kernel/serial.h>
#include <kernel/multiboot.h>

void parse_mbi(MultibootInformation* mbi) {
  u32 consumed = sizeof(*mbi);
  
  while (consumed < mbi->total_size) {
    MultibootTag* tag = (MultibootTag*)((void*)mbi + consumed);
    
    if (tag->type == MULTIBOOT_TAG_FramebufferInfo) {
      MultibootFramebufferTag* framebuffer_tag = (MultibootFramebufferTag*)tag;
      
      framebuffer.addr = framebuffer_tag->addr;
      framebuffer.pitch = framebuffer_tag->pitch;
      framebuffer.width = framebuffer_tag->width;
      framebuffer.height = framebuffer_tag->height;
      framebuffer.bpp = framebuffer_tag->bpp;
      framebuffer.type = framebuffer_tag->frame_type;
      
      serial_write(SERIAL_COM1, "Framebuffer initialized. Type: ");
      serial_write_u32(SERIAL_COM1, framebuffer.type);
      
      serial_write(SERIAL_COM1, "Framebuffer addr: ");
      serial_write_addr(SERIAL_COM1, framebuffer.addr);
    }
    
    consumed += (tag->size + 7) & ~7;
  }
}

void kmain(MultibootInformation* mbi) {
  parse_mbi(mbi);
  
#if 0
  framebuffer_clear();
  FramebufferWriter writer = framebuffer_writer_create(FRAMEBUFFER_COLOR_LIGHT_BLUE, FRAMEBUFFER_COLOR_BLACK);

  framebuffer_writer_write(&writer, "testing\nmultiline\n");

  writer.fg = FRAMEBUFFER_COLOR_RED;
  framebuffer_writer_write(&writer, "abc");

  framebuffer_move_cursor(3, 1);
#endif

  serial_write(SERIAL_COM1, "kmain returning\n");
}
