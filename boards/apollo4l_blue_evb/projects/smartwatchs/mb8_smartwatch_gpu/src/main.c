#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#include "helpers/utils/gpios.h"
#include "helpers/utils/uart_output.h"
#include "helpers/utils/utils.h"
#include "helpers/display/lvgl_display.h"
#include "helpers/utils/rtc.h"

#include "lvgl.h"
#include "screens/menu/menu_screen.h"

int main(void)
{
    mcu_init();

    lvgl_display_init();

    screen_menu_create();

    while (1)
    {
        lvgl_display_update();

        am_util_delay_ms(5);
    }
}