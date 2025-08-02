#include "./rtc.h"

// Global variable to store the system time in milliseconds
static volatile uint32_t g_system_time_ms = 0;

// Global variable to store RTC time
static am_hal_rtc_time_t hal_time;

// Timer Initialization function
void rtc_timer_init(void)
{
    am_hal_timer_config_t TimerConfig;

    // Initialize the clock for the RTC (select external XTAL oscillator for RTC)
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_RTC_SEL_XTAL, 0);  // Set RTC to use external crystal oscillator
    am_hal_rtc_osc_select(AM_HAL_RTC_OSC_XT);  // Select external oscillator for RTC
    am_hal_rtc_osc_enable();  // Enable the RTC oscillator

    NVIC_SetPriority(TIMER_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(TIMER_IRQn);

    // Set up Timer #0
    am_hal_timer_default_config_set(&TimerConfig);
    TimerConfig.eInputClock = AM_HAL_TIMER_CLOCK_HFRC_DIV16;  // Use HFRC divided by 16
    TimerConfig.eFunction = AM_HAL_TIMER_FN_UPCOUNT;
    TimerConfig.ui32PatternLimit = 0;
    TimerConfig.ui32Compare1 = AM_HAL_CLKGEN_FREQ_MAX_HZ / 16 / 1000;
#ifdef APOLLO4_FPGA
    // Adjust the compare value for FPGA configurations
    TimerConfig.ui32Compare1 = (APOLLO4_FPGA * 1000000) / 16;
#endif
    am_hal_timer_config(TIMER_NUM, &TimerConfig);
    am_hal_timer_clear(TIMER_NUM);
    am_hal_timer_interrupt_enable(AM_HAL_TIMER_MASK(TIMER_NUM, AM_HAL_TIMER_COMPARE1));
}

// Timer Interrupt Service Routine (ISR)
void am_ctimer_isr(void)
{
    // Clear TimerA0 Interrupt (write to clear)
    am_hal_timer_interrupt_clear(AM_HAL_TIMER_MASK(TIMER_NUM, AM_HAL_TIMER_COMPARE1));

    // Get the RTC time
    am_hal_rtc_time_get(&hal_time);

    // Increment the system time by 1ms
    g_system_time_ms++;

    // Clear the timer
    am_hal_timer_clear(TIMER_NUM);
}

uint32_t rtc_get_system_time_ms(void)
{
    return g_system_time_ms;
}

void rtc_set_time(uint32_t hour, uint32_t minute, uint32_t second, uint32_t day, uint32_t month, uint32_t year)
{
    hal_time.ui32Hour = hour;
    hal_time.ui32Minute = minute;
    hal_time.ui32Second = second;
    hal_time.ui32Hundredths = 0;
    hal_time.ui32Weekday = am_util_time_computeDayofWeek(year, month, day);
    hal_time.ui32DayOfMonth = day;
    hal_time.ui32Month = month;
    hal_time.ui32Year = year;

    // Set the RTC time
    if (am_hal_rtc_time_set(&hal_time))
    {
        am_util_stdio_printf("Error setting RTC time!\n");
    }

    am_hal_timer_start(TIMER_NUM);
}