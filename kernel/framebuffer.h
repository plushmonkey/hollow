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

typedef enum  {
  FRAMEBUFFER_COLOR_BLACK = 0,
  FRAMEBUFFER_COLOR_BLUE = 1,
  FRAMEBUFFER_COLOR_GREEN = 2,
  FRAMEBUFFER_COLOR_CYAN = 3,
  FRAMEBUFFER_COLOR_RED = 4,
  FRAMEBUFFER_COLOR_MAGENTA = 5,
  FRAMEBUFFER_COLOR_BROWN = 6,
  FRAMEBUFFER_COLOR_LIGHT_GRAY = 7,
  FRAMEBUFFER_COLOR_DARK_GRAY = 8,
  FRAMEBUFFER_COLOR_LIGHT_BLUE = 9,
  FRAMEBUFFER_COLOR_LIGHT_GREEN = 10,
  FRAMEBUFFER_COLOR_LIGHT_CYAN = 11,
  FRAMEBUFFER_COLOR_LIGHT_RED = 12,
  FRAMEBUFFER_COLOR_LIGHT_MAGENTA = 13,
  FRAMEBUFFER_COLOR_LIGHT_BROWN = 14,
  FRAMEBUFFER_COLOR_WHITE = 15,
} FramebufferColor;

void framebuffer_put(u8 x, u8 y, FramebufferColor fg, FramebufferColor bg, char c);
void framebuffer_clear();
void framebuffer_move_cursor(u8 x, u8 y);

typedef struct {
  u8 x;
  u8 y;
  FramebufferColor fg;
  FramebufferColor bg;
} FramebufferWriter;

FramebufferWriter framebuffer_writer_create(FramebufferColor fg, FramebufferColor bg);
void framebuffer_writer_write(FramebufferWriter* writer, const char* text);

#endif

