# HollowOS
Toy x64 operating system for learning.  
Attempts to take a modern approach by making it 64-bit and booting from UEFI with GRUB multiboot2.  
Uses GRUB multiboot2 to create a framebuffer and then pages it in so graphics can be displayed.  

## Requirements
Compilation and ISO creation requires Linux/WSL.  
Emulation can be done on any system that has qemu.  

### Compilation
- build-essential
- nasm
- clang
  
### ISO creation
- mtools
- grub-common
- grub-efi
- xorriso
  
### Emulator
- qemu
