#pragma once

#include "am_util.h"
#include "lvgl.h"
#include "./display.h"
#include "../utils/gpios.h"
#include "./touch.h"
#include "./rtc.h"
#include "string.h"

static bool touch_available = false;

void lvgl_display_init();
void lvgl_display_update();
void lv_rounder_cb(lv_disp_drv_t *disp_drv, lv_area_t *area);
void disp_flush(lv_disp_drv_t* display, const lv_area_t* area, lv_color_t * px_map);
void touchpad_read(lv_indev_t * indev, lv_indev_data_t * data);
static uint32_t rtc_tick(void);
void disp_flush_done();
void lvgl_log_cb(lv_log_level_t level, const char * buf);