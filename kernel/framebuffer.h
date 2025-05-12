#ifndef KERNEL_FRAMEBUFFER_H_
#define KERNEL_FRAMEBUFFER_H_

#include <kernel/types.h>

#pragma pack(push, 1)
typedef struct {
  void* addr;
  u32 pitch;
  u32 width;
  u32 height;
  u8 bpp;
  u8 type;
  u8 reserved;
} Framebuffer;
#pragma pack(pop)

extern Framebuffer framebuffer;

void framebuffer_clear(u32 color);
void framebuffer_setpixel(u32 x, u32 y, u32 color);
void framebuffer_draw_line(u32 x0, u32 y0, u32 x1, u32 y1, u32 color);

#endif
