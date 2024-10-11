# A Makefile for myos
LD=i686-elf-gcc
LDFLAGS=-ffreestanding -O2 -nostdlib -lgcc 

CC=i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -pedantic

AS=i686-elf-as
#ASFLAGS none thus far

VPATH=src:build

# Link
build/bin/myos.bin : boot.o kernel.o
	mkdir build/bin
	$(LD) -T src/linker.ld -o $@ $(LDFLAGS) $? 

# Assemble 
build/boot.o : boot.s
	mkdir build
	$(AS)  $? -o $@

# Compile
build/kernel.o : kernel.c
	$(CC) $(CFLAGS) -c $? -o $@


.PHONY=clean
clean :
	rm -rvf build

.PHONY=iso 
iso : build/bin/myos.bin
	mkdir build/iso
	cp build/bin/myos.bin deploy/boot/
	grub-mkrescue -o build/iso/myos.iso deploy

.PHONY=run
run : iso
	qemu-system-i386 -cdrom build/iso/myos.iso


