#pragma once
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "../utils/gpios.h"

void touch_transmit(uint16_t cmd, uint32_t *buffer, int sendLen);
void touch_transceive(uint16_t cmd, uint32_t *buffer, int readLen);
void touch_init();
void touch_read_data(uint16_t *touch_x, uint16_t *touch_y, bool *touch_available);