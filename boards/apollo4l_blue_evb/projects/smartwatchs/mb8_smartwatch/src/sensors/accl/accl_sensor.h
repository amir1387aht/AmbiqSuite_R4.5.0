#pragma once
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "../gpio/gpio.h"
#include "../../helpers/utils/utils.h"
#include <math.h>

static bool IsAcclSensorInitSuccessfully = false;
static uint32_t ui32ModuleAcclSens = 5;
static void* AcclSens_pIomHandle;

void acclSens_write(uint8_t reg, uint8_t value);
void acclSens_read(uint8_t reg, uint8_t* rxBuf, uint32_t len);
void acclSens_task(void* pvParameters);
void acclSens_init();