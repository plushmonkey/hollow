#ifndef KERNEL_IDT_H_
#define KERNEL_IDT_H_

#include <kernel/types.h>

typedef enum {
  INTERRUPT_TYPE_Divide0 = 0,
  INTERRUPT_TYPE_Debug = 1,
  INTERRUPT_TYPE_NMI = 2,
  INTERRUPT_TYPE_Breakpoint = 3,
  INTERRUPT_TYPE_Overflow = 4,
  INTERRUPT_TYPE_Bound = 5,
  INTERRUPT_TYPE_InvalidOpcode = 6,
  INTERRUPT_TYPE_DeviceNotAvailable = 7,
  INTERRUPT_TYPE_DoubleFault = 8,
  INTERRUPT_TYPE_CoprocessorSegmentOverrun = 9,
  INTERRUPT_TYPE_InvalidTSS = 10,
  INTERRUPT_TYPE_SegmentNotPresent = 11,
  INTERRUPT_TYPE_StackFault = 12,
  INTERRUPT_TYPE_GeneralProtection = 13,
  INTERRUPT_TYPE_PageFault = 14,
  INTERRUPT_TYPE_x87 = 16,
  INTERRUPT_TYPE_AlignmentCheck = 17,
  INTERRUPT_TYPE_MachineCheck = 18,
  INTERRUPT_TYPE_SIMD = 19,
  INTERRUPT_TYPE_Virtualization = 20,
  INTERRUPT_TYPE_ControlProtection = 21,
} InterruptType;

typedef enum { 
  // Local descriptor table
  IDT_TYPE_LDT = 2,
  // Task-state segment
  IDT_TYPE_TSSAvailable = 9,
  // Task-state segment
  IDT_TYPE_TSSBusy = 11,
  IDT_TYPE_CallGate = 12,
  IDT_TYPE_InterruptGate = 14,
  IDT_TYPE_TrapGate = 15,
} IdtType;

#define IDT_FLAG_PRESENT (1 << 7)
// Descriptor privilege level
#define IDT_FLAG_DPL(dpl) ((dpl & 0x03) << 5)
#define IDT_FLAG_TYPE(type) (type & 0x0F)

#pragma pack(push, 1)
// Intel Volume 3: 6.14.1 64-bit mode IDT
typedef struct {
  u16 offset_low;
  u16 segment_selector;
  u8 ist;
  u8 flags;
  u16 offset_mid;
  u32 offset_high;
  u32 reserved;
} IdtEntry;

typedef struct {
  IdtEntry entries[256];
} Idt;

typedef struct {
  u16 limit;
  u64 base;
} IdtDescriptor;

typedef struct {
  u64 r15;
  u64 r14;
  u64 r13;
  u64 r12;
  u64 r11;
  u64 r10;
  u64 r9;
  u64 r8;
  u64 rbp;
  u64 rdi;
  u64 rsi;
  u64 rdx;
  u64 rcx;
  u64 rbx;
  u64 rax;
  
  u64 interrupt_number;
  u64 error_code;
} Frame;
#pragma pack(pop)

inline IdtEntry idt_entry_create(uintptr_t handler, u8 ist, u8 flags) {
  IdtEntry result = {};
  
  result.offset_low = handler & 0xFFFF;
  result.segment_selector = 0x08; // Index 1 of gdt
  result.ist = ist;
  result.flags = flags;
  result.offset_mid = (handler >> 16) & 0xFFFF;
  result.offset_high = (handler >> 32) & 0xFFFFFFFF;
  result.reserved = 0;
  
  return result;
}

extern void idt_load(IdtDescriptor* addr);
extern uintptr_t* interrupt_vector;

#endif
