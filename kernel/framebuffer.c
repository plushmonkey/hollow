#include <kernel/framebuffer.h>

Framebuffer framebuffer;

static inline s32 framebuffer_abs(s32 v) {
  return v < 0 ? -v : v;
}

void framebuffer_clear(u32 color) {
  void* addr = framebuffer.addr;
  void* addr_end = framebuffer.addr + framebuffer.pitch * framebuffer.height;
  
  while (addr < addr_end) {
    *(u32*)addr = color;
    addr += sizeof(u32);
  }
}

void framebuffer_setpixel(u32 x, u32 y, u32 color) {
  if (x >= framebuffer.width || y >= framebuffer.height) return;
  
  u32* addr = (u32*)(framebuffer.addr + (framebuffer.pitch * y) + (x * sizeof(u32)));
  
  *addr = color;
}

void framebuffer_draw_line(u32 x0, u32 y0, u32 x1, u32 y1, u32 color) {
  s32 dx = framebuffer_abs((s32)x1 - (s32)x0);
  s32 sx = x0 < x1 ? 1 : -1;
  
  s32 dy = -framebuffer_abs((s32)y1 - (s32)y0);
  s32 sy = y0 < y1 ? 1 : -1; 
  
  s32 err = dx + dy;
  s32 e2 = 0;

  for (;;) {
    framebuffer_setpixel(x0, y0, color);
    if (x0 == x1 && y0 == y1) break;
    
    e2 = 2 * err;
    
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}
