#******************************************************************************
#
# Makefile - Rules for building the libraries, examples and docs.
#
# Copyright (c) 2024, Ambiq Micro, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# Third party software included in this distribution is subject to the
# additional license terms as defined in the /docs/licenses directory.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# This is part of revision release_sdk_4_5_0-a1ef3b89f9 of the AmbiqSuite Development Package.
#
#******************************************************************************
TARGET := nemagfx_masking
COMPILERNAME := Segger
PROJECT := nemagfx_masking_Segger
CONFIG := bin
AM_SoftwareRoot ?= ../../../../../..

SHELL:=/bin/bash
#### Required Executables ####
K := $(shell type -p emBuild.exe)
RM := $(shell which rm 2>/dev/null)

ifeq ($(K),)
all clean:
	$(info Tools w/$(COMPILERNAME) not installed.)
	$(RM) -rf bin
else

LIBS = ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/segger/bin/lib_nema_apollo4p_nemagfx.a
LIBS+= ../../../../bsp/segger/bin/libam_bsp.a
LIBS+= ../../../../../../mcu/apollo4p/hal/mcu/segger/bin/libam_hal.a
INCS = ../../../../bsp
INCS+= ../../../../../../mcu/apollo4p
INCS+= ../../../../../../CMSIS/ARM/Include
INCS+= ../../../../../../utils
INCS+= ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/SEGGER/AMapollo4
INCS+= ../../../../../../CMSIS/AmbiqMicro/Include
INCS+= ../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaVG
INCS+= ../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaGFX
INCS+= ../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/common
INCS+= ../src
INCS+= ../../../../../../mcu/apollo4p/hal/mcu
INCS+= ../../../../../..
INCS+= ../../../../../../devices
INCS+= ../../../../../../third_party/ThinkSi/NemaGFX_SDK/common/mem
INCS+= ../../../../../../third_party/FreeRTOSv10.5.1/Source/include
INCS+= ../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaDC
INCS+= ../../../../../../mcu/apollo4p/hal
INCS+= ../../../../../../third_party/ThinkSi/NemaGFX_SDK/NemaDC
INCS+= ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx
INCS+= ../../../../../../third_party/ThinkSi/NemaGFX_SDK/NemaGFX

all: directories $(CONFIG)/$(TARGET).bin

# Source Dependencies must be defined before they are used.
SRCS = ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/SEGGER/AMapollo4/port.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/nema_dc_hal.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/nema_event.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/nema_hal.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/nema_utils.c
SRCS += ../src/am_resources.c
SRCS += ../src/display_task.c
SRCS += ../src/graphics_power_control.c
SRCS += ../src/gui_task.c
SRCS += ../src/nemagfx_masking.c
SRCS += ../src/psram.c
SRCS += ../src/render_task.c
SRCS += ../src/rtos.c
SRCS += ../../../../../../utils/am_util_delay.c
SRCS += ../../../../../../utils/am_util_faultisr.c
SRCS += ../../../../../../utils/am_util_id.c
SRCS += ../../../../../../utils/am_util_stdio.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/event_groups.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/list.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/queue.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/tasks.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/timers.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/MemMang/heap_4.c
SRCS += ../../../../../../devices/am_devices_dc_dsi_raydium.c
SRCS += ../../../../../../devices/am_devices_dc_xspi_raydium.c

$(CONFIG)/$(TARGET).bin: $(LIBS) $(INCS) $(SRCS)
	emBuild -config "Debug" nemagfx_masking.emProject

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@


clean:
	@echo Cleaning...
	$(RM) -rf $(CONFIG)

endif
.PHONY: all clean directories

