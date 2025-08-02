//*****************************************************************************
//
//! @file simple_pwm.c
//!
//! @brief Simple PWM Output Example
//!
//! @addtogroup peripheral_examples Peripheral Examples
//
//! @defgroup simple_pwm Simple PWM Example
//! @ingroup peripheral_examples
//! @{
//!
//! Purpose: This example shows how to setup and start a pwm output
//! This is a PWM with a fixed duty cycle.
//!
//! @note This example shows a low power operation using reduced DTCM size
//! The 8K size is very near the limit with most BSPs.
//! The standard stack size had to be slightly reduced for this example's
//! RAM use to fit in 8K.(with IAR compiler)
//!
//! //! Printing takes place over the ITM at 1M Baud.
//!
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2024, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk_4_5_0-a1ef3b89f9 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_bsp.h"
#include "am_util.h"
#include "low_power_config.h"
#include "pwm_config.h"

#ifndef AM_BSP_PWM_LED_TIMER
#define AM_BSP_PWM_LED_TIMER 0
#endif

#ifndef AM_BSP_GPIO_LED0
#define AM_BSP_GPIO_LED0 0x10000
#endif
#ifndef AM_BSP_GPIO_LED1
#define AM_BSP_GPIO_LED1 0x10000
#endif

//*****************************************************************************
//
// Macros
//
//*****************************************************************************

#define PWM_CLK     AM_HAL_TIMER_CLOCK_HFRC_DIV16
#define PWM_FREQ    200
#define FREQ_FRAC_SCALING 128

#define DEBUG_TIMER_ENABLED

static pwm_setup_t tPwmSetup;

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    //
    // Configure the board for low-power operation.
    //
    am_bsp_low_power_init();

    uint32_t ui32CfgStatus = std_cache_setup();

#if (AM_BSP_NUM_LEDS > 0)
    //
    // Configure the pins for this board.
    //
    am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);
#endif

    //
    // Initialize the printf interface for ITM/SWO output.
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Timer PWM Output example\n");
#if AM_BSP_NUM_LEDS > 0
    am_util_stdio_printf("Timer will output PWM one LED on the EVB.\n");
#endif

    if (ui32CfgStatus)
    {
        am_util_stdio_printf("Error: Cache config error: this function will hang\n");
        while (true);
    }

    if ((AM_BSP_GPIO_LED0 > 0xFFFF) && (AM_BSP_GPIO_LED1 > 0xFFFF))
    {
        am_util_stdio_printf("Warning: no output pins defined.\n");
    }

    ui32CfgStatus = deepsleep_config();
    if (ui32CfgStatus)
    {
        am_util_stdio_printf("Error: Deepsleep config error: this function will hang\n");
        while (true);
    }

    ui32CfgStatus = lowpower_memory_config();
    if (ui32CfgStatus)
    {
        am_util_stdio_printf("Error: Low Power Config Error: this function will hang\n");
        while (true);
    }

    //
    // allow time for prints to complete
    //
    am_util_delay_ms(10);

#ifdef DISABLE_PRINTING
    am_bsp_itm_printf_disable();
    //am_bsp_debug_printf_disable();
#endif

    tPwmSetup.ui32PadNumber1 = AM_BSP_GPIO_LED0;
    tPwmSetup.ui32PadNumber2 = AM_BSP_GPIO_LED2;
    tPwmSetup.ui32TimerNumber = AM_BSP_PWM_LED_TIMER;
    tPwmSetup.ui32FreqFractionalScaling = 128;
    tPwmSetup.ui32PwmFreq_x128 = PWM_FREQ * tPwmSetup.ui32FreqFractionalScaling;
    tPwmSetup.ui32DC_x100 = 85;
    tPwmSetup.ePwmClk = PWM_CLK;

    pwm_setupPwm(&tPwmSetup);

    //
    // enable global interrupts
    //
    am_hal_interrupt_master_enable();

    //
    // Start the PWM output TIMER
    //
    am_hal_timer_start(AM_BSP_PWM_LED_TIMER);

    //
    // Sleep forever
    //
    while (1)
    {
        //
        // Go to Deep Sleep.
        //

        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

    }
} // main()

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

