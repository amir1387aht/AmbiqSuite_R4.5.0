// rtos.h
#ifndef RTOS_H
#define RTOS_H

#include "am_util.h"
#include "helpers/display/lvgl_display.h"
#include "helpers/utils/utils.h"
#include "sensors/light/light_sensor.h"
#include "sensors/accl/accl_sensor.h"
#include "FreeRTOS.h"
#include "task.h"
#include "helpers/wdt/wdt.h"

#ifdef __cplusplus
extern "C" {
#endif
	extern TaskHandle_t xSetupTask;
	extern TaskHandle_t xWDTTask;
	extern TaskHandle_t xLvglTask;
	extern TaskHandle_t xLightSensTask;
	extern TaskHandle_t xAcclSensTask;

	extern void run_tasks(void);
#ifdef __cplusplus
}
#endif

#endif // RTOS_H