#pragma once
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "./uart_output.h"
#include "./gpios.h"
#include "./rtc.h"

extern const am_hal_pwrctrl_mcu_memory_config_t i_DefaultMcuMemCfg;
extern const am_hal_pwrctrl_sram_memcfg_t i_DefaultSRAMCfg;

void mcu_init();
void configure_power_mode(bool highPerformance);