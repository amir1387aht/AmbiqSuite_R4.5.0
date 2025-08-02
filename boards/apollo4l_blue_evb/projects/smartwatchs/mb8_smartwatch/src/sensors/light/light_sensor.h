#pragma once
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "helpers/display/display.h"
#include "../gpio/gpio.h"

#define OUTPUT_MIN 20       // Minimum brightness value
#define OUTPUT_MAX 255      // Maximum brightness value
#define RAW_MAX 65535       // Maximum raw sensor value
#define NORMAL_LIGHT 150    // Reference value for normal room lighting
#define DEFAULT_TARGET 128  // Target brightness for normal lighting (middle range)
#define TRANSITION_TIME_MS 1000 // Transition time in milliseconds
#define SENSOR_READ_TIME_MS 2000

static uint16_t RAW_TOLERANCE = 30;  // Default tolerance for raw value changes
static float SENSITIVITY = 1;     // Sensitivity factor to adjust response curve
static uint16_t last_raw = 32767;    // Default Room Light Value
static uint8_t last_output = 150;    // Default Room Light Value
static uint8_t current_brightness = 150; // Current brightness being displayed

static uint32_t ui32ModuleLightSens;
static void* LightSens_pIomHandle;

uint8_t lightSens_getNormValue(void);
void lightSens_setTolerance(uint16_t tolerance);
void lightSens_setSensitivity(float sensitivity);
void lightSens_task(void* pvParameters);
void lightSens_transmit(uint8_t cmd, uint8_t data);
void lightSens_transceive(uint8_t cmd, uint32_t *buffer, int readLen);
void lightSens_init();