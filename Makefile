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
ASMFILES := $(shell find $(PROJDIRS) -type f -name '*.s')
HDRFILES := $(shell find $(PROJDIRS) -type f -name '*.h')

BUILD := build
BIN   := $(BUILD)/bin

OBJFILES    := $(patsubst %.c,%.o,$(SRCFILES))
ASMOBJFILES := $(patsubst %.s,%.s.o,$(ASMFILES))
DEPFILES    := $(patsubst %.c,%.d,$(SRCFILES))

.PHONY: all clean iso run

-include $(DEPFILES)

all: $(BIN)/myos.bin

# Link
$(BIN)/myos.bin : $(ASMOBJFILES) $(OBJFILES) | $(BIN)
	$(LD) -T src/linker.ld -o $@ $(LDFLAGS) $^

# Assemble 
%.s.o : %.s
	$(AS) $< -o $@

# Compile
%.o: %.c Makefile
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Ensure bin dir exists
$(BIN):
	mkdir -p $@

clean :
	rm -rf build
	-@$(RM) $(wildcard $(OBJFILES) $(DEPFILES))

iso: $(BIN)/myos.bin
	mkdir -p build/iso
	cp $(BIN)/myos.bin deploy/boot/
	grub-mkrescue -o build/iso/myos.iso deploy

run: iso
	qemu-system-i386 -cdrom build/iso/myos.iso
