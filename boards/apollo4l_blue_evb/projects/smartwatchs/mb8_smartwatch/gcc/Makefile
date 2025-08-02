PLATFORM := WINDOWS
TARGET := mb8_smartwatch
COMPILERNAME := gcc
PROJECT := mb8_smartwatch_gcc
CONFIG := bin

JOBS := 16

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
DEFINES += -Dgcc

ROOT_DIR := ../../../../../..
INCLUDES = -I$(ROOT_DIR)
INCLUDES += -I$(ROOT_DIR)/CMSIS/AmbiqMicro/Include
INCLUDES += -I../../../../bsp
INCLUDES += -I$(ROOT_DIR)/CMSIS/ARM/Include
INCLUDES += -I$(ROOT_DIR)/devices
INCLUDES += -I$(ROOT_DIR)/utils
INCLUDES += -I$(ROOT_DIR)/mcu/apollo4l
INCLUDES += -I$(ROOT_DIR)/mcu/apollo4l/hal/mcu
INCLUDES += -I$(ROOT_DIR)/third_party/FreeRTOSv10.5.1/Source/include
INCLUDES += -I$(ROOT_DIR)/third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo4

SRC_DIR := ../src
LIBS_DIR := ../libs

rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

HDR_DIRS := $(sort $(dir $(call rwildcard,$(SRC_DIR)/,*.h) $(call rwildcard,$(LIBS_DIR)/,*.h)))
INCLUDES += $(addprefix -I,$(HDR_DIRS))

VPATH := $(SRC_DIR):$(LIBS_DIR):$(ROOT_DIR)/utils
VPATH += :$(ROOT_DIR)/third_party/FreeRTOSv10.5.1/Source
VPATH += :$(ROOT_DIR)/third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo4
VPATH += :$(ROOT_DIR)/third_party/FreeRTOSv10.5.1/Source/portable/MemMang

SRC_FILES := $(call rwildcard,$(SRC_DIR)/,*.c)
LIBS_FILES := $(call rwildcard,$(LIBS_DIR)/,*.c)

UTIL_SRC := am_util_delay.c am_util_faultisr.c am_util_id.c am_util_stdio.c am_util_string.c am_util_time.c
SRC_FLAT := $(UTIL_SRC) startup_gcc.c

FREERTOS_SRC := port.c queue.c tasks.c timers.c heap_4.c list.c
SRC_FLAT += $(FREERTOS_SRC)

SRC_OBJS := $(patsubst $(SRC_DIR)/%.c,$(CONFIG)/src/%.o,$(SRC_FILES))
LIBS_OBJS := $(patsubst $(LIBS_DIR)/%.c,$(CONFIG)/libs/%.o,$(LIBS_FILES))
FLAT_OBJS := $(patsubst %.c,$(CONFIG)/%.o,$(SRC_FLAT))

OBJS := $(SRC_OBJS) $(LIBS_OBJS) $(FLAT_OBJS)

DEPS := $(OBJS:.o=.d)

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

DEPENDENCY_FILE := $(CONFIG)/.build_deps

MAKEFLAGS += -j$(JOBS)

.SECONDEXPANSION:
.PRECIOUS: $(CONFIG)/%.o

all: directories $(CONFIG)/$(TARGET).bin
	@echo "Build complete using $(JOBS) parallel jobs"

directories:
	@echo "Creating output directories..."
	$(Q) $(MKDIR_P) $(CONFIG)
	$(Q) for dir in $(sort $(dir $(OBJS))); do \
		$(MKDIR_P) $$dir; \
	done
	$(Q) touch $(DEPENDENCY_FILE)

$(CONFIG)/src/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<"
	$(Q) $(MKDIR_P) $(dir $@)
	$(Q) $(CC) -c $(CFLAGS) $< -o $@
	@echo "$@: $<" >> $(DEPENDENCY_FILE)

$(CONFIG)/libs/%.o: $(LIBS_DIR)/%.c
	@echo "Compiling $<"
	$(Q) $(MKDIR_P) $(dir $@)
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

$(CONFIG)/$(TARGET).axf: $(OBJS) $(LIBS)
	@echo "Linking $(CONFIG)/$(TARGET).axf"
	$(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $(CONFIG)/$(TARGET).axf $(OBJS) $(LFLAGS)

$(CONFIG)/$(TARGET).bin: $(CONFIG)/$(TARGET).axf
	@echo "Copying binary to $(CONFIG)/$(TARGET).bin"
	$(Q) $(CP) $(CPFLAGS) $(CONFIG)/$(TARGET).axf $(CONFIG)/$(TARGET).bin
	$(Q) $(OD) $(ODFLAGS) $(CONFIG)/$(TARGET).axf > $(CONFIG)/$(TARGET).lst
	$(Q) $(SIZE) $(CONFIG)/$(TARGET).axf > $(CONFIG)/$(TARGET).size
	$(Q) $(SIZE) $(CONFIG)/$(TARGET).axf

$(ROOT_DIR)/mcu/apollo4l/hal/mcu/gcc/bin/libam_hal.a:
	@echo "Building HAL library..."
	$(Q) $(MAKE) -C $(ROOT_DIR)/mcu/apollo4l/hal/mcu

../../../../bsp/gcc/bin/libam_bsp.a:
	@echo "Building BSP library..."
	$(Q) $(MAKE) -C ../../../../bsp

clean:
	@echo "Cleaning..."
	$(Q) [ -d $(CONFIG) ] && rm -rf $(CONFIG) || true

-include $(DEPS)
-include $(DEPENDENCY_FILE)

print-debug:
	@echo "Source files from SRC_DIR:"
	@echo $(SRC_FILES)
	@echo "Source files from LIBS_DIR:"
	@echo $(LIBS_FILES)
	@echo "FreeRTOS source files:"
	@echo $(FREERTOS_SRC)
	@echo "Flat source files:"
	@echo $(SRC_FLAT)
	@echo "Object files:"
	@echo $(OBJS)
	@echo "Include directories:"
	@echo $(INCLUDES)

endif

.PHONY: all clean directories print-debug syscalls
.SUFFIXES: