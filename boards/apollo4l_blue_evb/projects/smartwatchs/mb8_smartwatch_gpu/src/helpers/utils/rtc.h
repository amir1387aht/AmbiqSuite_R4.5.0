#ifndef RTC_TIMER_H
#define RTC_TIMER_H

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

// Timer and RTC configurations
#define TIMER_NUM 0

// Function to initialize the timer
void rtc_timer_init(void);

// Function to get the current system time in milliseconds
uint32_t rtc_get_system_time_ms(void);

// Timer interrupt service routine, must be defined in your main file
void am_ctimer_isr(void);

void rtc_set_time(uint32_t hour, uint32_t minute, uint32_t second, uint32_t day, uint32_t month, uint32_t year);

#endif // RTC_TIMER_H