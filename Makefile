# This is only a stub makefile as of now

CC=i686-elf-gcc


# assemble boot.s 
i686-elf-as boot.s -o boot.o

# make kernel  
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# link
i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc


# Check for multiboot
if grub-file --is-x86-multiboot myos.bin; then
  echo multiboot confirmed
else
  echo the file is not multiboot
fi

# Make iso
grub-mkrescue -o myos.iso isodir
