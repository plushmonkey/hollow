#include <kernel/framebuffer.h>
#include <kernel/serial.h>
#include <kernel/multiboot.h>
#include <kernel/idt.h>

static Idt idt;
static IdtDescriptor idt_descriptor;

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
  u8 flags = IDT_FLAG_PRESENT | IDT_FLAG_TYPE(IDT_TYPE_InterruptGate);
  for (size_t i = 0; i < HOLLOW_ARRAY_COUNT(idt.entries); ++i) {
    idt.entries[i] = idt_entry_create((uintptr_t)&interrupt_vector[i], 0, flags);
  }
  
  idt_descriptor.limit = HOLLOW_ARRAY_COUNT(idt.entries) - 1;
  idt_descriptor.base = (u64)&idt;
  
  idt_load(&idt_descriptor);
  
  parse_mbi(mbi);
  
  // Force page-fault once page tables are implemented
  //*((u32*)framebuffer.addr) = 0;
  
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

void interrupt_dispatch(u64 rsp) {
  Frame* frame = (Frame*)rsp;
  
  u64 interrupt_number = frame->interrupt_number;
  
  switch (interrupt_number) {
    case INTERRUPT_TYPE_PageFault: { // Page fault
      serial_write(SERIAL_COM1, "TODO: Implementing paging\n");
      return;
    } break;
    default: {
      serial_write(SERIAL_COM1, "Unhandled Interrupt: ");
      serial_write_u64(SERIAL_COM1, interrupt_number);
    } break;
  }
}
