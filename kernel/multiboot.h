#ifndef _KERNEL_MULTIBOOT_H_
#define _KERNEL_MULTIBOOT_H_

#include <kernel/types.h>

typedef struct {
  u32 total_size;
  u32 reserved;
} MultibootInformation;

typedef enum {
  MULTIBOOT_TAG_CommandLine = 1,
  MULTIBOOT_TAG_BootLoaderName = 2,
  MULTIBOOT_TAG_Modules = 3,
  MULTIBOOT_TAG_MemoryInformation = 4,
  MULTIBOOT_TAG_BIOSBootDevice = 5,
  MULTIBOOT_TAG_MemoryMap = 6,
  MULTIBOOT_TAG_VBEInfo = 7,
  MULTIBOOT_TAG_FramebufferInfo = 8,
  MULTIBOOT_TAG_ELFSymbols = 9,
  MULTIBOOT_TAG_APMTable = 10,
  MULTIBOOT_TAG_EFISystemTable32 = 11,
  MULTIBOOT_TAG_EFISystemTable64 = 12,
  MULTIBOOT_TAG_SMBIOS = 13,
  MULTIBOOT_TAG_ACPI1 = 14,
  MULTIBOOT_TAG_ACPI2 = 15,
  MULTIBOOT_TAG_Networking = 16,
  MULTIBOOT_TAG_EFIMemory = 17,
  MULTIBOOT_TAG_EFIBootServices = 18,
  MULTIBOOT_TAG_EFIImageHandle32 = 19,
  MULTIBOOT_TAG_EFIImageHandle64 = 20,
  MULTIBOOT_TAG_ImageLoadBase = 21,
} MultibootTagType;

typedef struct {
  u32 type;
  u32 size;
} MultibootTag;

typedef enum {
  FRAMEBUFFER_TYPE_INDEXED,
  FRAMEBUFFER_TYPE_RGB,
  FRAMEBUFFER_TYPE_EGA_TEXT
} FramebufferType;

#pragma pack(push, 1)
typedef struct 
{
  u32 type;
  u32 size;

  void* addr;
  u32 pitch;
  u32 width;
  u32 height;
  u8 bpp;
  u8 frame_type; // FramebufferType
} MultibootFramebufferTag;
#pragma pack(pop)

#endif
