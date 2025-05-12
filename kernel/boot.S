bits 32
global _start

extern kmain
extern _kernel_physical_end
extern framebuffer_memory

KERNEL_STACK_SIZE equ 4096
KERNEL_PHYSICAL_START equ 0x0000000000400000
KERNEL_VIRTUAL_START  equ 0xFFFFFFFF80400000

%define CONTROL_REGISTER0_PROTECTED_MODE_ENABLED (1 << 0)
%define CONTROL_REGISTER0_EXTENSION_TYPE (1 << 4)
%define CONTROL_REGISTER0_PAGING (1 << 31)

; Physical address extension
%define CONTROL_REGISTER4_PAE (1 << 5)

%define KERNEL_GDT_ENTRY 1
%define KERNEL_CR4 CONTROL_REGISTER4_PAE
%define KERNEL_CR0                                                             \
  (                                                                            \
    CONTROL_REGISTER0_PROTECTED_MODE_ENABLED |                                 \
    CONTROL_REGISTER0_EXTENSION_TYPE |                                         \
    CONTROL_REGISTER0_PAGING                                                   \
  )

%define GDT_FLAG_64BIT_MODE (1 << 1)
%define GDT_ACCESS_PRESENT (1 << 7)
%define GDT_ACCESS_PRIVILEGE_RING0 0
%define GDT_ACCESS_EXECUTABLE (1 << 3)

%define GDT_ENTRY_SIZE 8
%define DECLARE_GDT_ENTRY(base, limit, flags, access)                          \
  (                                                                            \
    (((((base)) >> 24) & 0xFF) << 56) |                                        \
    ((((flags)) & 0xF) << 52) |                                                \
    (((((limit)) >> 16) & 0xF) << 48) |                                        \
    (((((access) | (1 << 4))) & 0xFF) << 40) |                                 \
    ((((base)) & 0xFFF) << 16) |                                               \
    (((limit)) & 0xFFFF)                                                       \
  )
%define GDT_KERNEL_ENTRY DECLARE_GDT_ENTRY(0, 0, GDT_FLAG_64BIT_MODE, GDT_ACCESS_PRESENT | GDT_ACCESS_PRIVILEGE_RING0 | GDT_ACCESS_EXECUTABLE)

%define PML4_SIZE 0x1000
%define PML4_ALIGNMENT 0x1000
%define PML4_ENTRY_SIZE 8
%define PML4_ADDR_TO_ENTRY_INDEX(addr) (((addr) >> 39) & 0x1FF)

%define PDPT_SIZE 0x1000
%define PDPT_ALIGNMENT 0x1000
%define PDPT_ENTRY_SIZE 8
%define PDPT_ADDR_TO_ENTRY_INDEX(addr) (((addr) >> 30) & 0x1FF)

%define PAGE_DIRECTORY_SIZE 0x1000
%define PAGE_DIRECTORY_ALIGNMENT 0x1000
%define PAGE_DIRECTORY_ENTRY_SIZE 8

%define PAGE_TABLE_SIZE 0x1000
%define PAGE_TABLE_ALIGNMENT 0x1000
%define PAGE_TABLE_ENTRY_SIZE 8

%define MMU_PRESENT (1 << 0)
%define MMU_WRITABLE (1 << 1)
%define MMU_USER_MEMORY (1 << 2)
%define MMU_PDE_TWO_MB (1 << 7)

; Extended Feature Enable Register
%define MSR_EFER 0xC0000080
; Long mode
%define MSR_EFER_LME (1 << 8)

%define TWO_MEGABYTES_SHIFT 21
%define TWO_MEGABYTES (1 << TWO_MEGABYTES_SHIFT)

section .multiboot
align 4

MULTIBOOT2_MAGIC_NUMBER equ 0xe85250d6
MULTIBOOT2_HEADER_LENGTH equ header_end - header_start
MULTIBOOT2_CHECKSUM equ -(MULTIBOOT2_MAGIC_NUMBER + MULTIBOOT2_HEADER_LENGTH)

