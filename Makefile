# === Directories ===
SRC_DIRS = kernel drivers
OBJ_DIR = build
HEADERS_DIR = headers
ISO_ROOT = iso_root
GRUB_DIR = $(ISO_ROOT)/boot/grub

# === Compiler and Tools ===
KERNEL_HEADERS = headers
CC = gcc
ASM = nasm
LD = ld
QEMU = qemu-system-i386
GRUB_MKRESCUE = grub-mkrescue

# === Source Files ===
C_SOURCES = $(shell find $(SRC_DIRS) -name "*.c")
ASM_SOURCES = multiboot_entry.asm kernel/isr.asm kernel/panic_asm.asm
C_OBJECTS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS = $(patsubst %.asm,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
ALL_OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS)

# === Compilation Flags ===
CFLAGS = \
	-fno-builtin  \
	-fno-stack-protector    \
	-I$(HEADERS_DIR)       \
	-nodefaultlibs	\
	-nostdlib \
	-m32
LDFLAGS = -m elf_i386 -T linker.ld

# === Targets ===
.PHONY: all clean build iso run fclean

all: iso

build:
	@mkdir -p $(OBJ_DIR) $(OBJ_DIR)/kernel $(OBJ_DIR)/drivers
	@mkdir -p $(GRUB_DIR)

# Compile C files
$(OBJ_DIR)/%.o: %.c build
	$(CC) $(CFLAGS) -I $(KERNEL_HEADERS) -c $< -o $@

# Assemble ASM files
$(OBJ_DIR)/%.o: %.asm build
	$(ASM) -f elf32 -o $@ $<

# Link everything into ELF kernel
kernel.elf: $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^
	strip -s kernel.elf
	@echo "✓ Kernel compiled: kernel.elf"

# Build bootable ISO
iso: kernel.elf build
	@rm -rf $(ISO_ROOT)
	@mkdir -p $(GRUB_DIR)
	@cp kernel.elf $(ISO_ROOT)/boot/kernel.elf
	@cp boot/grub/grub.cfg $(GRUB_DIR)/grub.cfg
	$(GRUB_MKRESCUE) -o kfs.iso $(ISO_ROOT) --compress=xz --install-modules="multiboot"
	@echo "✓ Bootable ISO created: kfs.iso"

# Run ISO in QEMU
run: iso
	$(QEMU) -cdrom kfs.iso -m 256M

clean:
	rm -rf $(OBJ_DIR) kernel.elf $(ISO_ROOT)
	@echo "✓ Cleaned build files"

fclean: clean
	rm -rf kfs.iso
	@echo "✓ Removed iso"

