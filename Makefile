# This is only a stub makefile as of now

CC=i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra

AS=i686-elf-as
#ASFLAGS none thus far

VPATH=src:build

# Link
build/bin/myos.bin : boot.o kernel.o
	mkdir build/bin
	$(CC) -T src/linker.ld -o $@ -ffreestanding -O2 -nostdlib $? -lgcc

# Assemble 
build/boot.o : boot.s
	mkdir build
	$(AS)  $? -o $@

# Compile
build/kernel.o : kernel.c
	$(CC) $(CFLAGS) -c $? -o $@


.PHONY = clean
clean :
	rm -rvf build

.PHONY = iso 
iso : build/bin/myos.bin
	mkdir build/iso
	cp build/bin/myos.bin deploy/boot/
	grub-mkrescue -o build/iso/myos.iso deploy





