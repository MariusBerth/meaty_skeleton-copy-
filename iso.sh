#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/veldos.kernel isodir/boot/veldos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "veldos" {
	multiboot2 /boot/veldos.kernel
}
EOF
grub-mkrescue -o veldos.iso isodir
