#include "./wdt.h"

const am_hal_wdt_config_t g_sWdtConfig =
{
    .eClockSource = AM_HAL_WDT_16HZ,
    .bInterruptEnable = false,
    .ui32InterruptValue = 0,
    .bResetEnable = true,
    .ui32ResetValue = WDT_CLOCK_HZ * WDT_TIMEOUT_SEC,
};

void wdt_init(void* pvParameters) {
    am_hal_wdt_config(AM_HAL_WDT_MCU, &g_sWdtConfig);
    am_hal_wdt_start(AM_HAL_WDT_MCU, false);

    while(1) {
        am_hal_wdt_restart(AM_HAL_WDT_MCU);
        vTaskDelay(pdMS_TO_TICKS(WDT_PET_INTERVAL_MS));
    }
}