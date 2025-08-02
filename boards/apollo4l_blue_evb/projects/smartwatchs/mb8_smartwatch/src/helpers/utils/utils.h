#pragma once
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "../uart/uart_output.h"
#include "../gpio/gpio.h"
#include "../rtc/rtc.h"
#include "../../sensors/accl/accl_sensor.h"
#include "../../sensors/light/light_sensor.h"
#include "FreeRTOS.h"
#include "task.h"

#define print(...) am_util_stdio_printf(__VA_ARGS__)
#define delay(...) am_util_delay_ms(__VA_ARGS__)

extern const am_hal_pwrctrl_mcu_memory_config_t i_DefaultMcuMemCfg;
extern const am_hal_pwrctrl_sram_memcfg_t i_DefaultSRAMCfg;

static bool IsDeviceInLightSleep = false;

void mcu_init(void* pvParameters);
void configure_power_mode(bool highPerformance);
void device_lightsleep();
void device_lightsleep_wakeup();