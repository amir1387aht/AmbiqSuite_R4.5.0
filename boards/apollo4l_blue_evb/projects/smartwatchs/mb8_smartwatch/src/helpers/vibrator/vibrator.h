#ifndef VIBRATOR_H
#define VIBRATOR_H

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "../gpio/gpio.h"
#include "FreeRTOS.h"
#include "task.h"

void device_vibrate(uint8_t count);
void device_vibrate_task(void* pvParameters);

#endif // VIBRATOR_H