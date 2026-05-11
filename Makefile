# === Directories ===
SRC_DIRS = kernel drivers
OBJ_DIR = build
HEADERS_DIR = headers

# === Compiler and Tools ===
CC = gcc -g
ASM = nasm
LD = ld
QEMU = qemu-system-i386

# === Source Files ===
C_SOURCES = $(shell find $(SRC_DIRS) -name "*.c")
ASM_SOURCES = multiboot_entry.asm
C_OBJECTS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS = $(patsubst %.asm,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
ALL_OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS)

# === Compilation Flags (CRITICAL FOR KERNEL) ===
CFLAGS = \
    -m32                    \
    -fno-pic                \
    -fno-pie                \
    -ffreestanding          \
    -fno-builtin            \
    -fno-stack-protector    \
    -Wall                   \
    -Wextra                 \
    -I$(HEADERS_DIR)        \
    -nostdlib               \
    -nodefaultlibs

LDFLAGS = -m elf_i386 -T linker.ld

# === Targets ===
.PHONY: all clean build run grub test

all: build kernel.elf

# Create build directory
build:
	@mkdir -p $(OBJ_DIR) $(OBJ_DIR)/kernel $(OBJ_DIR)/drivers

# Compile C files
$(OBJ_DIR)/%.o: %.c build
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM files
$(OBJ_DIR)/%.o: %.asm build
	$(ASM) -f elf -o $@ $<

# Link everything into ELF kernel
kernel.elf: kernel/isr.o  $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^
	@echo "✓ Kernel compiled: kernel.elf"

%.o : %.asm
	nasm $< -f elf -o $@

# Copy kernel to disk image
grub: kernel.elf
	sudo losetup -P /dev/loop0 kernel.img
	sleep 1
	sudo mount /dev/loop0p1 /tmp/kfs_mount
	sudo cp kernel.elf /tmp/kfs_mount/boot/kernel.bin
	sudo umount /tmp/kfs_mount
	sudo losetup -d /dev/loop0
	@echo "✓ Kernel copied to disk image"@echo "✓ Kernel copied to disk image"

# Test with QEMU
run: grub
	$(QEMU) -drive file=kernel.img,format=raw -m 256M

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) kernel.elf
	@echo "✓ Cleaned build files"

# Debug: show variables
debug:
	@echo "C_SOURCES: $(C_SOURCES)"
	@echo "C_OBJECTS: $(C_OBJECTS)"
	@echo "ASM_SOURCES: $(ASM_SOURCES)"
	@echo "ASM_OBJECTS: $(ASM_OBJECTS)"
	@echo "ALL_OBJECTS: $(ALL_OBJECTS)"
