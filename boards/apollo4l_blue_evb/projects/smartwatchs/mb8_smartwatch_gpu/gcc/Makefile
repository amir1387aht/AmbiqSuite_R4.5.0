#****************************************************************************** 
# Makefile - Rules for building the libraries, examples, and docs.
#******************************************************************************

TARGET := mb8_smartwatch
COMPILERNAME := gcc
PROJECT := mb8_smartwatch_gcc
CONFIG := bin

JOBS := 1

SHELL := /bin/bash

# Enable printing explicit commands with 'make VERBOSE=1'
ifneq ($(VERBOSE),1)
Q := @
endif

#### Setup ####

TOOLCHAIN ?= arm-none-eabi
PART = apollo4l
CPU = cortex-m4
FPU = fpv4-sp-d16
# Default to FPU hardware calling convention.  However, some customers and/or
# applications may need the software calling convention.
# FABI = softfp
FABI = hard

LINKER_FILE := ./linker_script.ld
STARTUP_FILE := ./startup_$(COMPILERNAME).c

#### Required Executables ####
CC = $(TOOLCHAIN)-gcc
GCC = $(TOOLCHAIN)-gcc
CPP = $(TOOLCHAIN)-cpp
LD = $(TOOLCHAIN)-ld
CP = $(TOOLCHAIN)-objcopy
OD = $(TOOLCHAIN)-objdump
RD = $(TOOLCHAIN)-readelf
AR = $(TOOLCHAIN)-ar
SIZE = $(TOOLCHAIN)-size
RM = $(shell which rm 2>/dev/null)
MKDIR_P = mkdir -p

EXECUTABLES = CC LD CP OD AR RD SIZE GCC
K := $(foreach exec,$(EXECUTABLES),\
        $(if $(shell which $($(exec)) 2>/dev/null),,\
        $(info $(exec) not found on PATH ($($(exec))).)$(exec)))
$(if $(strip $(value K)),$(info Required Program(s) $(strip $(value K)) not found))

ifneq ($(strip $(value K)),)
all clean:
	$(info Tools $(TOOLCHAIN)-$(COMPILERNAME) not installed.)
	$(RM) -rf bin
else

#### Flags and Directories ####

DEFINES = -DPART_$(PART)
DEFINES += -DAM_PART_APOLLO4L
DEFINES += -DAM_PACKAGE_BGA
DEFINES+= -DLD_LIBRARY_PATH=../../../../../third_party/ThinkSi/NemaGFX_SDK/lib
DEFINES+= -DLV_AMBIQ_FB_REFRESH_TWO
DEFINES+= -DLV_AMBIQ_FB_RESX=192
DEFINES+= -DLV_AMBIQ_FB_RESY=490
DEFINES+= -DLV_AMBIQ_FB_USE_RGB565=1
DEFINES+= -DLV_AMBIQ_FB_USE_RGB888=0
DEFINES+= -DNEMAGFX_SDK_PATH=../../../../../third_party/ThinkSi/NemaGFX_SDK
DEFINES+= -D'NEMA_CUSTOM_MALLOC_INCLUDE="lv_ambiq_nema_hal.h"'
DEFINES+= -DNEMA_PLATFORM=apollo4l_nemagfx
DEFINES += -Dgcc

# Base include paths
ROOT_DIR := ../../../../../..
INCLUDES = -I$(ROOT_DIR)
INCLUDES += -I$(ROOT_DIR)/CMSIS/AmbiqMicro/Include
INCLUDES += -I../../../../bsp
INCLUDES += -I$(ROOT_DIR)/CMSIS/ARM/Include
INCLUDES += -I$(ROOT_DIR)/devices
INCLUDES += -I$(ROOT_DIR)/utils
INCLUDES += -I$(ROOT_DIR)/mcu/apollo4l
INCLUDES += -I$(ROOT_DIR)/mcu/apollo4l/hal
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/NemaDC
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/NemaGFX
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/common/mem
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaDC
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaGFX
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/common
INCLUDES+= -I../../../../../../third_party/ThinkSi/config/apollo4l_nemagfx

