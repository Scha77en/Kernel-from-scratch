#!/bin/bash

set -e  # Exit on error

LOOP_DEVICE="/dev/loop0"
MOUNT_POINT="/tmp/kfs_mount"
IMAGE_FILE="kernel.img"
IMAGE_SIZE=50  # MB
KERNEL_ELF="kernel.elf"

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

cleanup() {
    log_info "Cleaning up..."
    
    # Unmount if mounted
    if mount | grep -q "$MOUNT_POINT"; then
        log_info "Unmounting $MOUNT_POINT..."
        sudo umount "$MOUNT_POINT" 2>/dev/null || true
    fi
    
    # Detach loop device
    if sudo losetup -l | grep -q "$IMAGE_FILE"; then
        log_info "Detaching $LOOP_DEVICE..."
        sudo losetup -d "$LOOP_DEVICE" 2>/dev/null || true
    fi
    
    log_info "Cleanup complete"
}

# Trap to cleanup on script exit
trap cleanup EXIT

setup_image() {
    log_info "Creating ${IMAGE_SIZE}MB disk image..."
    dd if=/dev/zero of="$IMAGE_FILE" bs=1M count="$IMAGE_SIZE"
    
    log_info "Attaching to $LOOP_DEVICE..."
    sudo losetup "$LOOP_DEVICE" "$IMAGE_FILE"
    
    log_info "Creating partition table..."
    sudo parted "$LOOP_DEVICE" mklabel msdos
    
    log_info "Creating partition (1MiB to $((IMAGE_SIZE - 1))MiB)..."
    PARTITION_END=$((IMAGE_SIZE - 1))
    sudo parted "$LOOP_DEVICE" mkpart primary ext2 1MiB "${PARTITION_END}MiB"
    
    log_info "Formatting filesystem with ext2..."
    sudo mkfs.ext2 "${LOOP_DEVICE}p1"
    
    log_info "Creating mount point..."
    sudo mkdir -p "$MOUNT_POINT"
    
    log_info "Mounting partition..."
    sudo mount "${LOOP_DEVICE}p1" "$MOUNT_POINT"
    
    log_info "Verifying mount..."
    df -h "$MOUNT_POINT"
}

install_grub() {
    log_info "Installing GRUB..."
    sudo grub-install \
        --target=i386-pc \
        --root-directory="$MOUNT_POINT" \
        --no-floppy \
        --force \
        --install-modules="biosdisk part_msdos ext2 normal" \
        "$LOOP_DEVICE"
    
    log_info "Verifying GRUB installation..."
    ls -la "$MOUNT_POINT/boot/grub/"
}

create_grub_config() {
    log_info "Creating grub.cfg..."
    sudo tee "$MOUNT_POINT/boot/grub/grub.cfg" > /dev/null << 'EOF'
set default=0
set timeout=0
insmod part_msdos
insmod ext2
insmod multiboot

menuentry "kfs-1" {
    multiboot /kernel.elf
    boot
}
EOF
    
    log_info "grub.cfg content:"
    sudo cat "$MOUNT_POINT/boot/grub/grub.cfg"
}

copy_kernel() {
    if [ ! -f "$KERNEL_ELF" ]; then
        log_error "$KERNEL_ELF not found. Build the kernel first with: make clean && make all"
        return 1
    fi
    
    log_info "Stripping kernel symbols..."
    strip -s "$KERNEL_ELF" 2>/dev/null || log_info "Note: strip may not remove all symbols"
    
    log_info "Copying kernel to mount point..."
    sudo cp "$KERNEL_ELF" "$MOUNT_POINT/"
    
    log_info "Verifying kernel copied..."
    ls -lah "$MOUNT_POINT/$KERNEL_ELF"
}

check_image_size() {
    local size=$(du -h "$IMAGE_FILE" | cut -f1)
    log_info "Final image size: $size"
    
    if (( $(echo "$size" | sed 's/M//') > IMAGE_SIZE )); then
        log_error "Image exceeds ${IMAGE_SIZE}MB limit!"
        return 1
    fi
}

test_qemu() {
    log_info "Image setup complete. Ready to boot with QEMU."
    log_info "Run: qemu-system-i386 -drive file=$IMAGE_FILE,format=raw -m 512M"
    
    read -p "Do you want to launch QEMU now? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        log_info "Launching QEMU..."
        qemu-system-i386 -drive file="$IMAGE_FILE",format=raw -m 512M
    fi
}

main() {
    log_info "Starting kernel image setup..."
    
    # Setup image
    setup_image
    
    # Install GRUB
    install_grub
    
    # Create config
    create_grub_config
    
    # Copy kernel
    copy_kernel
    
    # Verify size
    check_image_size
    
    log_info "Setup complete!"
    test_qemu
}

# Run main function
main "$@"
