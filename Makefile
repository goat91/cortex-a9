TOOLCHAIN = arm-none-eabi
TARGET = kernel.elf
ARMARCH = armv7-a
CC = $(TOOLCHAIN)-gcc
AS = $(TOOLCHAIN)-as
SIZE = $(TOOLCHAIN)-size
DUMP = $(TOOLCHAIN)-objdump

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

LFILE = $(SRCDIR)/linker.ld

# Compile flags here
CFLAGS   = -std=gnu11 -Wall -nostartfiles -fno-exceptions -static -g
AFLAGS   = 
LINKER   = $(CC) -o
# linking flags here
LFLAGS   = -Wall -T $(LFILE) -nostartfiles -fno-exceptions --specs=nosys.specs -static -g -lc

PLATFORM?:=vexpress-a15
GDB = $(TOOLCHAIN)-gdb
QEMU = qemu-system-arm
ifeq ($(PLATFORM),vexpress-a9)
QEMU_OPTS = -M vexpress-a9 -serial mon:stdio -kernel
CFLAGS += -DPLATFORM_VEXPRESS_A9 -mcpu=cortex-a9
LFLAGS += -mcpu=cortex-a9
else
QEMU_OPTS = -M vexpress-a15 -dtb vexpress-v2p-ca15-tc1.dtb -serial stdio -kernel
CFLAGS += -DPLATFORM_VEXPRESS_A15 -mcpu=cortex-a15
LFLAGS += -mcpu=cortex-a15
endif


C_FILES := $(wildcard $(SRCDIR)/*.c)
AS_FILES := $(filter-out $(SRCDIR)/linker.ld.S,$(wildcard $(SRCDIR)/*.S))
OBJECTS_C := $(addprefix $(OBJDIR)/,$(notdir $(C_FILES:.c=.o)))
OBJECTS_S := $(addprefix $(OBJDIR)/,$(notdir $(AS_FILES:.S=.o)))
OBJECTS_ALL := $(OBJECTS_S) $(OBJECTS_C)
rm = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS_ALL) $(LFILE)
	@mkdir -p $(@D)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS_ALL)
	@echo "Linking complete!"
	@$(SIZE) $@

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
	@$(rm) $(OBJECTS_ALL) $(LFILE)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