SRC_DIR := ../src
LIBS_DIR := ../libs

# Define the rwildcard function to find files recursively
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

# Automatically find all subdirectories for headers in src and libs
HDR_DIRS := $(sort $(dir $(call rwildcard,$(SRC_DIR)/,*.h) $(call rwildcard,$(LIBS_DIR)/,*.h)))
INCLUDES += $(addprefix -I,$(HDR_DIRS))

# Build VPATH for source resolution - improved for faster lookups
VPATH := $(SRC_DIR):$(LIBS_DIR):$(ROOT_DIR)/utils
VPATH += :../../../../../../third_party/ThinkSi/config/apollo4l_nemagfx

# Directly find source files with their relative paths
SRC_FILES := $(call rwildcard,$(SRC_DIR)/,*.c)
LIBS_FILES := $(call rwildcard,$(LIBS_DIR)/,*.c)

# Utility sources (flat files)
UTIL_SRC := am_util_delay.c am_util_faultisr.c am_util_id.c am_util_stdio.c am_util_string.c am_util_time.c
SRC_FLAT := $(UTIL_SRC) startup_gcc.c

# Create objects for nested source files (preserving directory structure)
SRC_OBJS := $(patsubst $(SRC_DIR)/%.c,$(CONFIG)/src/%.o,$(SRC_FILES))
LIBS_OBJS := $(patsubst $(LIBS_DIR)/%.c,$(CONFIG)/libs/%.o,$(LIBS_FILES))
FLAT_OBJS := $(patsubst %.c,$(CONFIG)/%.o,$(SRC_FLAT))

# Combine all objects
OBJS := $(SRC_OBJS) $(LIBS_OBJS) $(FLAT_OBJS)

# Create dependency files
DEPS := $(OBJS:.o=.d)

# Compiler and linker flags
CFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
CFLAGS += -ffunction-sections -fdata-sections -fomit-frame-pointer
CFLAGS += -MMD -MP -std=c2x -Wall -g
CFLAGS += -O2
CFLAGS += $(DEFINES)
CFLAGS += $(INCLUDES)

LFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
LFLAGS += -nostartfiles -static
LFLAGS += -Wl,--gc-sections,--entry,Reset_Handler,-Map,$(CONFIG)/$(TARGET).map
LFLAGS += -Wl,--start-group -lm -lc -lgcc -lnosys $(LIBS) -Wl,--end-group

CPFLAGS = -Obinary
ODFLAGS = -S

DEPS = $(CSRC:%.c=$(CONFIG)/%.d)
DEPS += $(ASRC:%.s=$(CONFIG)/%.d)

LIBS = $(ROOT_DIR)/mcu/apollo4l/hal/mcu/gcc/bin/libam_hal.a
LIBS += ../../../../bsp/gcc/bin/libam_bsp.a

# Use a dependency file to track which files need to be rebuilt
DEPENDENCY_FILE := $(CONFIG)/.build_deps

# Use one-shell mode to speed up execution
.ONESHELL:

# Enable parallel build by default - force higher number of jobs
MAKEFLAGS += -j$(JOBS)

# Use secondary expansion for better parallelism
.SECONDEXPANSION:

# Load on demand to prevent recalculation of file lists
.PRECIOUS: $(CONFIG)/%.o

all: directories $(CONFIG)/$(TARGET).bin
	@echo "Build complete using $(JOBS) parallel jobs"

directories:
	@echo "Creating output directories..."
	$(Q) $(MKDIR_P) $(CONFIG) $(sort $(dir $(OBJS)))
	$(Q) touch $(DEPENDENCY_FILE)

# Rules for building object files - with path preservation
$(CONFIG)/src/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<"
	$(Q) $(CC) -c $(CFLAGS) $< -o $@
	@echo "$@: $<" >> $(DEPENDENCY_FILE)

