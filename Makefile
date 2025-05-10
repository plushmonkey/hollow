ISO_FILE=os.iso

.PHONY: all
.PHONY: clean
.PHONY: kernel
.PHONY: run
.PHONE: workspace

all: kernel $(ISO_FILE)

kernel:
	make -C kernel all

# Extra debug output
#-D ./log.txt 
#-d cpu_reset,int,guest_errors,mmu,unimp,plugin,strace,page
run:
	qemu-system-x86_64                               \
		-accel tcg,thread=single                       \
		-m 128                                         \
		-no-reboot                                     \
		-drive format=raw,media=cdrom,file=os.iso      \
		-serial stdio                                  \
		-smp 1                                         \
		-usb                                           \
		-vga std                                       \
		-L bios --bios bios/ovmf.fd -net none

$(ISO_FILE): kernel
	mkdir -p iso/boot/grub
	cp grub.cfg iso/boot/grub/
	cp kernel/build/kernel iso/boot/kernel
	grub-mkrescue -o $(ISO_FILE) iso	

# Used for generating compile_commands for clangd lsp
workspace:
	compiledb -n make
	
clean:
	make -C kernel clean
	rm -rf iso
	rm -rf $(ISO_FILE)
