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
TARGET := lvgl_test
COMPILERNAME := gcc
PROJECT := lvgl_test_gcc
CONFIG := bin

SHELL:=/bin/bash

# Enable printing explicit commands with 'make VERBOSE=1'
ifneq ($(VERBOSE),1)
Q:=@
endif

#### Setup ####

TOOLCHAIN ?= arm-none-eabi
PART = apollo4p
CPU = cortex-m4
FPU = fpv4-sp-d16
# Default to FPU hardware calling convention.  However, some customers and/or
# applications may need the software calling convention.
#FABI = softfp
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

DEFINES = -DPART_$(PART)
DEFINES+= -DAM_FREERTOS
DEFINES+= -DAM_PACKAGE_BGA
DEFINES+= -DAM_PART_APOLLO4P
DEFINES+= -DAM_UTIL_FAULTISR_PRINT
DEFINES+= -DLD_LIBRARY_PATH=../../../../../third_party/ThinkSi/NemaGFX_SDK/lib
DEFINES+= -DLV_AMBIQ_FB_REFRESH_TWO
DEFINES+= -DLV_AMBIQ_FB_RESX=390
DEFINES+= -DLV_AMBIQ_FB_RESY=390
DEFINES+= -DLV_AMBIQ_FB_USE_RGB565=1
DEFINES+= -DLV_AMBIQ_FB_USE_RGB888=0
DEFINES+= -DLV_CONF_INCLUDE_SIMPLE
DEFINES+= -DLV_LVGL_H_INCLUDE_SIMPLE
DEFINES+= -DNEMAGFX_SDK_PATH=../../../../../third_party/ThinkSi/NemaGFX_SDK
DEFINES+= -D'NEMA_CUSTOM_MALLOC_INCLUDE="lv_ambiq_nema_hal.h"'
DEFINES+= -DNEMA_PLATFORM=apollo4p_nemagfx
DEFINES+= -DNEMA_USE_CUSTOM_MALLOC
DEFINES+= -DWAIT_IRQ_BINARY_SEMAPHORE=1
DEFINES+= -Dapollo4p_evb_disp_shield_rev2
DEFINES+= -Dgcc

INCLUDES = -I../../../../../..
INCLUDES+= -I../../../../../../CMSIS/ARM/Include
INCLUDES+= -I../../../../../../CMSIS/AmbiqMicro/Include
INCLUDES+= -I../../../../../../devices
INCLUDES+= -I../../../../../../mcu/apollo4p
INCLUDES+= -I../../../../../../mcu/apollo4p/hal
INCLUDES+= -I../../../../../../third_party/FreeRTOSv10.5.1/Source/include
INCLUDES+= -I../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo4
INCLUDES+= -I../../../../../../third_party/LVGL/ambiq_support
INCLUDES+= -I../../../../../../third_party/LVGL/lvgl/src
INCLUDES+= -I../../../../../../third_party/LVGL/lvgl/src/gpu
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/NemaDC
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/NemaGFX
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/common/mem
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaDC
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaGFX
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaVG
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/common
INCLUDES+= -I../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx
INCLUDES+= -I../../../../../../utils
INCLUDES+= -I../../../../bsp
INCLUDES+= -I../src

VPATH = ../../../../../../devices
VPATH+=:../../../../../../third_party/FreeRTOSv10.5.1/Source
VPATH+=:../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo4
VPATH+=:../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/MemMang
VPATH+=:../../../../../../third_party/LVGL/ambiq_support
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/core
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/draw
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/font
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/gpu
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/hal
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/layouts
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/layouts/flex
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/layouts/grid
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/libs
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/libs/bmp
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/libs/freetype
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/libs/fsdrv
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/libs/gif
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/libs/png
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/libs/qrcode
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/libs/rlottie
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/libs/sjpg
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/misc
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/others
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/others/snapshot
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/themes
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/themes/basic
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/themes/default
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/themes/mono
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/animimg
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/arc
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/bar
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/btn
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/btnmatrix
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/calendar
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/canvas
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/chart
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/checkbox
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/colorwheel
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/dropdown
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/img
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/imgbtn
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/keyboard
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/label
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/led
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/line
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/list
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/meter
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/msgbox
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/objx_templ
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/roller
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/slider
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/span
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/spinbox
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/spinner
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/switch
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/table
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/tabview
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/textarea
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/tileview
VPATH+=:../../../../../../third_party/LVGL/lvgl/src/widgets/win
VPATH+=:../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx
VPATH+=:../../../../../../utils
VPATH+=:../src

