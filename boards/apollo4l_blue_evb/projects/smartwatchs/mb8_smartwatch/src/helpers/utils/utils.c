#include "./utils.h"

const am_hal_pwrctrl_mcu_memory_config_t i_DefaultMcuMemCfg =
{
    .eCacheCfg          = AM_HAL_PWRCTRL_CACHE_ALL,
    .bRetainCache       = true,
    .eDTCMCfg           = AM_HAL_PWRCTRL_DTCM_384K,
    .eRetainDTCM        = AM_HAL_PWRCTRL_DTCM_384K,
    .bEnableNVM0        = true,
    .bRetainNVM0        = false
};

const am_hal_pwrctrl_sram_memcfg_t i_DefaultSRAMCfg =
{
    .eSRAMCfg           = AM_HAL_PWRCTRL_SRAM_ALL,
    .eActiveWithMCU     = AM_HAL_PWRCTRL_SRAM_ALL,
    .eActiveWithGFX     = AM_HAL_PWRCTRL_SRAM_ALL,
    .eActiveWithDISP    = AM_HAL_PWRCTRL_SRAM_ALL,
    .eActiveWithDSP     = AM_HAL_PWRCTRL_SRAM_ALL,
    .eSRAMRetain        = AM_HAL_PWRCTRL_SRAM_ALL
};

void mcu_init(void* pvParameters)
{
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    am_hal_pwrctrl_mcu_memory_config((am_hal_pwrctrl_mcu_memory_config_t *)&i_DefaultMcuMemCfg);
    am_hal_pwrctrl_sram_config((am_hal_pwrctrl_sram_memcfg_t *)&i_DefaultSRAMCfg);

    am_bsp_external_pwr_on();
    am_bsp_low_power_init();

    configure_power_mode(true);
    config_mcu_essential_pins();

    am_hal_interrupt_master_enable();

    uart_init();

    rtc_timer_init();

    rtc_set_time(11, 31, 30, 12, 5, 25);

    am_util_stdio_printf("MCU Inited.\n");

    lightSens_init();
    acclSens_init();

    vTaskDelete(NULL);
}

void configure_power_mode(bool highPerformance)
{
    uint32_t ui32retval = am_hal_pwrctrl_mcu_mode_select(highPerformance ? AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE : AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);
    
    if (ui32retval != AM_HAL_STATUS_SUCCESS)
    {
        am_util_stdio_printf("Error setting power mode, returned %d.\n", ui32retval);
        while(1);
    }
}

void device_lightsleep() {
    display_turn_off();
    configure_power_mode(false);
    IsDeviceInLightSleep = true;
}

void device_lightsleep_wakeup() {
    display_turn_on();
    configure_power_mode(true);
    IsDeviceInLightSleep = false;
}