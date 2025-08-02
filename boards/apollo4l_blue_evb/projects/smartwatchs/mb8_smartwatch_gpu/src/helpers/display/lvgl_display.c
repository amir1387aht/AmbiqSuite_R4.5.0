#include "./lvgl_display.h"

static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

void lvgl_display_init() {
    display_init();
    touch_init();
    lv_init();

    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, display_buffer1, display_buffer2, DRAW_BUF_SIZE);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = display_width;
    disp_drv.ver_res = display_height;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 0;
    disp_drv.rounder_cb = lv_rounder_cb;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    lv_indev_drv_register(&indev_drv);
#if LV_USE_LOG
    lv_log_register_print_cb(lvgl_log_cb);
#endif
}

void lv_rounder_cb(lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    /* 1) Align the start to an even column */
    if (area->x1 & 1) area->x1--;
    /* 2) Ensure width is even (x2 becomes odd if needed) */
    if (((area->x2 - area->x1 + 1) & 1) != 0) area->x2++;
}

void lvgl_log_cb(lv_log_level_t level, const char * buf)
{
    am_util_stdio_printf(buf);
}

void lvgl_display_update() {
    lv_timer_handler();
}

static uint32_t rtc_tick(void)
{
    return rtc_get_system_time_ms();
}

void disp_flush(lv_disp_drv_t* display, const lv_area_t* area, lv_color_t * px_map) {
    display_update_area(area, (uint8_t *)px_map, disp_flush_done);
}

void disp_flush_done() {
    lv_disp_flush_ready(&disp_drv);
}

void touchpad_read(lv_indev_t * indev, lv_indev_data_t * data) {
    touch_read_data(&data->point.x, &data->point.y, &touch_available);

    data->state = touch_available ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}