SRC = am_devices_display_generic.c
SRC += am_devices_tma525.c
SRC += am_resources.c
SRC += am_util_delay.c
SRC += am_util_faultisr.c
SRC += am_util_id.c
SRC += am_util_stdio.c
SRC += am_util_syscalls.c
SRC += display_task_cpu_only.c
SRC += display_task_fake.c
SRC += display_task_one_and_partial_fb.c
SRC += display_task_one_fb.c
SRC += display_task_two_fb.c
SRC += event_groups.c
SRC += gifdec.c
SRC += gui_task.c
SRC += heap_4.c
SRC += list.c
SRC += lodepng.c
SRC += lv_ambiq_decoder.c
SRC += lv_ambiq_font_align.c
SRC += lv_ambiq_fs.c
SRC += lv_ambiq_misc.c
SRC += lv_ambiq_nema_hal.c
SRC += lv_ambiq_touch.c
SRC += lv_anim.c
SRC += lv_anim_timeline.c
SRC += lv_animimg.c
SRC += lv_arc.c
SRC += lv_area.c
SRC += lv_async.c
SRC += lv_bar.c
SRC += lv_bidi.c
SRC += lv_bmp.c
SRC += lv_btn.c
SRC += lv_btnmatrix.c
SRC += lv_calendar.c
SRC += lv_calendar_header_arrow.c
SRC += lv_calendar_header_dropdown.c
SRC += lv_canvas.c
SRC += lv_chart.c
SRC += lv_checkbox.c
SRC += lv_color.c
SRC += lv_colorwheel.c
SRC += lv_disp.c
SRC += lv_draw_arc.c
SRC += lv_draw_blend.c
SRC += lv_draw_img.c
SRC += lv_draw_label.c
SRC += lv_draw_line.c
SRC += lv_draw_mask.c
SRC += lv_draw_rect.c
SRC += lv_draw_triangle.c
SRC += lv_dropdown.c
SRC += lv_event.c
SRC += lv_flex.c
SRC += lv_font.c
SRC += lv_font_dejavu_16_persian_hebrew.c
SRC += lv_font_fmt_txt.c
SRC += lv_font_loader.c
SRC += lv_font_montserrat_10.c
SRC += lv_font_montserrat_12.c
SRC += lv_font_montserrat_12_subpx.c
SRC += lv_font_montserrat_14.c
SRC += lv_font_montserrat_16.c
SRC += lv_font_montserrat_18.c
SRC += lv_font_montserrat_20.c
SRC += lv_font_montserrat_22.c
SRC += lv_font_montserrat_24.c
SRC += lv_font_montserrat_26.c
SRC += lv_font_montserrat_28.c
SRC += lv_font_montserrat_28_compressed.c
SRC += lv_font_montserrat_30.c
SRC += lv_font_montserrat_32.c
SRC += lv_font_montserrat_34.c
SRC += lv_font_montserrat_36.c
SRC += lv_font_montserrat_38.c
SRC += lv_font_montserrat_40.c
SRC += lv_font_montserrat_42.c
SRC += lv_font_montserrat_44.c
SRC += lv_font_montserrat_46.c
SRC += lv_font_montserrat_48.c
SRC += lv_font_montserrat_8.c
SRC += lv_font_simsun_16_cjk.c
SRC += lv_font_unscii_16.c
SRC += lv_font_unscii_8.c
SRC += lv_freetype.c
SRC += lv_fs.c
SRC += lv_fs_fatfs.c
SRC += lv_fs_posix.c
SRC += lv_fs_stdio.c
SRC += lv_fs_win32.c
SRC += lv_gc.c
SRC += lv_gif.c
SRC += lv_gpu_ambiq_nema.c
SRC += lv_grid.c
SRC += lv_group.c
SRC += lv_hal_disp.c
SRC += lv_hal_indev.c
SRC += lv_hal_tick.c
SRC += lv_img.c
SRC += lv_img_buf.c
SRC += lv_img_cache.c
SRC += lv_img_decoder.c
SRC += lv_imgbtn.c
SRC += lv_indev.c
SRC += lv_indev_scroll.c
SRC += lv_keyboard.c
SRC += lv_label.c
SRC += lv_led.c
SRC += lv_line.c
SRC += lv_list.c
SRC += lv_ll.c
SRC += lv_log.c
SRC += lv_math.c
SRC += lv_mem.c
SRC += lv_meter.c
SRC += lv_msgbox.c
SRC += lv_obj.c
SRC += lv_obj_class.c
SRC += lv_obj_draw.c
SRC += lv_obj_pos.c
SRC += lv_obj_scroll.c
SRC += lv_obj_style.c
SRC += lv_obj_style_gen.c
SRC += lv_obj_tree.c
SRC += lv_objx_templ.c
SRC += lv_png.c
SRC += lv_printf.c
SRC += lv_qrcode.c
SRC += lv_refr.c
SRC += lv_rlottie.c
SRC += lv_roller.c
SRC += lv_sjpg.c
SRC += lv_slider.c
SRC += lv_snapshot.c
SRC += lv_span.c
SRC += lv_spinbox.c
SRC += lv_spinner.c
SRC += lv_style.c
SRC += lv_style_gen.c
SRC += lv_switch.c
SRC += lv_table.c
SRC += lv_tabview.c
SRC += lv_templ.c
SRC += lv_textarea.c
SRC += lv_theme.c
SRC += lv_theme_basic.c
SRC += lv_theme_default.c
SRC += lv_theme_mono.c
SRC += lv_tileview.c
SRC += lv_timer.c
SRC += lv_tlsf.c
SRC += lv_txt.c
SRC += lv_txt_ap.c
SRC += lv_utils.c
SRC += lv_win.c
SRC += lvgl_test.c
SRC += nema_dc_hal.c
SRC += nema_event.c
SRC += nema_hal.c
SRC += nema_utils.c
SRC += port.c
SRC += qrcodegen.c
SRC += queue.c
SRC += rtos.c
SRC += tasks.c
SRC += timers.c
SRC += tjpgd.c
SRC += startup_gcc.c

