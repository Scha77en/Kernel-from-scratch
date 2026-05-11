#!/bin/sh

echo "building docker container..."

./docker-build.sh build

echo "starting the kernel..."

./docker-build.sh run

qemu-system-i386 -drive file=kernel.img,format=raw -m 256M

echo "Kernel Is Ready :D"