header_start:
  dd MULTIBOOT2_MAGIC_NUMBER
  dd 0 ; architecture i386
  dd MULTIBOOT2_HEADER_LENGTH
  dd MULTIBOOT2_CHECKSUM
  
  ; Flags tag that indicates support of text console
  align 8
  dw 4
  dw 0
  dd 12
  dd 1
  
  ;  Framebuffer tag
  align 8
  dw 5
  dw 0
  dd 20
  dd 1360
  dd 768
  dd 32
  
  align 8
  dw 0 ; MULTIBOOT_HEADER_TAG_END - This must be the last tag specified
  dw 0 ; flags
  dd 8 ; size of tag including header
header_end:

section .data
align 0x1000
gdt_table:
  dq 0 ; null entry
  dq GDT_KERNEL_ENTRY
  times (0x800 - 0x10) db 0
gdt_table_end:
gdt_ptr:
  dw ((gdt_table_end - gdt_table) / 8) - 1 ; count - 1
  dd gdt_table ; ptr to table

section .bss
global pml4
align PML4_ALIGNMENT
pml4:
  resb PML4_SIZE

align PDPT_ALIGNMENT
low_pdpt:
  resb PDPT_SIZE

align PDPT_ALIGNMENT
high_pdpt:
  resb PDPT_SIZE

align PAGE_DIRECTORY_ALIGNMENT
low_page_directory_table:
  resb PAGE_DIRECTORY_SIZE

align PAGE_DIRECTORY_ALIGNMENT
high_page_directory_table:
  resb PAGE_DIRECTORY_SIZE

align 4
kernel_stack:
  resb KERNEL_STACK_SIZE
align 4
mbi:
  resb 4

section .text
align 4
_start:
  mov [mbi], ebx
  mov esp, kernel_stack + KERNEL_STACK_SIZE

  ; setup the page tables
  mov eax, low_pdpt
  or eax, (MMU_PRESENT | MMU_WRITABLE)
  mov [pml4 + (PML4_ADDR_TO_ENTRY_INDEX(KERNEL_PHYSICAL_START) * PML4_ENTRY_SIZE)], eax

  mov eax, high_pdpt
  or eax, (MMU_PRESENT | MMU_WRITABLE)
  mov [pml4 + (PML4_ADDR_TO_ENTRY_INDEX(KERNEL_VIRTUAL_START) * PML4_ENTRY_SIZE)], eax

  mov eax, low_page_directory_table
  or eax, (MMU_PRESENT | MMU_WRITABLE)
  mov [low_pdpt + (PDPT_ADDR_TO_ENTRY_INDEX(KERNEL_PHYSICAL_START) * PDPT_ENTRY_SIZE)], eax

  mov eax, high_page_directory_table
  or eax, (MMU_PRESENT | MMU_WRITABLE)
  mov [high_pdpt + (PDPT_ADDR_TO_ENTRY_INDEX(KERNEL_VIRTUAL_START) * PDPT_ENTRY_SIZE)], eax

  ; Setup 2MB directory table
  mov ecx, 0
  mov esi, _kernel_physical_end
  shr esi, TWO_MEGABYTES_SHIFT
  add esi, 1

page_directory_table_loop:
  mov eax, TWO_MEGABYTES
  mul ecx
  or eax, (MMU_PRESENT | MMU_WRITABLE | MMU_PDE_TWO_MB)
  mov [low_page_directory_table + ecx * PAGE_DIRECTORY_ENTRY_SIZE], eax
  mov [high_page_directory_table + ecx * PAGE_DIRECTORY_ENTRY_SIZE], eax
  inc ecx
  cmp ecx, esi
  jne page_directory_table_loop

  ; Setup cpu for long mode
  mov eax, pml4
  mov cr3, eax

  mov eax, KERNEL_CR4
  mov cr4, eax

  ; Enable long mode using the MSR
  mov ecx, MSR_EFER
  rdmsr
  or eax, MSR_EFER_LME
  wrmsr

  mov eax, KERNEL_CR0
  mov cr0, eax

  lgdt [gdt_ptr]
  
  ; Long jump to set cs to 0x08 (index 1 of gdt)
  jmp 0x08:_start64

bits 64
align 4
_start64:
  mov ax, 0
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  
  mov rdi, [mbi]
  
  call kmain

.loop:
  jmp .loop
  
global halt
align 4
halt:
  cli
  hlt
