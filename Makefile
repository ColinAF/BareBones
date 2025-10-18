LD      := i686-elf-gcc
CC      := i686-elf-gcc
AS      := i686-elf-as

LDFLAGS := -ffreestanding -O2 -nostdlib -lgcc
WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes
CFLAGS  := -std=gnu99 -ffreestanding -O2 $(WARNINGS)

PROJDIRS := src libc
SRCFILES := $(shell find $(PROJDIRS) -type f -name '*.c')
HDRFILES := $(shell find $(PROJDIRS) -type f -name '*.h')

BUILD := build
BIN   := $(BUILD)/bin

# Mirror source tree under build/
OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))
DEPFILES := $(patsubst %.c,%.d,$(SRCFILES))

-include $(DEPFILES)

all: $(BIN)/myos.bin

# Link
$(BIN)/myos.bin : boot.o $(OBJFILES) | $(BIN)
	$(LD) -T src/linker.ld -o $@ $(LDFLAGS) $^

# Assemble 
boot.o : src/boot.s
	$(AS) $< -o $@

%.o: %.c Makefile
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# ensure bin dir exists
$(BIN):
	mkdir -p $@

.PHONY: all clean iso run
# Clean objects/deps produced beside sources
clean :
	rm -rf build
	-@$(RM) $(wildcard $(OBJFILES) $(DEPFILES))

iso: $(BIN)/myos.bin
	mkdir -p build/iso
	cp $(BIN)/myos.bin deploy/boot/
	grub-mkrescue -o build/iso/myos.iso deploy

run: iso
	qemu-system-i386 -cdrom build/iso/myos.iso
