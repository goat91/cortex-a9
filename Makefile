TOOLCHAIN = arm-none-eabi
TARGET = kernel.elf
ARMARCH = armv7-a
CC = $(TOOLCHAIN)-gcc
AS = $(TOOLCHAIN)-as
SIZE = $(TOOLCHAIN)-size
DUMP = $(TOOLCHAIN)-objdump
DTC = /usr/bin/dtc

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin
DTSDIR	 = docs

LFILE = $(SRCDIR)/linker.ld

# Compile flags here
CFLAGS   = -std=gnu11 -Wall -nostartfiles -fno-exceptions -static -g
AFLAGS   = 
LINKER   = $(CC) -o
# linking flags here
LFLAGS   = -Wall -T $(LFILE) -nostartfiles -fno-exceptions --specs=nosys.specs -static -g -lc

# dtc flags
DTC_FLAGS=-I dts -O dtb

PLATFORM?:=vexpress-a15
GDB = $(TOOLCHAIN)-gdb
QEMU = qemu-system-arm
ifeq ($(PLATFORM),vexpress-a9)
QEMU_OPTS := -M vexpress-a9 -dtb vexpress-v2p-ca9.dtb
CFLAGS += -DPLATFORM_VEXPRESS_A9 -mcpu=cortex-a9
LFLAGS += -mcpu=cortex-a9
else
QEMU_OPTS := -M vexpress-a15 -dtb vexpress-v2p-ca15-tc1.dtb
CFLAGS += -DPLATFORM_VEXPRESS_A15 -mcpu=cortex-a15
LFLAGS += -mcpu=cortex-a15
endif

QEMU_OPTS += -serial stdio -kernel


DTS_FILES := $(wildcard $(DTSDIR)/*.dts)
C_FILES := $(wildcard $(SRCDIR)/*.c)
AS_FILES := $(filter-out $(SRCDIR)/linker.ld.S,$(wildcard $(SRCDIR)/*.S))
OBJECTS_C := $(addprefix $(OBJDIR)/,$(notdir $(C_FILES:.c=.o)))
OBJECTS_S := $(addprefix $(OBJDIR)/,$(notdir $(AS_FILES:.S=.o)))
OBJECTS_ALL := $(OBJECTS_S) $(OBJECTS_C)

DTB_FILES := $(notdir $(subst .dts,.dtb,$(DTS_FILES)))
rm = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS_ALL) $(LFILE) $(DTB_FILES)
	@mkdir -p $(@D)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS_ALL)
	@echo "Linking complete!"
	@$(SIZE) $@

$(DTB_FILES):%.dtb:$(DTSDIR)/%.dts
	@$(DTC) $(DTC_FLAGS) $< -o $@

$(LFILE): $(LFILE).S
	@$(CC) $(CFLAGS) -P -E $< -o $@
	@echo "Compiled "$<" successfully!"

$(OBJECTS_ALL) : | obj

$(OBJDIR):
	@mkdir -p $@

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(OBJDIR)/%.o : $(SRCDIR)/%.S
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

qemu: 
	$(QEMU) $(QEMU_OPTS) $(BINDIR)/$(TARGET)

gdb: 
	$(GDB) $(BINDIR)/$(TARGET)

dqemu: all
	$(QEMU) -s -S $(QEMU_OPTS) $(BINDIR)/$(TARGET)

.PHONY: clean
clean:
	$(rm) $(OBJECTS_ALL) $(LFILE) $(DTB_FILES)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
