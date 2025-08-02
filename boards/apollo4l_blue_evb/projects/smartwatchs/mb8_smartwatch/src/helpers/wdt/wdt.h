#ifndef WDT_H
#define WDT_H

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "FreeRTOS.h"
#include "task.h"

#define WDT_CLOCK_HZ 16          // 16 Hz LFRC
#define WDT_TIMEOUT_SEC 15       // 15 seconds timeout
#define WDT_PET_INTERVAL_MS 5000 // 5 seconds

extern const am_hal_wdt_config_t g_sWdtConfig;

void wdt_init(void* pvParameters);

#endif // WDT_H