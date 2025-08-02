#pragma once
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#define INPUT 0
#define OUTPUT 1

#define LOW 0
#define HIGH 1

void pinMode(uint32_t pin, uint32_t mode);
void digitalWrite(uint32_t pin, bool state);
bool digitalRead(uint32_t pin);
void config_mcu_essential_pins();