CSRC = $(filter %.c,$(SRC))
ASRC = $(filter %.s,$(SRC))

OBJS = $(CSRC:%.c=$(CONFIG)/%.o)
OBJS+= $(ASRC:%.s=$(CONFIG)/%.o)

DEPS = $(CSRC:%.c=$(CONFIG)/%.d)
DEPS+= $(ASRC:%.s=$(CONFIG)/%.d)

LIBS = ../../../../../../CMSIS/ARM/Lib/ARM/libarm_cortexM4lf_math.a
LIBS += ../../../../../../mcu/apollo4p/hal/mcu/gcc/bin/libam_hal.a
LIBS += ../../../../../../third_party/LVGL/ambiq_support/gpu_lib_apollo4/gcc/bin/lvgl_ambiq_porting.a
LIBS += ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/gcc/bin/lib_nema_apollo4p_nemagfx.a
LIBS += ../../../../bsp/gcc/bin/libam_bsp.a

CFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
CFLAGS+= -ffunction-sections -fdata-sections -fomit-frame-pointer
CFLAGS+= -MMD -MP -std=c99 -Wall -g
CFLAGS+= -O0
CFLAGS+= $(DEFINES)
CFLAGS+= $(INCLUDES)
CFLAGS+= 

LFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
LFLAGS+= -nostartfiles -static
LFLAGS+= -Wl,--gc-sections,--entry,Reset_Handler,-Map,$(CONFIG)/$(TARGET).map
LFLAGS+= -Wl,--start-group -lm -lc -lgcc -lnosys $(LIBS) -Wl,--end-group
LFLAGS+= 

# Additional user specified CFLAGS
CFLAGS+=$(EXTRA_CFLAGS)

CPFLAGS = -Obinary

ODFLAGS = -S

#### Rules ####
all: directories $(CONFIG)/$(TARGET).bin

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@

$(CONFIG)/%.o: %.c $(CONFIG)/%.d
	@echo " Compiling $(COMPILERNAME) $<"
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

$(CONFIG)/%.o: %.s $(CONFIG)/%.d
	@echo " Assembling $(COMPILERNAME) $<"
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

$(CONFIG)/$(TARGET).axf: $(OBJS) $(LIBS)
	@echo " Linking $(COMPILERNAME) $@"
	$(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $@ $(OBJS) $(LFLAGS)

$(CONFIG)/$(TARGET).bin: $(CONFIG)/$(TARGET).axf
	@echo " Copying $(COMPILERNAME) $@..."
	$(Q) $(CP) $(CPFLAGS) $< $@
	$(Q) $(OD) $(ODFLAGS) $< > $(CONFIG)/$(TARGET).lst
	$(Q) $(SIZE) $(OBJS) $(LIBS) $(CONFIG)/$(TARGET).axf >$(CONFIG)/$(TARGET).size

clean:
	@echo "Cleaning..."
	$(Q) $(RM) -rf $(CONFIG)
$(CONFIG)/%.d: ;

../../../../../../mcu/apollo4p/hal/mcu/gcc/bin/libam_hal.a:
	$(MAKE) -C ../../../../../../mcu/apollo4p/hal/mcu

../../../../bsp/gcc/bin/libam_bsp.a:
	$(MAKE) -C ../../../../bsp

# Automatically include any generated dependencies
-include $(DEPS)
endif
.PHONY: all clean directories