$(CONFIG)/libs/%.o: $(LIBS_DIR)/%.c
	@echo "Compiling $<"
	$(Q) $(CC) -c $(CFLAGS) $< -o $@
	@echo "$@: $<" >> $(DEPENDENCY_FILE)

$(CONFIG)/%.o: %.c
	@echo "Compiling $<"
	$(Q) $(CC) -c $(CFLAGS) $< -o $@
	@echo "$@: $<" >> $(DEPENDENCY_FILE)

$(CONFIG)/%.o: %.s
	@echo "Assembling $<"
	$(Q) $(CC) -c $(CFLAGS) $< -o $@
	@echo "$@: $<" >> $(DEPENDENCY_FILE)

# Rules for linking - executed after all objects are built
$(CONFIG)/$(TARGET).axf: $(OBJS) $(LIBS)
	@echo "Linking $@"
	$(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $@ $(OBJS) $(LFLAGS)

$(CONFIG)/$(TARGET).bin: $(CONFIG)/$(TARGET).axf
	@echo "Copying binary to $@"
	$(Q) $(CP) $(CPFLAGS) $< $@
	$(Q) $(OD) $(ODFLAGS) $< > $(CONFIG)/$(TARGET).lst
	# Only measure the final binary size, not individual object files
	$(Q) $(SIZE) $(CONFIG)/$(TARGET).axf > $(CONFIG)/$(TARGET).size
	# Print sections sizes for quick reference
	$(Q) $(SIZE) $(CONFIG)/$(TARGET).axf

# Rules for building libraries
$(ROOT_DIR)/mcu/apollo4l/hal/mcu/gcc/bin/libam_hal.a:
	@echo "Building HAL library..."
	$(Q) $(MAKE) -C $(ROOT_DIR)/mcu/apollo4l/hal/mcu

../../../../bsp/gcc/bin/libam_bsp.a:
	@echo "Building BSP library..."
	$(Q) $(MAKE) -C ../../../../bsp

# Add a special target to handle newlib syscalls if needed
# This can eliminate the linker warnings about missing I/O functions
syscalls:
	@echo "// Minimal syscalls implementation to satisfy libc" > syscalls.c
	@echo "#include <sys/stat.h>" >> syscalls.c
	@echo "#include <errno.h>" >> syscalls.c
	@echo "#include <sys/types.h>" >> syscalls.c
	@echo "int _close(int file) { return -1; }" >> syscalls.c
	@echo "int _fstat(int file, struct stat *st) { st->st_mode = S_IFCHR; return 0; }" >> syscalls.c
	@echo "int _isatty(int file) { return 1; }" >> syscalls.c
	@echo "int _lseek(int file, int ptr, int dir) { return 0; }" >> syscalls.c
	@echo "int _read(int file, char *ptr, int len) { return 0; }" >> syscalls.c
	@echo "int _write(int file, char *ptr, int len) { return len; }" >> syscalls.c
	$(Q) $(CC) -c $(CFLAGS) syscalls.c -o $(CONFIG)/syscalls.o
	@echo "$(CONFIG)/syscalls.o: syscalls.c" >> $(DEPENDENCY_FILE)

# Clean rule
clean:
	@echo "Cleaning..."
	$(Q) [ -d $(CONFIG) ] && rm -rf $(CONFIG) || true
	$(Q) rm -f syscalls.c

# Include dependencies if they exist
-include $(DEPS)
-include $(DEPENDENCY_FILE)

# Print debug info
print-debug:
	@echo "Source files from SRC_DIR:"
	@echo $(SRC_FILES)
	@echo "Source files from LIBS_DIR:"
	@echo $(LIBS_FILES)
	@echo "Flat source files:"
	@echo $(SRC_FLAT)
	@echo "Object files:"
	@echo $(OBJS)
	@echo "Include directories:"
	@echo $(INCLUDES)

endif
# Enable parallel compilation with $(MAKE) -j options
.PHONY: all clean directories print-debug syscalls

# Disable implicit rules to speed up processing
.SUFFIXES: