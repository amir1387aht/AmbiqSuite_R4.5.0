#pragma once

#include "am_util.h"
#include "lvgl.h"
#include "./display.h"
#include "./touch.h"
#include "string.h"
#include "screens/menu/menu_screen.h"
#include "FreeRTOS.h"
#include "task.h"
#include "screens/main/ui.h"
#include "rtos.h"

static bool touch_available = false;

void lvgl_display_init(void* pvParameters);
void lv_rounder_cb(lv_disp_drv_t *disp_drv, lv_area_t *area);
void disp_flush(lv_disp_drv_t* display, const lv_area_t* area, lv_color_t * px_map);
void touchpad_read(lv_indev_t * indev, lv_indev_data_t * data);
void disp_flush_done(void);
void lvgl_log_cb(lv_log_level_t level, const char * buf);
void display_turn_off();
void display_turn_on();