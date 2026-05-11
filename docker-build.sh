#!/bin/bash
# Docker build and run script for KFS-1 kernel

set -e

PROJECT_NAME="kfs1-kernel"
IMAGE_NAME="kfs1-kernel:latest"
CONTAINER_NAME="kfs1-runner"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Show usage
show_usage() {
    cat << EOF
Usage: ./docker-build.sh [COMMAND]

Commands:
    build       - Build Docker image
    run         - Run kernel in QEMU (interactive)
    run-detach  - Run kernel in background
    stop        - Stop running container
    logs        - Show container logs
    shell       - Open shell in container
    clean       - Remove image and containers
    help        - Show this help message

Examples:
    ./docker-build.sh build       # Build image first time
    ./docker-build.sh run         # Run kernel interactively
    ./docker-build.sh logs        # Check output
EOF
}

# Build Docker image
build_image() {
    print_info "Building Docker image: $IMAGE_NAME"
    docker build -t $IMAGE_NAME -f Dockerfile .
    print_success "Docker image built successfully"
}

# Run kernel
run_kernel() {
    print_info "Starting KFS-1 kernel in QEMU..."
    docker run -it \
        --rm \
        --name $CONTAINER_NAME \
        -v $(pwd):/workspace/kfs-1 \
        --device /dev/kvm \
        $IMAGE_NAME
}

hostrun() {
    if [ ! -f kernel.img ]; then
        print_error "kernel.img not found in current directory. Build it first!"
        exit 1
    fi
    print_info "Running QEMU on host using kernel.img ..."
    qemu-system-i386 -drive file=kernel.img,format=raw -m 256M
}

# Run kernel in background
run_detach() {
    print_info "Starting KFS-1 kernel in background..."
    docker run -d \
        --name $CONTAINER_NAME \
        -v $(pwd):/workspace/kfs-1 \
        --device /dev/kvm \
        $IMAGE_NAME
    print_success "Kernel running in background"
    docker logs -f $CONTAINER_NAME
}

# Stop container
stop_container() {
    print_info "Stopping container: $CONTAINER_NAME"
    if docker ps | grep -q $CONTAINER_NAME; then
        docker stop $CONTAINER_NAME
        print_success "Container stopped"
    else
        print_error "Container not running"
    fi
}

# Show logs
show_logs() {
    print_info "Showing logs for $CONTAINER_NAME"
    docker logs -f $CONTAINER_NAME
}

# Open shell
open_shell() {
    print_info "Opening shell in $CONTAINER_NAME"
    docker run -it \
        --rm \
        -v $(pwd):/workspace/kfs-1 \
        $IMAGE_NAME /bin/bash
}

# Clean up
cleanup() {
    print_info "Cleaning up Docker resources..."
    docker stop $CONTAINER_NAME 2>/dev/null || true
    docker rm $CONTAINER_NAME 2>/dev/null || true
    docker rmi $IMAGE_NAME 2>/dev/null || true
    print_success "Cleanup complete"
}

# Main script
case "${1:-help}" in
    build)
        build_image
        ;;
    run)
        run_kernel
        ;;
    hostrun)
        hostrun
        ;;
    run-detach)
        run_detach
        ;;
    stop)
        stop_container
        ;;
    logs)
        show_logs
        ;;
    shell)
        open_shell
        ;;
    clean)
        cleanup
        ;;
    help|*)
        show_usage
        ;;
esac

