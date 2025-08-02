//*****************************************************************************
//
//! @file systick_int.c
//!
//! @brief A simple example of using the SysTick interrupt.
//!
//! @addtogroup peripheral_examples Peripheral Examples
//
//! @defgroup systick_int SYSTICK Interrupt Example
//! @ingroup peripheral_examples
//! @{
//!
//! Purpose: This example is a simple demonstration of the use of the SysTick interrupt.
//!
//! If the test board has LEDs (as defined in the BSP), the example will
//! blink the board's LED0 every 1/2 second.
//! If the test board does not have LEDs, a GPIO is toggled every 1/2 second.
//!
//! Since the clock to the core is gated during sleep, whether deep sleep or
//! normal sleep, the SysTick interrupt cannot be used to wake the device.
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// Local defines
//
//*****************************************************************************
//
// Set the desired number of milliseconds between interrupts
//
#define INT_NUM_MS      2   // Number of desired ms between interrupts

//
// Compute the number of needed ticks between interrupts
//
#define SYSTICK_CNT     (AM_HAL_CLKGEN_FREQ_MAX_HZ / 1000 * INT_NUM_MS)

//
// LED (or GPIO) toggle period in MS
//
#define LED_GPIO_TOGGLE_MS  500
#define PNT_PERIOD_MS       1000

//
// Define a GPIO that could be used to toggle instead of an LED.
// This is typically defined if the BSP does not define AM_BSP_NUM_LEDS.
//
#ifndef AM_BSP_NUM_LEDS
#define INDICATOR_GPIO  0
#endif

//*****************************************************************************
//
// Globals
//
//*****************************************************************************
uint32_t g_ui32IntCount = 0;
uint32_t g_ui32IntPntCnt = 0;
uint32_t g_ui32PrintSecs = 0;
bool     g_bToggle = false;

//*****************************************************************************
//
// SysTick ISR
//
//*****************************************************************************
void
SysTick_Handler(void)
{
    g_ui32IntCount++;
    g_ui32IntPntCnt++;

    //
    // Periodically toggle the LED (or GPIO)
    //
    if ( (g_ui32IntCount * INT_NUM_MS) >= LED_GPIO_TOGGLE_MS )
    {
        g_ui32IntCount = 0;
        g_bToggle = !g_bToggle;

#ifdef AM_BSP_NUM_LEDS
        //
        // Toggle the LED.
        //
        am_devices_led_array_out(am_bsp_psLEDs, AM_BSP_NUM_LEDS, g_bToggle ? 0x1 : 0x0);
#else
        //
        // Toggle the GPIO.
        //
        am_hal_gpio_state_write(INDICATOR_GPIO,
                                g_bToggle ? AM_HAL_GPIO_OUTPUT_SET : AM_HAL_GPIO_OUTPUT_CLEAR);
#endif // AM_BSP_NUM_LEDS

#if 1
        //
        // Print time status to SWO
        //
        if ( (g_ui32IntPntCnt * INT_NUM_MS) >= PNT_PERIOD_MS )
        {
            g_ui32IntPntCnt = 0;

            //
            // Print to SWO
            //
            am_bsp_debug_printf_enable();

            g_ui32PrintSecs += (PNT_PERIOD_MS / 1000);
            am_util_stdio_printf("%4d", g_ui32PrintSecs );

            if ( ((g_ui32PrintSecs / (PNT_PERIOD_MS / 1000)) % 10) == 0 )
            {
                am_util_stdio_printf("\n");
            }

            am_bsp_debug_printf_disable();

            //
            // Restart the count every minute.
            //
            if ( (g_ui32PrintSecs / (PNT_PERIOD_MS / 1000)) >= 60 )
            {
                g_ui32PrintSecs = 0;
            }
        }
#endif
    }
} // SysTick_Handler()

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

    //
    // Initialize the printf interface for ITM output
    //
    if ( am_bsp_debug_printf_enable() )
    {
        //
        // An error occurred.
        // Just as an indication, force a 3 second delay before proceeding.
        //
        am_hal_delay_us(3000000);
    }

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("systick_int\n\n");
    am_util_stdio_printf("This example is a simple demonstration of the SysTick timer and interrupts.\n");

    //
    // Disable printf messages on ITM.
    //
    am_bsp_debug_printf_disable();

    //
    // Set up LED
    //
#ifdef AM_BSP_NUM_LEDS
    //
    // Initialize the LED array
    //
    am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);
    am_devices_led_array_out(am_bsp_psLEDs, AM_BSP_NUM_LEDS, 0x0);
#else
    //
    // If no LEDs, set up a GPIO for toggling.
    //
    am_hal_gpio_pinconfig(INDICATOR_GPIO, am_hal_gpio_pincfg_output);
#endif

    //
    // Enable interrupts to the core.
    //
    am_hal_interrupt_master_enable();

    //
    // Start Systick.
    //
    am_hal_systick_int_enable();
    NVIC_SetPriority (SysTick_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    am_hal_systick_load(SYSTICK_CNT);

    //
    // Start systick
    //
    am_hal_systick_start();

#ifdef AM_DEVICES_BLECTRLR_RESET_PIN
    //
    // For SiP packages, put the BLE Controller in reset.
    //
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
    am_hal_gpio_pinconfig(AM_DEVICES_BLECTRLR_RESET_PIN,   am_hal_gpio_pincfg_output);
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_SET);
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
#endif // AM_DEVICES_BLECTRLR_RESET_PIN

    while (1)
    {
        //
        // Spin while waiting for a SysTick interrupt
        //
    }
} // main()

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

