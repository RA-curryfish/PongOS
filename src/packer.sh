#!/bin/bash

# add binary location to PATH
export PREFIX="$HOME/Academics/qemu/PongOS/opt/cross"
which -- i686-elf-as || export PATH="$PREFIX/bin:$PATH"

# creating boot assembled object
i686-elf-as asm_helper.s boot.s -o boot.o

# creating kernel compiled object
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c utils.c -o utils.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c terminal.c -o terminal.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c hal.c -o hal.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c idt.c -o idt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# linking boot and kernel objects using linker
i686-elf-gcc -T linker.ld -o pongos.bin -ffreestanding -O2 -nostdlib boot.o idt.o hal.o terminal.o kernel.o utils.o -lgcc
mv pongos.bin isodir/boot

# creating ISO to load from grub and boot into the OS
grub-mkrescue -o pongos.iso isodir

# clean object files in cur dir
rm *.o

# run it bruhh. Default is 128M. Anything <= 2M will not be able to boot
qemu-system-i386 -cdrom pongos.iso -m 256M
