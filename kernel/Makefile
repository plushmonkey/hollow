BUILD_DIR = build

CC = clang
INCLUDES = -I..
CFLAGS = -m64 -mno-sse -O2 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nodefaultlibs -fno-exceptions -fno-unwind-tables -MMD -mno-red-zone -mcmodel=kernel -fno-pie -Wall -Wextra -Werror -Wno-unused-variable -static $(INCLUDES) -c

AS = nasm
ASFLAGS = -f elf64

LD = ld
LDFLAGS = -T link.ld -melf_x86_64 -z max-page-size=0x1000 -z noexecstack

OBJS = boot.o kmain.o framebuffer.o io.o serial.o idt.o
LINK_OBJS = $(OBJS:%=$(BUILD_DIR)/%)

all: kernel

kernel: $(OBJS)
	$(LD) $(LDFLAGS) $(LINK_OBJS) -o $(BUILD_DIR)/kernel
		
%.o: %.c
	mkdir -p $(BUILD_DIR)/$(@D)
	$(CC) $(CFLAGS) $< -o $(BUILD_DIR)/$@
	
%.o: %.S
	mkdir -p $(BUILD_DIR)/$(@D)
	$(AS) $(ASFLAGS) $< -o $(BUILD_DIR)/$@

clean:
	rm -rf $(BUILD_DIR)
