#include <kernel/framebuffer.h>
#include <kernel/serial.h>
#include <kernel/multiboot.h>
#include <kernel/idt.h>

extern void halt();

extern char _boot_end;

static Idt idt;
static IdtDescriptor idt_descriptor;

#define MMU_PRESENT (1 << 0)
#define MMU_WRITABLE (1 << 1)
#define MMU_USER_MEMORY (1 << 2)
#define MMU_PDE_TWO_MB (1 << 7)
// Mask off the flag bits to get the 0x1000 aligned address
#define MMU_ADDRESS_MASK 0xFFFFF000
#define MMU_TABLE_ALIGN __attribute__((aligned(0x1000)))

extern u64 pml4[512];
static u64 framebuffer_pdt[512] MMU_TABLE_ALIGN;

#define VIRTUAL_TO_PHYSICAL(addr) (((addr) & 0x7FFFFFFF) + ((u64)&_boot_end - 0x400000))

// Very simple 1-1 mapping
void vmm_map_framebuffer() {
  u64 framebuffer_size = framebuffer.pitch * framebuffer.height;
  uintptr_t addr_start = (uintptr_t)framebuffer.addr;
  uintptr_t addr_end = addr_start + framebuffer_size;
  
  size_t page_count = ((addr_end - addr_start) + (1 << 21) - 1) / (1 << 21);
  
  serial_write(SERIAL_COM1, "Framebuffer map start: ");
  serial_write_addr(SERIAL_COM1, (void*)addr_start);
  serial_write(SERIAL_COM1, "Framebuffer map end: ");
  serial_write_addr(SERIAL_COM1, (void*)addr_end);
  
  for (size_t i = 0; i < page_count; ++i) {
    uintptr_t addr = addr_start + i * (1 << 21);
    
    uintptr_t* pdpt = (uintptr_t*)(pml4[(addr >> 39) & 0x1FF] & MMU_ADDRESS_MASK);
    if (!pdpt) {
      serial_write(SERIAL_COM1, "Failed to map framebuffer\n");
      return;
    }
    
    uintptr_t* directory = (uintptr_t*)(pdpt[(addr >> 30) & 0x1FF] & MMU_ADDRESS_MASK);
    if (!directory) {
      pdpt[(addr >> 30) & 0x1FF] = VIRTUAL_TO_PHYSICAL((u64)framebuffer_pdt) | MMU_PRESENT | MMU_WRITABLE;
      directory = (uintptr_t*)(pdpt[(addr >> 30) & 0x1FF] & MMU_ADDRESS_MASK);
      
      u64 top_addr = (addr >> 21) << 21;
      
      u64 page_size = (1 << 21);
      for (size_t j = 0; j < 512; ++j) {
        u64 mapping = (top_addr + (page_size * j)) | MMU_PRESENT | MMU_WRITABLE | MMU_PDE_TWO_MB;
        directory[j] = mapping;
      }
    }
  }
}

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
      
      serial_write(SERIAL_COM1, "Framebuffer width: ");
      serial_write_u32(SERIAL_COM1, framebuffer.width);
      
      serial_write(SERIAL_COM1, "Framebuffer height: ");
      serial_write_u32(SERIAL_COM1, framebuffer.height);
      
      serial_write(SERIAL_COM1, "Framebuffer pitch: ");
      serial_write_u32(SERIAL_COM1, framebuffer.pitch);
      
      serial_write(SERIAL_COM1, "Framebuffer bpp: ");
      serial_write_u32(SERIAL_COM1, framebuffer.bpp);
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
  
  vmm_map_framebuffer();
  
  framebuffer_clear(0x111111);
  framebuffer_draw_line(0, 0, framebuffer.width, framebuffer.height, 0x888888);
  framebuffer_draw_line(framebuffer.width, 0, 0, framebuffer.height, 0x888888);
  
  serial_write(SERIAL_COM1, "kmain returning\n");
}

void interrupt_dispatch(u64 rsp) {
  Frame* frame = (Frame*)rsp;
  
  u64 interrupt_number = frame->interrupt_number;
  
  switch (interrupt_number) {
    case INTERRUPT_TYPE_PageFault: {
      serial_write(SERIAL_COM1, "Interrupt: Page not loaded\n");
    } break;
    default: {
      serial_write(SERIAL_COM1, "Unhandled Interrupt: ");
      serial_write_u64(SERIAL_COM1, interrupt_number);
    } break;
  }
  
  halt();
}
