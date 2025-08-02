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
TARGET := lvgl_benchmark
COMPILERNAME := iar
PROJECT := lvgl_benchmark_iar
CONFIG := bin
AM_SoftwareRoot ?= ../../../../..

SHELL:=/bin/bash
#### Required Executables ####
K := $(shell type -p IarBuild.exe)
RM = $(shell which rm 2>/dev/null)

ifeq ($(K),)
all clean:
	$(info Tools w/$(COMPILERNAME) not installed.)
	$(RM) -rf bin
else

LIBS = ${libraries}
INCS = ${incs}

all: directories $(CONFIG)/$(TARGET).bin

# Source Dependencies must be defined before they are used.
SRCS = ../../../../../../third_party/LVGL/lvgl/src/libs/bmp/lv_bmp.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/hal/lv_hal_disp.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/hal/lv_hal_indev.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/hal/lv_hal_tick.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/img/lv_img.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/table/lv_table.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/tabview/lv_tabview.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/bar/lv_bar.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/gif/gifdec.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/gif/lv_gif.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/png/lodepng.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/png/lv_png.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/sjpg/lv_sjpg.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/sjpg/tjpgd.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/arc/lv_arc.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/animimg/lv_animimg.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/checkbox/lv_checkbox.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/spinbox/lv_spinbox.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/btn/lv_btn.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/others/snapshot/lv_snapshot.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/textarea/lv_textarea.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/switch/lv_switch.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/qrcode/lv_qrcode.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/qrcode/qrcodegen.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/win/lv_win.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/themes/default/lv_theme_default.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/chart/lv_chart.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/roller/lv_roller.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4b_nemagfx/nema_dc_hal.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4b_nemagfx/nema_event.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4b_nemagfx/nema_hal.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4b_nemagfx/nema_utils.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/spinner/lv_spinner.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/span/lv_span.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/line/lv_line.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_disp.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_event.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_group.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_indev.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_indev_scroll.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_obj.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_obj_class.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_obj_draw.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_obj_pos.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_obj_scroll.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_obj_style.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_obj_style_gen.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_obj_tree.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_refr.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/core/lv_theme.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/fsdrv/lv_fs_fatfs.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/fsdrv/lv_fs_posix.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/fsdrv/lv_fs_stdio.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/fsdrv/lv_fs_win32.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/objx_templ/lv_objx_templ.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/layouts/grid/lv_grid.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/rlottie/lv_rlottie.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_anim.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_anim_timeline.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_area.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_async.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_bidi.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_color.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_fs.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_gc.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_ll.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_log.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_math.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_mem.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_printf.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_style.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_style_gen.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_templ.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_timer.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_tlsf.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_txt.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_txt_ap.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/misc/lv_utils.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/msgbox/lv_msgbox.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/tileview/lv_tileview.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/colorwheel/lv_colorwheel.c
SRCS += ../src/am_resources.c
SRCS += ../src/gui_task.c
SRCS += ../src/lvgl_benchmark.c
SRCS += ../src/lv_demo_benchmark.c
SRCS += ../src/rtos.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/draw/lv_draw_arc.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/draw/lv_draw_blend.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/draw/lv_draw_img.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/draw/lv_draw_label.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/draw/lv_draw_line.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/draw/lv_draw_mask.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/draw/lv_draw_rect.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/draw/lv_draw_triangle.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/draw/lv_img_buf.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/draw/lv_img_cache.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/draw/lv_img_decoder.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/themes/mono/lv_theme_mono.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/canvas/lv_canvas.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/themes/basic/lv_theme_basic.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/IAR/AMapollo4/port.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/IAR/AMapollo4/portasm.s
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/dropdown/lv_dropdown.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/list/lv_list.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/label/lv_label.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/meter/lv_meter.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/slider/lv_slider.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/libs/freetype/lv_freetype.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/imgbtn/lv_imgbtn.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/keyboard/lv_keyboard.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/led/lv_led.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_dejavu_16_persian_hebrew.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_fmt_txt.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_loader.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_10.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_12.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_12_subpx.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_14.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_16.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_18.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_20.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_22.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_24.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_26.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_28.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_28_compressed.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_30.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_32.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_34.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_36.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_38.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_40.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_42.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_44.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_46.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_48.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_montserrat_8.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_simsun_16_cjk.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_unscii_16.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/font/lv_font_unscii_8.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/calendar/lv_calendar.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/calendar/lv_calendar_header_arrow.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/calendar/lv_calendar_header_dropdown.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/widgets/btnmatrix/lv_btnmatrix.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/layouts/flex/lv_flex.c
SRCS += ../../../../../../third_party/LVGL/lvgl/src/gpu/lv_gpu_ambiq_nema.c
SRCS += ../../../../../../third_party/LVGL/ambiq_support/display_task_cpu_only.c
SRCS += ../../../../../../third_party/LVGL/ambiq_support/display_task_fake.c
SRCS += ../../../../../../third_party/LVGL/ambiq_support/display_task_one_and_partial_fb.c
SRCS += ../../../../../../third_party/LVGL/ambiq_support/display_task_one_fb.c
SRCS += ../../../../../../third_party/LVGL/ambiq_support/display_task_two_fb.c
SRCS += ../../../../../../third_party/LVGL/ambiq_support/lv_ambiq_decoder.c
SRCS += ../../../../../../third_party/LVGL/ambiq_support/lv_ambiq_font_align.c
SRCS += ../../../../../../third_party/LVGL/ambiq_support/lv_ambiq_fs.c
SRCS += ../../../../../../third_party/LVGL/ambiq_support/lv_ambiq_misc.c
SRCS += ../../../../../../third_party/LVGL/ambiq_support/lv_ambiq_nema_hal.c
SRCS += ../../../../../../third_party/LVGL/ambiq_support/lv_ambiq_touch.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/event_groups.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/list.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/queue.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/tasks.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/timers.c
SRCS += ../../../../../../devices/am_devices_display_generic.c
SRCS += ../../../../../../devices/am_devices_tma525.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/MemMang/heap_4.c
SRCS += ../../../../../../utils/am_util_delay.c
SRCS += ../../../../../../utils/am_util_faultisr.c
SRCS += ../../../../../../utils/am_util_id.c
SRCS += ../../../../../../utils/am_util_stdio.c

$(CONFIG)/$(TARGET).bin: $(LIBS) $(INCS) $(SRCS)
	IarBuild.exe lvgl_benchmark.ewp -make Debug -log info

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@


clean:
	@echo Cleaning...
	IarBuild.exe lvgl_benchmark.ewp -clean Debug -log all

endif
.PHONY: all clean directories

