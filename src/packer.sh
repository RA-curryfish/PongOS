#!/bin/bash

GCC="i686-elf-gcc"
ASM="i686-elf-as"
PREFIX="$HOME/Academics/qemu/PongOS/opt/cross"
CFLAGS="-std=gnu99 -ffreestanding -O2 -Wall -Wextra"
which -- $ASM || PATH="$PREFIX/bin:$PATH"
CFILES=$(find . -name "*.c")
OFILES="boot.o"
ASMFILES=$(find . -name "*.s")

# creating boot assembled object
$ASM $ASMFILES -o boot.o

# creating kernel compiled object
for file in $CFILES
do
    ofile="${file%.*}.o"
    OFILES="$OFILES $ofile"
    $GCC -c $file -o $ofile $CFLAGS 
done

# linking boot and kernel objects using linker
$GCC -T linker.ld -o pongos.bin -ffreestanding -O2 -nostdlib $OFILES -lgcc
mv pongos.bin isodir/boot

# creating ISO to load from grub and boot into the OS
grub-mkrescue -o pongos.iso isodir

# clean object files in cur dir
rm *.o

# run it bruhh. Default is 128M. Anything <= 2M will not be able to boot
qemu-system-i386 -cdrom pongos.iso -m 256M
