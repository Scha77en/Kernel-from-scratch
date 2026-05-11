# KFS-1 Kernel Build Environment
# Multi-stage Docker build for kernel compilation and testing

FROM ubuntu:22.04 as builder

# Prevent interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc-multilib \
    g++-multilib \
    nasm \
    binutils \
    make \
    grub-pc-bin \
    xorriso \
    parted \
    e2fsprogs \
    dosfstools \
    git \
    curl \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace/kfs-1

# Copy project files
COPY . .

# Build kernel
RUN make clean && make all

# Final stage - runtime environment
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    qemu-system-i386 \
    qemu-system-x86 \
    grub-pc-bin \
    parted \
    e2fsprogs \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# Create workspace
WORKDIR /workspace/kfs-1

# Copy built kernel from builder stage
COPY --from=builder /workspace/kfs-1/kernel.elf .
COPY --from=builder /workspace/kfs-1/kernel.img .
COPY --from=builder /workspace/kfs-1 .

# Set up mount point for disk image
RUN mkdir -p /mnt/kfs-mount /tmp/kfs_mount

# Default command - run kernel with QEMU
CMD ["qemu-system-i386", "-drive", "file=kernel.img,format=raw", "-m", "256M", "-nographic", "serial", "stdio"]

